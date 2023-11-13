addToLibrary({
    EM_PersistFilesystem: function () {
        FS.syncfs(/*populate=*/false, function(err) {
            if (err)
                throw err;
            else {
                console.log("FS Synced");
            }
        })
    },

    EM_SetCanvasSize: function(width, height, fullscreen) {
        const {canvas} = Module;
        if (fullscreen) {
            width = 1280;
            height = 960;
            canvas.requestFullscreen();
      }
        canvas.width = width;
        canvas.height = height;
    }
})