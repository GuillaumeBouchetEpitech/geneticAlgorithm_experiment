
import Logger from "../Logger";

export const getWebGl2Context = (inCanvas: HTMLCanvasElement, logger: Logger, displayErrorText: (msg: string) => void): WebGL2RenderingContext => {

  // const onContextCreationError = (event: WebGLContextEvent): void => {
  //   const statusMessage = event.statusMessage || "Unknown error";
  //   displayErrorText(`Could not create a WebGL2 context, statusMessage=${statusMessage}.`);
  const onContextCreationError = (): void => {
    displayErrorText(`Could not create a WebGL2 context.`);
  };
  // inCanvas.addEventListener<WebGLContextEvent>("webglcontextcreationerror", onContextCreationError, false);
  inCanvas.addEventListener("webglcontextcreationerror", onContextCreationError, false);

  const onWebGlContextLost = (): void => {
    displayErrorText("WebGL2 context lost. You will need to reload the page.");
  };
  inCanvas.addEventListener("webglcontextlost", onWebGlContextLost, false);

  // this prevent the contextual menu to appear on a right click
  const onContextMenu = (event: MouseEvent): void => {
    event.preventDefault();
  };
  inCanvas.addEventListener("contextmenu", onContextMenu, false);

  if (!window.WebGL2RenderingContext) {
    displayErrorText("Missing WebGL2 feature (unsuported)");
    throw new Error("missing WebGL2 feature (unsuported)");
  }

  logger.log("[JS] WebGL2 feature => supported");

  const renderingContextAttribs: WebGLContextAttributes = {
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
    // suitable for the WebGL2 context. Possible values are:
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

  const webglCtx = inCanvas.getContext("webgl2", renderingContextAttribs);

  if (!webglCtx) {
    const errMsg = "WebGL2 context failed (initialisation)";
    displayErrorText(errMsg);
    throw new Error(errMsg);
  }

  logger.log("[JS] WebGL2 context => initialised");

  return webglCtx;
};
