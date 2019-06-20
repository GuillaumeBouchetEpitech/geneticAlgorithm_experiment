
import Logger from "./utilities/Logger.js";

function onGlobalPageLoad() {

    const logger = new Logger("loggerOutput");

    logger.log("[JS] page loaded");

    function onBasicGlobalPageError(event) {

        logger.error(`[JS] exception, event=${event}`);
    };
    window.addEventListener("error", onBasicGlobalPageError);

    const canvas = document.getElementById("canvas");
    canvas.width = 800;
    canvas.height = 600;
    canvas.style.width = "800px"; // <= fix Apple retina display issue(s)
    canvas.style.height = "600px"; // <= fix Apple retina display issue(s)

    function onContextCreationError(event) {

        event.preventDefault();

        const statusMessage = event.statusMessage || "Unknown error";
        logger.error(`[JS] could not create a WebGL2 context, statusMessage=${statusMessage}.`);
    }
    canvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);

    function onWebGl2ContextLost(event) {

        event.preventDefault();
        logger.error("[JS] WebGL context lost. You will need to reload the page.");
    }

    canvas.addEventListener("webglcontextlost", onWebGl2ContextLost, false);

    // this forbid right click contextual menu
    canvas.addEventListener("contextmenu", function onContextMenu(event) {
        event.preventDefault();
    });

    let webgl2Ctx;

    try {

        if (!window.Worker)
            throw new Error(`missing WebWorker feature`);

        if (!window.WebGL2RenderingContext)
            throw new Error("missing WebGL2 feature (unsuported)");

        const contextAttribs = {
            // Boolean that indicates if the canvas contains an alpha buffer.
            alpha: true,

            // Boolean that indicates whether or not to perform anti-aliasing.
            antialias: false,

            // Boolean that indicates that the drawing buffer has a depth buffer of at least 16 bits.
            depth: true,

            // Boolean that indicates if a context will be created if the system performance is low.
            failIfMajorPerformanceCaveat: false,

            // A hint to the user agent indicating what configuration of GPU is suitable for the WebGL context. Possible values are:
            // "default": Let the user agent decide which GPU configuration is most suitable. This is the default value.
            // "high-performance": Prioritizes rendering performance over power consumption.
            // "low-power": Prioritizes power saving over rendering performance.
            powerPreference: "high-performance",

            //Boolean that indicates that the page compositor will assume the drawing buffer contains colors with pre-multiplied alpha.
            premultipliedAlpha: false,

            //If the value is true the buffers will not be cleared and will preserve their values until cleared or overwritten by the author.
            preserveDrawingBuffer: true,

            // Boolean that indicates that the drawing buffer has a stencil buffer of at least 8 bits.
            stencil: false,
        };

        webgl2Ctx = (
            canvas.getContext("webgl2", contextAttribs) ||
            canvas.getContext("experimental-webgl2", contextAttribs)
        );

        if (!webgl2Ctx)
            throw new Error("missing WebGL2 feature (initialisation)");

    } catch (err) {

        logger.error(`[JS] dependencies check failed: message="${err.message}"`);
        return;
    }

    let scriptFolder = "asm.js";

    if (window.SharedArrayBuffer !== undefined)
    {
        logger.log("[JS] multithreading: supported");

        scriptFolder += "/pthread";
    }
    else
    {
        logger.log("[JS] multithreading: unsupported (fallback to webworker)");

        scriptFolder += "/webworker";
    }

    const Module = {
        preRun: [],
        postRun: [],
        locateFile: function(url) { return `${scriptFolder}/${url}`; },
        print: function(text) { logger.log(`[C++] ${text}`); },
        printErr: function(text) { logger.error(`[C++] ${text}`); },
        canvas: canvas,
        setStatus: function (text) {
            if (text)
                logger.log(`[JS] ${text}`);
        },
        preinitializedWebGLContext: webgl2Ctx,
        // dependencies: 0,
        // totalDependencies: 0,
        // monitorRunDependencies: function(left) {

        //     if (left > 0) {

        //         if (this.totalDependencies < left) {
        //             this.totalDependencies = left;
        //             Module.setStatus(`Adding... (${this.totalDependencies})`);
        //         } else {

        //             const lastLog = logger.peekLast();
        //             if (lastLog.includes("Preparing..."))
        //                 logger.popLast();

        //             this.dependencies = left - 1;

        //             const progress = this.totalDependencies - this.dependencies;
        //             Module.setStatus(`Preparing... (${progress}/${this.totalDependencies})`);
        //         }
        //     }
        //     else if (this.dependencies > 0)
        //     {
        //         Module.setStatus("All downloads complete.");
        //     }
        // }



        // noImageDecoding: true

        // getPreloadedPackage: function(remotePackageName, remotePackageSize) {

        //     // asm.js/pthread/index.data

        //     logger.log(`[JS] remotePackageName=${remotePackageName}`);
        //     logger.log(`[JS] remotePackageSize=${remotePackageSize}`);

        //     // console.log('Runtime asking for remote package ' + remotePackageName + ', expected size ' + remotePackageSize + 'bytes.');
        //     return Module.downloadedData;
        // }
    };

    // this is needed by the asm.js side
    window.Module = Module;

    //
    //
    // attempt to handle potential error(s)

    function onAdvancedGlobalPageError(event) {

        logger.error(`[JS] exception, event=${event.message}`);

        Module.setStatus = (text) => {
            if (text)
                logger.error(`[JS, post-exception] ${text}`);
        };
    };
    window.removeEventListener("error", onBasicGlobalPageError);
    window.addEventListener("error", onAdvancedGlobalPageError);

    //
    //
    // downloading the demo script

    Module.setStatus("Downloading...");

    // downloadData(`./${scriptFolder}/index.data`)
    //     .then((data) => {

    //         Module.downloadedData = data;

    //         logger.log("[JS] asm.js data: download successful");

    //         loadScript(`./${scriptFolder}/index.js`)
    //             .then(() => { logger.log("[JS] asm.js script: loading successful"); })
    //             .catch((err) => logger.error(`[JS] asm.js script: loading failed, err=${err.message}`));
    //     })
    //     .catch((err) => logger.error(`[JS] asm.js data: download failed, err=${err.message}`));

    loadScript(`./${scriptFolder}/index.js`)
        .then(() => { logger.log("[JS] asm.js script: loading successful"); })
        .catch((err) => logger.error(`[JS] asm.js script: loading failed, err=${err.message}`));

};

window.addEventListener("load", onGlobalPageLoad);

function loadScript(src) {
    return new Promise(function(resolve, reject) {
        const scriptElement = document.createElement("script");
        scriptElement.src = src;
        scriptElement.onload = resolve;
        // scriptElement.onload = function() { resolve(xhr.response); };
        scriptElement.onerror = reject;
        document.head.appendChild(scriptElement);
    });
}

// function downloadData(src) {
//     return new Promise(function(resolve, reject) {
//         const xhr = new XMLHttpRequest();
//         xhr.open("GET", src, true);
//         xhr.responseType = "arraybuffer";
//         // xhr.onload = resolve;
//         xhr.onload = function() { resolve(xhr.response); };
//         xhr.onerror = reject;
//         xhr.send(null);
//     });
// }
