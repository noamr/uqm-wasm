/*
 * .ani file extractor
 * By Serge van den Boom (svdb@stack.nl), 20021012
 * The GPL applies.
 *
 * 
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <string.h>
#include <errno.h>

#include "unani.h"

#include <png.h>


struct options {
	char *infile;
	char *outdir;
	char *prefix;
	char makeani;
	char list;
	char print;
	char verbose;
	int num_palettefiles;
	char **palettefiles;
};

int max_palettes;
png_color palettes[256][256];

index_header *readIndex(const uint8 *buf);
void printIndex(const index_header *h, const uint8 *buf, FILE *out);
void generateAni(const index_header *h, const uint8 *buf, const char *prefix,
		FILE *out);
void writeFiles(const index_header *h, const uint8 *data, const char *path,
		const char *prefix);
void parse_arguments(int argc, char *argv[], struct options *opts);
void listFrames(const index_header *h, const uint8 *buf, FILE *out);
void loadPalette(const char *filename);


int
main(int argc, char *argv[]) {
	int in;
	struct stat sb;
	uint8 *buf;
	index_header *h;
	struct options opts;
	char *prefix;

	parse_arguments(argc, argv, &opts);

	in = open(opts.infile, O_RDONLY);
	if (in == -1) {
		fprintf(stderr, "Error: Could not open file %s: %s\n", opts.infile,
				strerror(errno));
		return EXIT_FAILURE;
	}
	
	if (fstat(in, &sb) == -1) {
		perror("stat() failed");
		return EXIT_FAILURE;
	}
	
	buf = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, in, 0);
	if (buf == MAP_FAILED) {
		perror("mmap() failed");
		return EXIT_FAILURE;
	}
	close(in);

	{
		int i;

		memset(palettes, '\0', sizeof (palettes));
		max_palettes = 0;
		for (i = 0; i < opts.num_palettefiles; i++)
			loadPalette(opts.palettefiles[i]);
	}

	h = readIndex(buf);
	if (opts.print)
		printIndex(h, buf, stdout);

	if (opts.list) {
		listFrames(h, buf, stdout);
	}
	
	if (opts.prefix == NULL) {
		char *start, *end;

		start = strrchr(opts.infile, '/');
		if (start == NULL) {
			start = opts.infile;
		} else
			start++;
		end = strrchr(start, '.');
		if (end == NULL)
			end = start + strlen(start);
		prefix = alloca(end - start + 1);
		memcpy(prefix, start, end - start);
		prefix[end - start] = '\0';
	} else
		prefix = opts.prefix;

	if (opts.makeani) {
		generateAni(h, buf, prefix, stdout);
	}
	
	if (!opts.print && !opts.list && !opts.makeani) {
		size_t len;
		struct stat sb;
		if (access(opts.outdir, W_OK) == -1) {
			fprintf(stderr, "Error: Invalid destination dir %s: %s\n",
					opts.outdir, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (stat(opts.outdir, &sb) == -1) {
			fprintf(stderr, "Error: Could not stat %s: %s\n", opts.outdir,
					strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (!S_ISDIR(sb.st_mode)) {
			fprintf(stderr, "Error: %s is not a directory.\n",
					opts.outdir);
			exit(EXIT_FAILURE);
		}
				
		len = strlen(opts.outdir);
		if (opts.outdir[len - 1] == '/')
			opts.outdir[len - 1] = '\0';

		writeFiles(h, buf, opts.outdir, prefix);
	}

	// freeIndex(h);
	munmap(buf, sb.st_size);
	return EXIT_SUCCESS;
}

void
usage() {
	fprintf(stderr,
			"unani -a <infile>\n"
			"unani -p <infile>\n"
			"unani [-c <clutfile>] [-o <outdir>] [-n <prefix>] <infile>\n");
}

void
parse_arguments(int argc, char *argv[], struct options *opts) {
	char ch;
	
	memset(opts, '\0', sizeof (struct options));
	while (1) {
		ch = getopt(argc, argv, "ac:hln:o:pv");
		if (ch == -1)
			break;
		switch(ch) {
			case 'a':
				opts->makeani = 1;
				break;
			case 'c':
				opts->palettefiles = realloc(opts->palettefiles, 
						(opts->num_palettefiles + 1) * sizeof (char *));
				opts->palettefiles[opts->num_palettefiles] = optarg;
				opts->num_palettefiles++;
				break;
			case 'l':
				opts->list = 1;
				break;
			case 'o':
				opts->outdir = optarg;
				break;
			case 'n':
				opts->prefix = optarg;
				break;
			case 'p':
				opts->print = 1;
				break;
			case 'v':
				opts->verbose = 1;
				break;
			case '?':
			case 'h':
			default:
				usage();
				exit(EXIT_FAILURE);
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 1) {
		usage();
		exit(EXIT_FAILURE);
	}
	opts->infile = argv[0];
	if (opts->outdir == NULL)
		opts->outdir = ".";

}

index_header *
readIndex(const uint8 *buf) {
	index_header *h;
	const uint8 *bufptr;
	uint16 i;
	
	h = malloc(sizeof (index_header));
	
	if ((uint32) MAKE_DWORD(buf[3], buf[2], buf[1], buf[0]) != 0xffffffff ||
			MAKE_DWORD(buf[7], buf[6], buf[5], buf[4]) != 0x00000000) {
		fprintf(stderr, "File is not a valid .ani file.\n");
		exit(EXIT_FAILURE);
	}
	
	h->num_frames =
			INDEX_GET(MAKE_DWORD(buf[11], buf[10], buf[9], buf[8])) + 1;

	bufptr = buf + 12;
	h->frames = malloc(h->num_frames * sizeof (frame_desc));
	for (i = 0; i < h->num_frames; i++) {
		uint32 temp;
		temp = MAKE_DWORD(bufptr[3], bufptr[2], bufptr[1], bufptr[0]);
		h->frames[i].index = INDEX_GET(temp);
		h->frames[i].plut = (temp >> 16) & 0xff;
		h->frames[i].flags = temp >> 24;
		temp = MAKE_DWORD(bufptr[7], bufptr[6], bufptr[5], bufptr[4]);
		h->frames[i].hotspot.x = temp & 0xffff;
		h->frames[i].hotspot.y = temp >> 16;
		temp = MAKE_DWORD(bufptr[11], bufptr[10], bufptr[9], bufptr[8]);
		h->frames[i].bounds.w = temp & 0xffff;
		h->frames[i].bounds.h = temp >> 16;
		h->frames[i].start = bufptr +
				MAKE_DWORD(bufptr[15], bufptr[14], bufptr[13], bufptr[12]);
		bufptr += 16;
	}
	return h;
}

void
printIndex(const index_header *h, const uint8 *buf, FILE *out) {
	const uint8 *bufptr;
	
	fprintf(out, "0x00000008  Number of frames:        %d\n",
			h->num_frames);
	
	bufptr = buf + 0x0c;
	fprintf(out, "0x0000000c  frame info:\n");
	{
		int i;
		
		for (i = 0; i < h->num_frames; i++) {
			fprintf(out, "0x%08x    Frame %d:\n", bufptr - buf,
					h->frames[i].index);
			fprintf(out, "0x%08x      Plut index: %d\n", bufptr - buf,
					h->frames[i].plut);
			fprintf(out, "0x%08x      Flags: 0x%02x - %s%s%s%s\n",
					bufptr - buf,
					h->frames[i].flags,
					(h->frames[i].flags & FLAG_DATA_HARDWARE) ?
					"DATA_HARDWARE " : "",
					(h->frames[i].flags & FLAG_DATA_COPY) ?
					"DATA_COPY " : "",
					(h->frames[i].flags & FLAG_DATA_PACKED) ?
					"DATA_PACKED " : "",
					(h->frames[i].flags & FLAG_X_FLIP) ?
					"X_FLIP" : "");
			bufptr += 4;
			fprintf(out, "0x%08x      Hotspot: (%d, %d)\n", bufptr - buf,
					h->frames[i].hotspot.x, h->frames[i].hotspot.y);
			bufptr += 4;
			fprintf(out, "0x%08x      Size: %dx%d\n", bufptr - buf,
					h->frames[i].bounds.w, h->frames[i].bounds.h);
			bufptr += 8;
		}
	}
}

void
listFrames(const index_header *h, const uint8 *buf, FILE *out) {
	int i;
	
	for (i = 0; i < h->num_frames; i++) {
		fprintf(out, "0x%04x  %dx%d image, %s%s%s%s\n",
				h->frames[i].index,
				h->frames[i].bounds.w, h->frames[i].bounds.h,
				(h->frames[i].flags & FLAG_DATA_HARDWARE) ?
				"DATA_HARDWARE " : "",
				(h->frames[i].flags & FLAG_DATA_COPY) ?
				"DATA_COPY " : "",
				(h->frames[i].flags & FLAG_DATA_PACKED) ?
				"DATA_PACKED " : "",
				(h->frames[i].flags & FLAG_X_FLIP) ?
				"X_FLIP" : "");
	}
	(void) buf;
}

void
generateAni(const index_header *h, const uint8 *buf, const char *prefix,
		FILE *out) {
	int i;
		
	for (i = 0; i < h->num_frames; i++) {
		fprintf(out, "%s.%d.%s %d %d %d %d %d %d %d %d\n",
				prefix, h->frames[i].index,
				(h->frames[i].flags & FLAG_DATA_HARDWARE) ? "cel" :
				(h->frames[i].flags & FLAG_DATA_PACKED) ? "png" : "???",
				-1, h->frames[i].plut, 0, 0,
				h->frames[i].bounds.w, h->frames[i].bounds.h,
				h->frames[i].hotspot.x, h->frames[i].hotspot.y);
	}
	(void) buf;
}

void
writeFile(const char *file, const uint8 *data, size_t len) {
	int fd;

	// check if all the path components exist
	{
		const char *ptr;
		char path[MAXPATHLEN];
		
		ptr = file;
		if (ptr[0] == '/')
			ptr++;
		while (1) {
			ptr = strchr(ptr, '/');
			if (ptr == NULL)
				break;
			ptr++;
			memcpy(path, file, ptr - file);
			path[ptr - file] = '\0';
			if (mkdir(path, 0777) == -1) {
				if (errno == EEXIST)
					continue;
				fprintf(stderr, "Error: Could not make directory %s: %s\n",
						path, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}
	
	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd == -1) {
		perror("Could not open file for writing");
		exit(EXIT_FAILURE);
	}
	while (len > 0) {
		ssize_t written;
		written = write(fd, data, len);
		if (written < 0) {
			if (errno != EINTR) {
				perror("write failed");
				exit(1);
			}
		}
		len -= written;
		data += written;
	}
	close(fd);
}

void
writeCel(const char *filename, const frame_desc *frame, const uint8 *data) {
	writeFile(filename, data + 4,
			MAKE_DWORD(data[3], data[2], data[1], data[0]));
	(void) frame;
}

void
writePacked(const char *filename, const frame_desc *f,
		const uint8 *data) {
	// information on the packed format can be found in 3do/imageint.c
	uint8 *buf;
	const uint8 *src, *srclineend;
	uint8 *dst, *dstlineend;
	uint8 **lines;
	uint8 pack_type, num_pixels;
	int i;
	
	buf = alloca(f->bounds.w * f->bounds.h * sizeof (uint8));
	memset(buf, '\0', f->bounds.w * f->bounds.h * sizeof (uint8));
	dst = buf;
	src = data;
	lines = alloca(f->bounds.h * sizeof (uint8 *));
	for (i = 0; i < f->bounds.h; i++) {
		lines[i] = dst;
		srclineend = src + ((MAKE_WORD(src[1], src[0]) + 2) << 2);
		dstlineend = dst + f->bounds.w;
		src += 2;
		while (src < srclineend) {
			pack_type = PACK_TYPE(*src);
			num_pixels = PACK_COUNT(*src);
			src++;
			switch (pack_type) {
				case PACK_EOL:
					while (dst != dstlineend)
						*(dst++) = 0;  // rest is transparant
					break;
				case PACK_LITERAL:
					memcpy(dst, src, num_pixels);
					src += num_pixels;
					dst += num_pixels;
					break;
				case PACK_TRANS:
					memset(dst, '\0', num_pixels);
					dst += num_pixels;
					break;
				case PACK_REPEAT:
					memset(dst, *src, num_pixels);
					src++;
					dst += num_pixels;
					break;
			}
		}
	}

	{
		FILE *file;
		png_structp png_ptr;
		png_infop info_ptr;
		png_color_8 sig_bit;
		png_byte trans[1];
	
		png_ptr = png_create_write_struct
				(PNG_LIBPNG_VER_STRING, (png_voidp) NULL /* user_error_ptr */,
				NULL /* user_error_fn */, NULL /* user_warning_fn */);
		if (!png_ptr) {
			fprintf(stderr, "png_create_write_struct failed.\n");
			exit(EXIT_FAILURE);
		}
	
		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
			fprintf(stderr, "png_create_info_struct failed.\n");
			exit(EXIT_FAILURE);
		}
		if (setjmp(png_jmpbuf(png_ptr))) {
			fprintf(stderr, "png error.\n");
			png_destroy_write_struct(&png_ptr, &info_ptr);
			exit(EXIT_FAILURE);
		}

		file = fopen(filename, "wb+");
		if (file == NULL) {
			fprintf(stderr, "Could not open file '%s': %s\n",
					filename, strerror(errno));
			png_destroy_write_struct(&png_ptr, &info_ptr);
			exit(EXIT_FAILURE);
		}
		png_init_io(png_ptr, file);
		png_set_IHDR(png_ptr, info_ptr, f->bounds.w, f->bounds.h,
				8 /* bit_depth per channel */, PNG_COLOR_TYPE_PALETTE,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT);
