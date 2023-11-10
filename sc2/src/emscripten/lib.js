addToLibrary({
    js_persist: function () {
        FS.syncfs(/*populate=*/false, function(err) {
            if (err)
                reject(err)
            else {
                console.log("FS Synced");
                resolve();
            }
        })
    }
})