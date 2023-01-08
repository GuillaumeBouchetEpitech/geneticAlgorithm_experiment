
export const isWebGL2Supported = (): boolean => {
  return !!window.WebGL2RenderingContext;
}
