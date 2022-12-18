
import Logger from "../Logger";

export const ensureWasmSupport = (logger: Logger, displayErrorText: (msg: string) => void): void => {
  const wasmSupported = (() => {
    try {
      if (typeof(WebAssembly) === "object" && typeof(WebAssembly.instantiate) === "function") {
        const module = new WebAssembly.Module(Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00));
        if (module instanceof WebAssembly.Module)
          return ((new WebAssembly.Instance(module)) instanceof WebAssembly.Instance);
      }
    } catch (err) {}
    return false;
  })();

  if (!wasmSupported) {
    const errMsg = "missing WebAssembly feature (unsuported)";
    displayErrorText(errMsg);
    throw new Error(errMsg);
  }

  logger.log("[JS] WebAssembly feature => supported");
};
