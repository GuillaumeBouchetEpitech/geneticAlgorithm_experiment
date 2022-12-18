
export const scriptLoadingUtility = (src: string): Promise<void> => {
  return new Promise((resolve, reject) => {
    const scriptElement = document.createElement("script") as HTMLScriptElement;
    scriptElement.src = src;
    // scriptElement.onprogress = (event) => logger.log("event", event);
    scriptElement.addEventListener('load', () => resolve);
    scriptElement.addEventListener('error', reject);
    document.head.appendChild(scriptElement);
  });
};
