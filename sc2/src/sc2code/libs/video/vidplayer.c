/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "vidplayer.h"
#include "sdl/sdlvideo.h"

// video callbacks
static void* vp_GetCanvasLine (TFB_VideoDecoder*, uint32 line);
static uint32 vp_GetTicks (TFB_VideoDecoder*);
static bool vp_SetTimer (TFB_VideoDecoder*, uint32 msecs);

TFB_VideoCallbacks vp_DecoderCBs =
{
	vp_GetCanvasLine,
	vp_GetTicks,
	vp_SetTimer
};

// audio stream callbacks
static bool vp_AudioStart (TFB_SoundSample* sample);
static void vp_AudioEnd (TFB_SoundSample* sample);
static void vp_BufferTag (TFB_SoundSample* sample, TFB_SoundTag* tag);
static void vp_QueueBuffer (TFB_SoundSample* sample, TFBSound_Object buffer);

static TFB_SoundCallbacks vp_AudioCBs =
{
	vp_AudioStart,
	NULL,
	vp_AudioEnd,
	vp_BufferTag,
	vp_QueueBuffer
};

// inter-thread param guarded by mutex
static Semaphore vp_interthread_lock = 0;
void* vp_interthread_clip = NULL;

typedef struct
{
	// standard state required by DoInput
	BOOLEAN (*InputFunc) (PVOID pInputState);
	COUNT MenuRepeatDelay;

	VIDEO_REF CurVideo;

} VIDEO_INPUT_STATE;

void
TFB_FadeClearScreen ()
{
	BYTE xform_buf[1];
	RECT r = {{0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT}};

	xform_buf[0] = FadeAllToBlack;
	SleepThreadUntil (XFormColorMap (
			(COLORMAPPTR) xform_buf, ONE_SECOND / 2));
	
	// paint black rect over screen	
	LockMutex (GraphicsLock);
	SetContext (ScreenContext);
	SetContextForeGroundColor (BUILD_COLOR (
			MAKE_RGB15 (0x0, 0x0, 0x0), 0x00));
	DrawFilledRectangle (&r);	
	UnlockMutex (GraphicsLock);

	// fade in black rect instantly
	xform_buf[0] = FadeAllToColor;
	XFormColorMap ((COLORMAPPTR) xform_buf, 0);
}

int
ac_video_play_task (void* data)
{
	Task task = (Task) data;
	volatile TFB_VideoClip* vid;
	int ret;
	uint32 want_frame;
	TimeCount TimeOut;
	int bTerm;
	uint32 clagged;

	// snatch the clip pointer and release
	vid = vp_interthread_clip;
	vp_interthread_clip = NULL;
	ClearSemaphore (vp_interthread_lock);

	LockMutex (vid->guard);
	want_frame = vid->want_frame;
	PlayMusic (vid->hAudio, FALSE, 1);
	UnlockMutex (vid->guard);

	clagged = 0;

	ret = VideoDecoder_Decode (vid->decoder);
	TimeOut = GetTimeCounter () + vid->decoder->max_frame_wait *
			ONE_SECOND / 1000;

	while (!(bTerm = Task_ReadState (task, TASK_EXIT)) && ret > 0)
	{
		// wait till its time to render next frame
		while (!(bTerm = Task_ReadState (task, TASK_EXIT))
				&& want_frame == vid->cur_frame - clagged
				&& TimeOut > GetTimeCounter ())
		{
			TaskSwitch ();
			LockMutex (vid->guard);
			want_frame = vid->want_frame;
			UnlockMutex (vid->guard);
		}

		if (bTerm)
			break;
		
		if (want_frame == vid->cur_frame - clagged)
		{	// timed out - draw the next frame
			++clagged;
		}
		else if (want_frame > vid->cur_frame - clagged
				&& want_frame <= vid->cur_frame)
		{
			// catching up
			clagged = vid->cur_frame - want_frame;
			// try again
			continue;
		}
		else if (want_frame != vid->cur_frame + 1)
		{	// out of sequence frame, let's get it
			vid->cur_frame = VideoDecoder_SeekFrame (
					vid->decoder, want_frame);
			ret = VideoDecoder_Decode (vid->decoder);
			clagged = 0;
		}
		else
		{
			clagged = 0;
		}
		vid->cur_frame = vid->decoder->cur_frame;

		// draw the frame
		if (ret > 0)
		{
			LockMutex (GraphicsLock);
			SetContext (ScreenContext);
			TFB_DrawScreen_Image (vid->frame,
					vid->dst_rect.corner.x, vid->dst_rect.corner.y,
					GSCALE_IDENTITY, NULL, TFB_SCREEN_MAIN);
			UnlockMutex (GraphicsLock);
			FlushGraphics (); // needed to prevent half-frame updates
		}

		// increase timeout with lag-count to allow audio to catch up
		TimeOut = GetTimeCounter () + vid->decoder->max_frame_wait *
				ONE_SECOND / 1000 + clagged * ONE_SECOND / 100;
		
		ret = VideoDecoder_Decode (vid->decoder);
	}
	StopMusic ();
	vid->playing = false;

	FinishTask (task);

	return 0;
}