#if 0
		sig_bit.red = 8;
		sig_bit.green = 8;
		sig_bit.blue = 8;
		png_set_sBIT(png_ptr, info_ptr, &sig_bit);
		png_set_shift(png_ptr, &sig_bit);
#endif
		png_set_PLTE(png_ptr, info_ptr, palettes[f->plut], 256);

#if 0
		trans[0] = 0;
		png_set_tRNS(png_ptr, info_ptr, trans, 1, NULL);
#endif
		png_write_info(png_ptr, info_ptr);
		png_set_packing(png_ptr);
		png_write_image(png_ptr, (png_byte **) lines);
		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
	}
}

void
writeFiles(const index_header *h, const uint8 *data, const char *path,
		const char *prefix) {
	int i;
	char filename[MAXPATHLEN];

	for (i = 0; i < h->num_frames; i++) {
		sprintf(filename, "%s/%s.%d.",
				path, prefix, h->frames[i].index);
		if (h->frames[i].flags & FLAG_DATA_HARDWARE) {
			strcat(filename, "cel");
			writeCel(filename, &h->frames[i], h->frames[i].start);
		} if (h->frames[i].flags & FLAG_DATA_PACKED) {
			strcat(filename, "png");
			writePacked(filename, &h->frames[i], h->frames[i].start);
		} else {
			fprintf(stderr, "Frame %d skipped - not supported.\n", i);
		}
	}
	(void) data;
}

