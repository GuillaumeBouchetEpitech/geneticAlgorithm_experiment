
import Logger from "./Logger";
import {
  extractVarsFromUrl,
  ensureWasmSupport,
  checkMultithreadingSupport,
  scriptLoadingUtility,
  getWebGl2Context,
} from "./helpers";

const onGlobalPageLoad = async () => {

  const logger = new Logger("#loggerOutput");

  logger.log("[JS] page loaded");

  const onBasicGlobalPageError = (event: ErrorEvent) => {
    logger.error(`[JS] exception, event=${event}`);
  };
  window.addEventListener("error", onBasicGlobalPageError);

  const errorText = document.querySelector<HTMLParagraphElement>("#errorText")!;
  const renderArea = document.querySelector<HTMLDivElement>("#renderArea")!;
  const mainCanvas = document.querySelector<HTMLCanvasElement>("#canvas")!;
  const buttons = {
    try_with_90_cars: document.querySelector<HTMLButtonElement>("#try_with_90_cars")!,
    try_with_270_cars: document.querySelector<HTMLButtonElement>("#try_with_270_cars")!,
  };

  const hide = (htmlElem: HTMLElement) => {
    if (htmlElem.style.display !== 'none')
      htmlElem.style.display = 'none';
  }
  const show = (htmlElem: HTMLElement) => {
    if (htmlElem.style.display !== 'block')
      htmlElem.style.display = 'block';
  }
  const showErrorText = (htmlText: string) => {
    errorText.innerHTML = htmlText;
    hide(mainCanvas);
    show(errorText);
  };
  const showCanvas = () => {
    hide(errorText);
    show(mainCanvas);
  };
  const resize = (width: number, height: number) => {
    renderArea.style.width = `${width}px`;
    renderArea.style.height = `${height}px`;
    mainCanvas.width = width;
    mainCanvas.height = height;
    mainCanvas.style.width = `${width}px`;
    mainCanvas.style.height = `${height}px`;
  };

  //
  //
  // buttons' logic

  const getInteger = (text: string, defaultValue: number): number => text ? parseInt(text, 10) : defaultValue;

  const urlVars = extractVarsFromUrl();
  const config: {
    width: number;
    height: number;
    totalCores: number;
    genomesPerCore: number;
  } = {
    width: getInteger(urlVars.width, 800),
    height: getInteger(urlVars.height, 600),
    totalCores: getInteger(urlVars.totalCores, 3),
    genomesPerCore: getInteger(urlVars.genomesPerCore, 30), // <= default to 3 * 30 => 90 cars
    // initialMemory: getInteger(urlVars.initialMemory, 128),
  };

  resize(config.width, config.height);

  const setupActiveButton = (currButton: HTMLButtonElement, className: string, genomesPerCore: number) => {

    currButton.disabled = false;

    // add active class
    if (!currButton.classList.contains(className))
      currButton.classList.add(className);

    // remove grey class
    if (currButton.classList.contains('grayButton'))
      currButton.classList.remove('grayButton');

    // handle events
    currButton.addEventListener("click", () => {
      // simple reload
      window.location.href = window.location.pathname + `?genomesPerCore=${genomesPerCore}`;
    });
  }

  if (config.genomesPerCore != 30) {
    buttons.try_with_270_cars.disabled = true;
    setupActiveButton(buttons.try_with_90_cars, 'blueButton', 30);
  }
  else {
    buttons.try_with_90_cars.disabled = true;
    setupActiveButton(buttons.try_with_270_cars, 'redButton', 90);
  }

  //
  //
  // setup the webgl context

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

    ensureWasmSupport(
      logger,
      (errMsg) => {
        logger.error(`[JS] ${errMsg}`);
        showErrorText(`
          this browser isn't compatible<br>
          error message:<br>
          => ${errMsg}
        `);
      });

    //
    //
    // WebGL support

    webglCtx = getWebGl2Context(
      mainCanvas,
      logger,
      (errMsg) => {
        logger.error(`[JS] ${errMsg}`);
        showErrorText(`
          this browser isn't compatible<br>
          error message:<br>
          => ${errMsg}
        `);
      });
  }
  catch (err) {
    logger.error(`[JS] dependencies check failed: message="${err.message}"`);
  }

  //
  //
  // multithreading support

  const multithreadingSupported = checkMultithreadingSupport();
  if (multithreadingSupported) {
    logger.log("[JS] multithreading => supported");
  } else {
    logger.log("[JS] multithreading => unsupported, fallback to webworker version");
  }

  const scriptFolder = "wasm/" + multithreadingSupported ? "pthread" : "webworker";

  //
  //
  // setup the wasm module

  const Module = {
    downloadingDataRegExp: /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/,
    lastProgressLevel: 0,
    locateFile: (url: string) => { return `${scriptFolder}/${url}`; },
    print: (text: string) => { logger.log(`[C++] ${text}`); },
    printErr: (text: string) => { logger.error(`[C++] ${text}`); },
    setStatus: (text: string) => {

      if (!text)
        return;

      // is the current message a "Downloading data..." one?
      const capture = Module.downloadingDataRegExp.exec(text);
      if (capture) {

        // is the latest log a "Downloading data..." one?
        if (logger.size > 0 && Module.downloadingDataRegExp.test(logger.peekLast()!))
          logger.popLast(); // yes, replace it

        const current = parseFloat(capture[1]);
        const total = parseFloat(capture[2]);
        const percent = ((current / total) * 100);

        if (Module.lastProgressLevel !== percent) {
          Module.lastProgressLevel = percent;

          const statusMsg = `${text} [${percent.toFixed(0)}%]`;
          logger.log(`[JS] ${statusMsg}`);
          showErrorText(statusMsg);
        }
      }
      else {
        logger.log(`[JS] ${text}`);
      }
    },
    onRuntimeInitialized: () => {

      console.log("=> onRuntimeInitialized()");

      const wasmFunctions = {
        startDemo: (window as any).Module.cwrap('startDemo', undefined, ['number', 'number', 'number', 'number']),
        updateDemo: (window as any).Module.cwrap('updateDemo', undefined, ['number']),
      };

      wasmFunctions.startDemo(config.width, config.height, config.totalCores, config.genomesPerCore);

      showCanvas();

      const deltaTime = Math.floor(1000 / 30);

      const onFrame = () => {
        // window.requestAnimationFrame(onFrame);
        setTimeout(onFrame, deltaTime);

        wasmFunctions.updateDemo(deltaTime);
      };
      onFrame();

    },
    canvas: mainCanvas,
    preinitializedWebGLContext: webglCtx,
    // noExitRuntime: true,
    // arguments: [
    //   `${config.width}`,
    //   `${config.height}`,
    //   `${config.totalCores}`,
    //   `${config.genomesPerCore}`
    // ],
    // INITIAL_MEMORY: initialMemory * 1024 * 1024
    noInitialRun: true,
    noExitRuntime: true,
  };

  // this is needed by the wasm side
  (window as any).Module = Module;

  //
  //
  // attempt to handle potential error(s)

  const onAdvancedGlobalPageError = (event: ErrorEvent) => {

    // we only want this callback to be called once
    window.removeEventListener("error", onAdvancedGlobalPageError);

    logger.error(`[JS] exception, event=${event.message}`);

    Module.setStatus = (text: string) => {
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
    await scriptLoadingUtility(`./${scriptFolder}/index.js`);
    logger.log("[JS] wasm script: loading successful");
  }
  catch {
    logger.error("[JS] wasm script: loading failed");
    showErrorText("Error<br>wasm script: loading failed.");
  }
};

window.addEventListener("load", onGlobalPageLoad);