int
video_play_task (void* data)
{
	Task task = (Task) data;
	TFB_VideoClip* vid;
	int ret;

	// snatch the clip pointer and release
	vid = vp_interthread_clip;
	vp_interthread_clip = NULL;
	ClearSemaphore (vp_interthread_lock);

	LockMutex (vid->guard);
	if (vid->hAudio)
		PlayMusic (vid->hAudio, FALSE, 1);
	UnlockMutex (vid->guard);

	// this works like so:
	//  1. you call VideoDecoder_Seek() [when necessary] and
	//     VideoDecoder_Decode()
	//  2. the decoder calls back vp_GetTicks() and vp_SetTimer()
	//     to figure out and tell you when to render the frame
	//     being decoded
	//  On a seek operation, the decoder should reset its internal
	//  clock and call vp_GetTicks() again
	//
	ret = VideoDecoder_Decode (vid->decoder);

	while (!Task_ReadState (task, TASK_EXIT) && ret > 0)
	{
		// wait till its time to render next frame
		SleepThreadUntil (vid->frame_time);
		vid->cur_frame = vid->decoder->cur_frame;

		LockMutex (GraphicsLock);
		SetContext (ScreenContext);
		TFB_DrawScreen_Image (vid->frame,
				vid->dst_rect.corner.x, vid->dst_rect.corner.y,
				GSCALE_IDENTITY, NULL, TFB_SCREEN_MAIN);
		UnlockMutex (GraphicsLock);
		FlushGraphics (); // needed to prevent half-frame updates

		ret = VideoDecoder_Decode (vid->decoder);
	}
	vid->playing = false;
	if (vid->hAudio)
		StopMusic ();

	FinishTask (task);

	return 0;
}

bool
TFB_PlayVideo (VIDEO_REF VidRef, uint32 x, uint32 y)
{
	TFB_VideoClip* vid = (TFB_VideoClip*) VidRef;
	RECT scrn_r;
	RECT clip_r = {{0, 0}, {vid->w, vid->h}};
	RECT vid_r = {{0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT}};
	RECT dr = {{x, y}, {vid->w, vid->h}};
	RECT sr;

	if (!vid)
		return false;

	// calculate the frame-source and screen-destination rects
	GetContextClipRect (&scrn_r);
	if (scrn_r.extent.width <= 0 || scrn_r.extent.height <= 0)
	{	// bad or empty rect
		scrn_r = vid_r;
	}
	else if (!BoxIntersect(&scrn_r, &vid_r, &scrn_r))
		return false; // drawing outside visible

	sr = dr;
	sr.corner.x = -sr.corner.x;
	sr.corner.y = -sr.corner.y;
	if (!BoxIntersect (&clip_r, &sr, &sr))
		return false; // drawing outside visible

	dr.corner.x += scrn_r.corner.x;
	dr.corner.y += scrn_r.corner.y;
	if (!BoxIntersect (&scrn_r, &dr, &vid->dst_rect))
		return false; // drawing outside visible

	vid->src_rect = vid->dst_rect;
	vid->src_rect.corner.x = sr.corner.x;
	vid->src_rect.corner.y = sr.corner.y;

	vid->decoder->callbacks = vp_DecoderCBs;
	vid->decoder->data = vid;
	
	vid->frame = TFB_CreateVideoImage (vid->w, vid->h);
	vid->cur_frame = -1;
	vid->want_frame = -1;

	vid->hAudio = LoadMusicFile (vid->decoder->filename);
	StopMusic ();

	if (vid->decoder->audio_synced)
	{
		TFB_SoundSample **pmus;

		if (!vid->hAudio)
		{
			fprintf (stderr, "TFB_PlayVideo: Cannot load sound-track for audio-synced video\n");
			return false;
		}

		// nasty hack for now
		LockMusicData (vid->hAudio, &pmus);
		(*pmus)->buffer_tag = HCalloc (sizeof (TFB_SoundTag) * (*pmus)->num_buffers);
		(*pmus)->callbacks = vp_AudioCBs;
		(*pmus)->data = vid;	// hijack data ;)
		UnlockMusicData (vid->hAudio);
	}

	// creation probably needs better handling
	if (!vp_interthread_lock)
		vp_interthread_lock = CreateSemaphore (1, "inter-thread param lock");
	SetSemaphore (vp_interthread_lock);
	vp_interthread_clip = vid;

	vid->playing = true;
	if (vid->decoder->audio_synced)
		vid->play_task = AssignTask (ac_video_play_task, 4096, "a/c video player");
	else
		vid->play_task = AssignTask (video_play_task, 4096, "video player");

	if (!vid->play_task)
	{
		vid->playing = false;
		//UnlockCrossThreadMutex (vid->frame_lock);
		ClearSemaphore (vp_interthread_lock);
		TFB_StopVideo (VidRef);

		return false;
	}

	return true;
}

