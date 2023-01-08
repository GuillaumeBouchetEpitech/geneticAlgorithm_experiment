
import Logger from "./Logger";
import {
  scriptLoadingUtility,
  getWebGl2Context,
} from "./helpers";
import {
  isWebWorkerSupported,
  isWasmSupported,
  isWebGL2Supported,
  isMultithreadingSupported,
} from "./environment";

export class Demo {

  private _canvas: HTMLCanvasElement;
  private _webglCtx?: WebGL2RenderingContext;

  private _width: number = 800;
  private _height: number = 600;

  private _isInitialised: boolean = false;
  private _isAborted: boolean = false;
  private _wasmDemoUpdateFunc: (deltaTime: number) => void;
  private _onProgress: (precent: number) => void;
  private _onError: (text: string) => void;

  constructor(
    canvas: HTMLCanvasElement,
    onProgress: (precent: number) => void,
    onError: (text: string) => void,
  ) {
    this._canvas = canvas;
    this._onProgress = onProgress;
    this._onError = onError;
  }

  async initialise(
    width: number,
    height: number,
    totalCores: number,
    genomesPerCore: number,
    inLogger: Logger,
  ): Promise<void> {

    this._width = width;
    this._height = height;

    //
    //
    // WebWorker support

    if (!isWebWorkerSupported()) {
      throw new Error("missing WebWorker feature (unsuported)");
    }
    inLogger.log("[JS][check] WebWorker feature => supported");

    //
    //
    // WebAssembly support

    if (!isWasmSupported()) {
      throw new Error("missing WebAssembly feature (unsuported)");
    }

    inLogger.log("[JS][check] WebAssembly feature => supported");

    //
    //
    // multithreading support

    const multithreadingSupported = isMultithreadingSupported();
    if (multithreadingSupported) {
      inLogger.log("[JS][check] multithreading => supported");
    } else {
      inLogger.log("[JS][check] multithreading => NOT supported");
      inLogger.log("[JS][check] => falling back to the webworker version");
    }

    //
    //
    // WebGL2 support

    if (!isWebGL2Supported()) {
      throw new Error("missing WebGL2 feature (unsuported)");
    }
    inLogger.log("[JS][check] WebGL2 feature => supported");

    //
    //
    // WebGL2 initialisation

    const onContextCreationError = (): void => {
      this._isAborted = true;
      if (this._onError)
        this._onError(`Could not create a WebGL2 context.`);
    };
    this._canvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);

    const onWebGlContextLost = (): void => {
      this._isAborted = true;
      if (this._onError)
        this._onError("WebGL2 context lost. You will need to reload the page.");
    };
    this._canvas.addEventListener("webglcontextlost", onWebGlContextLost, false);

    // this prevent the contextual menu to appear on a right click
    const onContextMenu = (event: MouseEvent): void => {
      event.preventDefault();
    };
    this._canvas.addEventListener("contextmenu", onContextMenu, false);

    this._webglCtx = getWebGl2Context(this._canvas);
    inLogger.log("[JS] WebGL2 context initialised");

    //
    //
    // multithreading support

    const wasmFolder = `wasm/${multithreadingSupported ? "pthread" : "webworker"}`;

    return new Promise((resolve, reject) => {

      //
      //
      // setup the wasm module

      const Module = {
        downloadingDataRegExp: /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/,
        lastProgressLevel: 0,
        locateFile: (url: string) => { return `${wasmFolder}/${url}`; },
        print: (text: string) => { inLogger.log(`[C++][out] ${text}`); },
        printErr: (text: string) => { inLogger.error(`[C++][err] ${text}`); },
        setStatus: (text: string) => {

          if (!text)
            return;

          // is the current message a "Downloading data..." one?
          const capture = Module.downloadingDataRegExp.exec(text);
          if (capture) {

            const current = parseFloat(capture[1]);
            const total = parseFloat(capture[2]);
            const percent = Math.floor((current / total) * 100);

            if (Module.lastProgressLevel !== percent) {
              Module.lastProgressLevel = percent;
              this._onProgress(percent);
            }
          }
          else {
            inLogger.log(`[JS][wasm][status] ${text}`);
          }
        },
        onRuntimeInitialized: () => {

          inLogger.log("[JS][wasm] loaded");
          inLogger.log("[JS][wasm] initialising");

          const wasmFunctions = {
            startDemo: (window as any).Module.cwrap('startDemo', undefined, ['number', 'number', 'number', 'number']),
            updateDemo: (window as any).Module.cwrap('updateDemo', undefined, ['number']),
          };

          this._wasmDemoUpdateFunc = wasmFunctions.updateDemo;

          wasmFunctions.startDemo(this._width, this._height, totalCores, genomesPerCore);

          this._isInitialised = true;

          inLogger.log("[JS][wasm] initialised");

          resolve();
        },
        canvas: this._canvas,
        preinitializedWebGLContext: this._webglCtx,
        noInitialRun: true,
        noExitRuntime: true,
      };

      // this is needed by the wasm side
      (window as any).Module = Module;

      inLogger.log("[JS][wasm] loading");
      scriptLoadingUtility(`./${wasmFolder}/index.js`).catch(reject);
    });
  }

  update(deltaTime: number) {
    if (!this._isInitialised || this._isAborted)
      return;

    if (this._wasmDemoUpdateFunc)
      this._wasmDemoUpdateFunc(deltaTime);
  }

  abort(): void {

    if (!this._isInitialised || this._isAborted)
      return;

    this._isAborted = true;
    const currModule = (window as any).Module;
    if (currModule) {
      currModule.setStatus = (text: string) => {
        if (text)
          console.error(`[JS][wasm][aborted] ${text}`);
      };
    }
  }
};