void
loadPalette(const char *filename) {
	int in;
	char *buf;
	struct stat sb;
	uint16 num_entries;
	uint8 *bufptr, *pptr;
	uint8 *sizeptr;
	uint32 size;
	uint8 startpalette, endpalette;
	uint32 i, pali, k, l, m;
	uint16 cval;
	uint16 r, g, b, rtot, gtot, btot;

	in = open(filename, O_RDONLY);
	if (in == -1) {
		fprintf(stderr, "Error: Could not open file %s: %s\n", filename,
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if (fstat(in, &sb) == -1) {
		perror("stat() failed");
		exit(EXIT_FAILURE);
	}
	
	buf = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, in, 0);
	if (buf == MAP_FAILED) {
		perror("mmap() failed");
		exit(EXIT_FAILURE);
	}
	close(in);

	if ((uint32) MAKE_DWORD(buf[3], buf[2], buf[1], buf[0]) != 0xffffffff ||
			MAKE_WORD(buf[5], buf[4]) != 0x0000) {
		fprintf(stderr, "File is not a valid .ct file.\n");
		exit(EXIT_FAILURE);
	}

	num_entries = MAKE_WORD(buf[7], buf[6]);
	fprintf(stderr, "%s contains %d palette entries\n",
			filename, num_entries);
	sizeptr = buf + 8;
	bufptr = sizeptr + (num_entries + 1) * 4;
	for (i = 0; i < num_entries; i++) {
		size = MAKE_DWORD(sizeptr[3], sizeptr[2], sizeptr[1], sizeptr[0]);
		startpalette = bufptr[0];
		endpalette = bufptr[1];
		fprintf(stderr, "Entry #%d contains palette %d-%d.\n",
				i, startpalette, endpalette);
		pptr = bufptr + 2;
		for (pali = startpalette; pali <= endpalette; pali++) {
			for (k = 0; k < 32; k++) {
				cval = MAKE_WORD(pptr[1], pptr[0]);

				rtot = r = (cval >> 10) & 0x1f;  // bits 10-14
				gtot = g = (cval >> 5) & 0x1f;   // bits 5-9
				btot = b = cval & 0x1f;          // bits 0-4

				for (l = 0; l < 8; l++) {
					m = ((l << 5) + k);
					palettes[pali][m].red = rtot;
					rtot += r;
					palettes[pali][m].green = gtot;
					gtot += g;
					palettes[pali][m].blue = btot;
					btot += b;
#if 0
					if (rtot > 0xff)
						rtot = 0xff;
					if (gtot > 0xff)
						gtot = 0xff;
					if (btot > 0xff)
						btot = 0xff;
#endif
				}
				pptr += 2;
			}
		}
		sizeptr += 4;
		bufptr += size;
	}
	munmap(buf, sb.st_size);
}