void
TFB_StopVideo (VIDEO_REF VidRef)
{
	TFB_VideoClip* vid = (TFB_VideoClip*) VidRef;

	if (!vid)
		return;

	vid->playing = false;
	if (vid->play_task)
		ConcludeTask (vid->play_task);
	
	if (vid->hAudio)
	{
		StopMusic ();
		DestroyMusic (vid->hAudio);
		vid->hAudio = 0;
	}
	if (vid->frame) 
	{
		TFB_DrawScreen_DeleteImage (vid->frame);
		vid->frame = NULL;
	}
}

bool
TFB_VideoPlaying (VIDEO_REF VidRef)
{
	TFB_VideoClip* vid = (TFB_VideoClip*) VidRef;

	if (!vid)
		return false;

	return vid->playing;
}

BOOLEAN
TFB_DoVideoInput (PVOID pIS)
{
	VIDEO_INPUT_STATE* pVIS = (VIDEO_INPUT_STATE*) pIS;
	TFB_VideoClip* vid = (TFB_VideoClip*) pVIS->CurVideo;

	if (!pVIS->CurVideo || !TFB_VideoPlaying (pVIS->CurVideo))
		return FALSE;

	if (CurrentMenuState.select
			|| CurrentMenuState.cancel
			|| CurrentMenuState.special)
	{	// abort movie
		TFB_StopVideo (pVIS->CurVideo);
		return FALSE;
	}
	else if (CurrentMenuState.left || CurrentMenuState.right)
	{
		if (vid->decoder->audio_synced)
		{
			float newpos;
			
			LockMutex (vid->guard);
			newpos = vid->decoder->pos;
			UnlockMutex (vid->guard);

			if (CurrentMenuState.left)
				newpos -= 2;
			else if (CurrentMenuState.right)
				newpos += 1;
			if (newpos < 0)
				newpos = 0;

			LockMutex (soundSource[MUSIC_SOURCE].stream_mutex);
			SeekStream (MUSIC_SOURCE, (uint32) (newpos * 1000));
			UnlockMutex (soundSource[MUSIC_SOURCE].stream_mutex);

			TaskSwitch ();
		}
		// non a/c decoder seeking is not supported yet
	}

	return TRUE;
}

void
TFB_VideoInput (VIDEO_REF VidRef)
{
	VIDEO_INPUT_STATE vis;

	vis.MenuRepeatDelay = 0;
	vis.InputFunc = TFB_DoVideoInput;
	vis.CurVideo = VidRef;

	DoInput (&vis, TRUE);
}

static void*
vp_GetCanvasLine (TFB_VideoDecoder* decoder, uint32 line)
{
	TFB_VideoClip* vid = (TFB_VideoClip*) decoder->data;

	if (!vid)
		return NULL;
	
	return TFB_GetVideoLine (vid->frame, line);
}

static uint32
vp_GetTicks (TFB_VideoDecoder* decoder)
{
	return GetTimeCounter () * 1000 / ONE_SECOND;

	(void)decoder; // gobble up compiler warning
}

static bool
vp_SetTimer (TFB_VideoDecoder* decoder, uint32 msecs)
{
	TFB_VideoClip* vid = (TFB_VideoClip*) decoder->data;

	if (!vid)
		return false;

	// time when next frame should be displayed
	vid->frame_time = GetTimeCounter () + msecs * ONE_SECOND / 1000;
	return true;
}

static bool
vp_AudioStart (TFB_SoundSample* sample)
{
	TFB_VideoClip* vid = sample->data;

	LockMutex (vid->guard);
	vid->want_frame = SoundDecoder_GetFrame (sample->decoder);
	UnlockMutex (vid->guard);

	return true;
}

static void
vp_AudioEnd (TFB_SoundSample* sample)
{
	TFB_VideoClip* vid = sample->data;

	LockMutex (vid->guard);
	vid->want_frame = vid->decoder->frame_count; // end it
	UnlockMutex (vid->guard);
}

static void
vp_BufferTag (TFB_SoundSample* sample, TFB_SoundTag* tag)
{
	TFB_VideoClip* vid = sample->data;
	uint32 frame = (uint32) tag->data;
	
	LockMutex (vid->guard);
	vid->want_frame = frame; // let it go!
	UnlockMutex (vid->guard);
}

static void
vp_QueueBuffer (TFB_SoundSample* sample, TFBSound_Object buffer)
{
	//TFB_VideoClip* vid = sample->data;

	TFB_TagBuffer (sample, buffer,
			(void*) SoundDecoder_GetFrame (sample->decoder));
}
