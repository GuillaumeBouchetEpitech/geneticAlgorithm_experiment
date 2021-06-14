
"use strict"

import Logger from "./utilities/Logger.js";

const onGlobalPageLoad = async () => {

    const logger = new Logger("loggerOutput");

    logger.log("[JS] page loaded");

    const onBasicGlobalPageError = (event) => {
        logger.error(`[JS] exception, event=${event}`);
    };
    window.addEventListener("error", onBasicGlobalPageError);

    const errorText = document.querySelector("#errorText");
    const renderArea = document.querySelector("#renderArea");
    const canvas = document.querySelector("#emscriptenCanvas");

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
    // buttons' logic

    const extractVarsFromUrl = () => {
        const varsRegexp = /[?&]+([^=&]+)=([^&]*)/gi;
        const vars = {};
        window.location.href.replace(varsRegexp, function (m, key, value) {
            vars[key] = value;
        });
        return vars;
    };

    const vars = extractVarsFromUrl();
    const config = {
        width: vars.width || 800,
        height: vars.height || 600,
        totalCores: vars.totalCores || 3,
        genomesPerCore: vars.genomesPerCore || 30, // <= default to 3 * 30 => 90 cars
        // const initialMemory = vars.initialMemory || 128;
    };



    renderArea.style.width = `${config.width}px`;
    renderArea.style.height = `${config.height}px`;
    canvas.width = config.width;
    canvas.height = config.height;
    canvas.style.width = `${config.width}px`;
    canvas.style.height = `${config.height}px`;



    const buttons = {
        try_with_90_cars: document.querySelector("#try_with_90_cars"),
        try_with_270_cars: document.querySelector("#try_with_270_cars"),
    };

    if (config.genomesPerCore != 30) {

        buttons.try_with_90_cars.disabled = false;
        buttons.try_with_270_cars.disabled = true;

        // add blue class
        if (!buttons.try_with_90_cars.classList.contains('blueButton'))
            buttons.try_with_90_cars.classList.add('blueButton');

        // remove grey class
        if (buttons.try_with_90_cars.classList.contains('grayButton'))
            buttons.try_with_90_cars.classList.remove('grayButton');

        // handle events
        buttons.try_with_90_cars.addEventListener("click", () => {
            // simple reload
            window.location.href = window.location.pathname + `?genomesPerCore=${30}`;
        });
    }
    else {

        buttons.try_with_90_cars.disabled = true;
        buttons.try_with_270_cars.disabled = false;

        // add red class
        if (!buttons.try_with_270_cars.classList.contains('redButton'))
            buttons.try_with_270_cars.classList.add('redButton');

        // remove grey class
        if (buttons.try_with_270_cars.classList.contains('grayButton'))
            buttons.try_with_270_cars.classList.remove('grayButton');

        // handle events
        buttons.try_with_270_cars.addEventListener("click", () => {
            // simple reload
            window.location.href = window.location.pathname + `?genomesPerCore=${90}`;
        });
    }

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

        //
        //
        // WebWorker support

        if (!window.Worker)
            throw new Error(`missing WebWorker feature`);

        logger.log("[JS] WebWorker feature => supported");

        //
        //
        // WebAssembly support

        const wasmSupported = (() => {
            try {
                if (typeof(WebAssembly) === "object" && typeof(WebAssembly.instantiate) === "function") {

                    const module = new WebAssembly.Module(Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00));

                    if (module instanceof WebAssembly.Module)
                        return new WebAssembly.Instance(module) instanceof WebAssembly.Instance;
                }
            } catch (err) {
            }
            return false;
        })();

        if (!wasmSupported)
            throw new Error("missing WebAssembly feature (unsuported)");

        logger.log("[JS] WebAssembly feature => supported");

        //
        //
        // WebGL support

        if (!window.WebGLRenderingContext)
            throw new Error("missing WebGL feature (unsuported)");

        logger.log("[JS] WebGL feature => supported");

        const renderingContextAttribs = {
            // Boolean that indicates if the canvas contains an alpha buffer.
            alpha: false,

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
        //
        // WebGL extensions support

        const webGLExtensions = webglCtx.getSupportedExtensions();

        // NOTE: incomplete names, require indexOf()
        const mandatoryWebGLExtensions = ["instanced_arrays", "vertex_array_object"];

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

    //
    //
    // multithreading support

    let supportMultithreading = (window.SharedArrayBuffer !== undefined);

    if (supportMultithreading) {

        // also check if wasm support threading (chrome mobile)

        const tmp_size = 8; // 8Mo, just for the check
        const wasmMemory = new WebAssembly.Memory({ "initial": tmp_size, "maximum": tmp_size, "shared": true });

        if (!(wasmMemory.buffer instanceof SharedArrayBuffer))
            supportMultithreading = false;
    }

    if (supportMultithreading) {
        logger.log("[JS] multithreading => supported");

        scriptFolder += "/pthread";
    }
    else {
        logger.log("[JS] multithreading => unsupported, fallback to webworker version");

        scriptFolder += "/webworker";
    }

    //
    //
    // setup the wasm module

    // const DownloadingDataRegExp = /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/;

    const Module = {
        downloadingDataRegExp: /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/,
        locateFile: (url) => { return `${scriptFolder}/${url}`; },
        print: (text) => { logger.log(`[C++] ${text}`); },
        printErr: (text) => { logger.error(`[C++] ${text}`); },
        setStatus: (text) => {

            if (!text)
                return;

            // is the current message a "Downloading data..." one?
            const capture = Module.downloadingDataRegExp.exec(text);
            if (capture) {

                // is the latest log a "Downloading data..." one?
                if (Module.downloadingDataRegExp.test(logger.peekLast()))
                    logger.popLast(); // yes, replace it

                const current = capture[1];
                const total = capture[2];
                const percent = ((current / total) * 100).toFixed(0);

                logger.log(`[JS] ${text} [${percent}%]`);
            }
            else {

                logger.log(`[JS] ${text}`);
            }
        },
        canvas: canvas,
        preinitializedWebGLContext: webglCtx,
        noExitRuntime: true,
        arguments: [
            `${config.width}`,
            `${config.height}`,
            `${config.totalCores}`,
            `${config.genomesPerCore}`
        ],
        // INITIAL_MEMORY: initialMemory * 1024 * 1024
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
            return new Promise((resolve, reject) => {
                const scriptElement = document.createElement("script");
                scriptElement.src = src;
                scriptElement.onprogress = (event) => logger.log("event", event);
                scriptElement.onload = resolve;
                scriptElement.onerror = reject;
                document.head.appendChild(scriptElement);
            });
        };

        await scriptLoadingUtility(`./${scriptFolder}/index.js`)

        logger.log("[JS] wasm script: loading successful");

        showCanvas();
    }
    catch (err) {

        logger.error(`[JS] wasm script: loading failed, err=${err.message}`);

        showErrorText(`Error<br>message:<br>${err.message}`);
    }
};

window.addEventListener("load", onGlobalPageLoad);
