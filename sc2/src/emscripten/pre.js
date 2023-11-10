if (!Module['preRun'])
    Module['preRun'] = [];

Module["preRun"].push(function() {
    addRunDependency('syncfs')

    FS.mkdir('/home/web_user/.uqm');
    FS.mount(IDBFS, {}, '/home/web_user/.uqm')
    FS.syncfs(true, function(err) {
        if (err)
            throw err
        removeRunDependency('syncfs')
        console.log("FS Synced")
    })
});