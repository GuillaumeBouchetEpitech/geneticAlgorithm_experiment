
export const isMultithreadingSupported = (): boolean => {

  const initialTest = ((window as any).SharedArrayBuffer !== undefined);

  if (initialTest) {

    // also check if wasm support threading (chrome mobile)

    const tmpMemorySize = 8; // 8Mo, just for the check

    const opts: WebAssembly.MemoryDescriptor = {
      initial: tmpMemorySize,
      maximum: tmpMemorySize,
      shared: true,
    } as any;
    const wasmMemory = new WebAssembly.Memory(opts);

    if (!(wasmMemory.buffer instanceof SharedArrayBuffer))
      return false;
  }

  return initialTest;
};
