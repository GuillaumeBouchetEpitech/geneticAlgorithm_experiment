
import Logger from "./utilities/Logger.js";

const onGlobalPageLoad = async () => {

    const logger = new Logger("loggerOutput");

    logger.log("[JS] page loaded");

    const onBasicGlobalPageError = (event) => {
        logger.error(`[JS] exception, event=${event}`);
    };
    window.addEventListener("error", onBasicGlobalPageError);

    const errorText = document.getElementById("errorText");
    const canvas = document.getElementById("emscriptenCanvas");

    const showErrorText = (htmlText) => {
        canvas.style.display = "none"; // hide
        errorText.innerHTML = htmlText;
        errorText.style.display = "block"; // show
    };
    const showCanvas = () => {
        errorText.style.display = "none"; // hide
        canvas.style.display = "block"; // show
    };

    //
    //
    // setup the webgl context

    const onContextCreationError = (event) => {

        event.preventDefault();

        const statusMessage = event.statusMessage || "Unknown error";
        logger.error(`[JS] could not create a WebGL context, statusMessage=${statusMessage}.`);
    };
    canvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);

    const onWebGlContextLost = (event) => {

        event.preventDefault();

        logger.error("[JS] WebGL context lost. You will need to reload the page.");
    };
    canvas.addEventListener("webglcontextlost", onWebGlContextLost, false);

    // this prevent the contextual menu to appear on a right click
    const onContextMenu = (event) => {
        event.preventDefault();
    };
    canvas.addEventListener("contextmenu", onContextMenu, false);

    let webglCtx;

    try {

        if (!window.Worker)
            throw new Error(`missing WebWorker feature`);

        logger.log("[JS] WebWorker feature => supported");

        if (!window.WebGLRenderingContext)
            throw new Error("missing WebGL feature (unsuported)");

        logger.log("[JS] WebGL feature => supported");

        const renderingContextAttribs = {
            // Boolean that indicates if the canvas contains an alpha buffer.
            alpha: true,

            // Boolean that indicates whether or not to perform anti-aliasing.
            antialias: false,

            // Boolean that indicates that the drawing buffer has a depth
            // buffer of at least 16 bits.
            depth: true,

            // Boolean that indicates if a context will be created if the
            // system performance is low.
            failIfMajorPerformanceCaveat: false,

            // A hint to the user agent indicating what configuration of GPU is
            // suitable for the WebGL context. Possible values are:
            // "default":
            //     Let the user agent decide which GPU configuration is most
            //     suitable. This is the default value.
            // "high-performance":
            //     Prioritizes rendering performance over power consumption.
            // "low-power":
            //     Prioritizes power saving over rendering performance.
            powerPreference: "high-performance",

            // Boolean that indicates that the page compositor will assume the
            // drawing buffer contains colors with pre-multiplied alpha.
            premultipliedAlpha: true,

            // If the value is true the buffers will not be cleared and will
            // preserve their values until cleared or overwritten by the author.
            preserveDrawingBuffer: true,

            // Boolean that indicates that the drawing buffer has a
            // stencil buffer of at least 8 bits.
            stencil: false,
        };

        webglCtx = (
            canvas.getContext("webgl", renderingContextAttribs) ||
            canvas.getContext("experimental-webgl", renderingContextAttribs)
        );

        if (!webglCtx)
            throw new Error("WebGL context failed (initialisation)");

        logger.log("[JS] WebGL context => initialised");

        //

        const webGLExtensions = webglCtx.getSupportedExtensions();

        // NOTE: incomplete names, require indexOf()
        const mandatoryWebGLExtensions = [ "instanced_arrays", "vertex_array_object" ];

        mandatoryWebGLExtensions.forEach((extensionName) => {

            let extensionFound = false;
            for (const webGLExtension of webGLExtensions)
                if (webGLExtension.indexOf(extensionName)) {
                    extensionFound = true;
                    break;
                }

            if (!extensionFound)
                throw new Error(`missing WebGL extension: ${extensionName}`);

            logger.log(`[JS] WebGL extension "${extensionName}" => supported`);
        });
    }
    catch (err) {

        logger.error(`[JS] dependencies check failed: message="${err.message}"`);

        return showErrorText(`
            this browser isn't compatible<br>
            error message:<br>
            => ${err.message}
        `);
    }

    let scriptFolder = "wasm";

    const supportMultithreading = (window.SharedArrayBuffer !== undefined);

    //
    //
    // extract the "genomesPerCore" value from the url

    function extractVarsFromUrl() {
        const varsRegexp = /[?&]+([^=&]+)=([^&]*)/gi;
        const vars = {};
        window.location.href.replace(varsRegexp, function (m, key, value) {
            vars[key] = value;
        });
        return vars;
    }

    const vars = extractVarsFromUrl();
    window.genomesPerCore = vars.genomesPerCore || 30; // <= default to 3 * 30 => 90 cars

    // extract the "genomesPerCore" value from the url
    //
    //

    if (supportMultithreading) {
        logger.log("[JS] multithreading => supported");

        scriptFolder += "/pthread";
    }
    else {
        logger.log("[JS] multithreading => unsupported");
        logger.log("[JS]                => fallback to webworker version");

        scriptFolder += "/webworker";
    }

    //
    //
    // setup the wasm module

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
        noExitRuntime: true,
    };

    // this is needed by the wasm side
    window.Module = Module;

    //
    //
    // attempt to handle potential error(s)

    const onAdvancedGlobalPageError = (event) => {

        // we only want this callback to be called once
        window.removeEventListener("error", onAdvancedGlobalPageError);

        logger.error(`[JS] exception, event=${event.message}`);

        Module.setStatus = (text) => {
            if (text)
                logger.error(`[JS, post-exception] ${text}`);
        };

        showErrorText(`Error<br>message:<br>${event.message}`);
    };
    // replace old global error callback by the new one
    window.removeEventListener("error", onBasicGlobalPageError);
    window.addEventListener("error", onAdvancedGlobalPageError);

    //
    //
    // downloading the demo script

    Module.setStatus("Downloading...");

    try {

        const scriptLoadingUtility = (src) => {
            return new Promise(function(resolve, reject) {
                const scriptElement = document.createElement("script");
                scriptElement.src = src;
                scriptElement.onprogress = (event) => logger.log("event", event);
                scriptElement.onload = resolve;
                scriptElement.onerror = reject;
                document.head.appendChild(scriptElement);
            });
        }

        await scriptLoadingUtility(`./${scriptFolder}/index.js`)

        logger.log("[JS] wasm script: loading successful");

        showCanvas();
    }
    catch (err) {

        logger.error(`[JS] wasm script: loading failed, err=${err.message}`);
    }
};

window.addEventListener("load", onGlobalPageLoad);

window.reloadWithDifferentCarAmount = (totalCars) => {

    // simple reload
    window.location.href = window.location.pathname + `?genomesPerCore=${totalCars}`;
};
