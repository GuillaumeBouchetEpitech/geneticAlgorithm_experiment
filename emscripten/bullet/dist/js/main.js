
import Logger from "./utilities/Logger.js";

function onGlobalPageLoad() {

    const logger = new Logger("loggerOutput");

    logger.log("[JS] page loaded");

    function onBasicGlobalPageError(event) {

        logger.error(`[JS] exception, event=${event}`);
    };
    window.addEventListener("error", onBasicGlobalPageError);

    // const renderArea = document.getElementById("renderArea");
    // renderArea.style.width = "800px";
    // renderArea.style.height = "600px";

    const errorText = document.getElementById("errorText");
    // errorText.style.width = "800px";
    // errorText.style.height = "600px";

    const canvas = document.getElementById("emscriptenCanvas");
    // canvas.width = 800;
    // canvas.height = 600;
    // canvas.style.width = "800px"; // <= fix Apple retina display issue(s)
    // canvas.style.height = "600px"; // <= fix Apple retina display issue(s)

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

    // this prevent the contextual menu to appear on a right click
    function onContextMenu(event) {
        event.preventDefault();
    }
    canvas.addEventListener("contextmenu", onContextMenu, false);

    let webglCtx;

    try {

        if (!window.Worker)
            throw new Error(`missing WebWorker feature`);

        if (!window.WebGLRenderingContext)
            throw new Error("missing WebGL feature (unsuported)");

        const renderingContextAttribs = {
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

        webglCtx = (
            canvas.getContext("webgl", renderingContextAttribs) ||
            canvas.getContext("experimental-webgl", renderingContextAttribs)
        );

        if (!webglCtx)
            throw new Error("missing WebGL feature (initialisation)");

        //

        const webglExtensions = webglCtx.getSupportedExtensions();
        // for (let ii = 0; ii < webglExtensions.length; ++ii)
        //     logger.error(`[JS] ${ii}: "${webglExtensions[ii]}"`);

        const mandatoryExtensions = [
            "instanced_arrays",
            "vertex_array_object"
        ];

        for (let ii = 0; ii < mandatoryExtensions.length; ++ii) {

            const extensionName = mandatoryExtensions[ii];

            let found = false;
            for (let jj = 0; jj < webglExtensions.length; ++jj)
                if (webglExtensions[jj].indexOf(extensionName)) {
                    found = true;
                    break;
                }

            if (!found)
                throw new Error(`missing WebGL extension: ${extensionName}`);
        }

    } catch (err) {

        logger.error(`[JS] dependencies check failed: message="${err.message}"`);

        errorText.innerHTML = `
            this browser isn't compatible<br>
            error message: ${err.message}
        `;
        return;
    }

    let scriptFolder = "asm.js";

    if (window.SharedArrayBuffer !== undefined) {
    // if (false) {

        logger.log("[JS] multithreading: supported");

        scriptFolder += "/pthread";

    } else {

        logger.log("[JS] multithreading: unsupported (fallback to webworker)");

        scriptFolder += "/webworker";
    }

    const Module = {
        locateFile: function(url) { return `${scriptFolder}/${url}`; },
        print: function(text) { logger.log(`[C++] ${text}`); },
        printErr: function(text) { logger.error(`[C++] ${text}`); },
        setStatus: function(text) {
            if (text)
                logger.log(`[JS] ${text}`);
        },
        canvas: canvas,
        preinitializedWebGLContext: webglCtx,

        // noInitialRun: true,
        noExitRuntime: true,
        // onRuntimeInitialized: function() {
        //     logger.log("main");

        // //     const create_demo = Module.cwrap("create_demo", null, [])
        // //     const update_demo = Module.cwrap("update_demo", null, ["number"])

        // //     create_demo();

        // //     let prevTime = Date.now();

        // //     loop();

        // //     function loop() {

        // //         window.requestAnimationFrame(loop);

        // //         const currTime = Date.now();
        // //         const elapsedTime = Math.max(currTime - prevTime, 0);
        // //         prevTime = currTime;

        // //         update_demo(elapsedTime);
        // //     }
        // }
    };

    // this is needed by the asm.js side
    window.Module = Module;

    //
    //
    // attempt to handle potential error(s)

    function onAdvancedGlobalPageError(event) {

        logger.error(`[JS] exception, event=${event.message}`);

        Module.setStatus = function(text) {
            if (text)
                logger.error(`[JS, post-exception] ${text}`);
        };

        errorText.innerHTML = `
            Error<br>
            error message: ${event.message}
        `;
        canvas.style.display = "none";
        errorText.style.display = "inline";

        window.removeEventListener("error", onAdvancedGlobalPageError);
    };
    window.removeEventListener("error", onBasicGlobalPageError);
    window.addEventListener("error", onAdvancedGlobalPageError);

    //
    //
    // downloading the demo script

    Module.setStatus("Downloading...");

    loadScript(`./${scriptFolder}/index.js`)
        .then(() => {
            logger.log("[JS] asm.js script: loading successful");
            canvas.style.display = "inline";
        })
        .catch((err) => logger.error(`[JS] asm.js script: loading failed, err=${err.message}`));
};

window.addEventListener("load", onGlobalPageLoad);

function loadScript(src) {
    return new Promise(function(resolve, reject) {
        const scriptElement = document.createElement("script");
        scriptElement.src = src;
        scriptElement.onload = resolve;
        scriptElement.onerror = reject;
        document.head.appendChild(scriptElement);
    });
}
