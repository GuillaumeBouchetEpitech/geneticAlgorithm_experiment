
export const isWebWorkerSupported = (): boolean => {
  return !!window.Worker;
};

