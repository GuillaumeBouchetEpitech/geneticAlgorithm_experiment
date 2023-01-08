
import Logger from "./Logger";
import { Demo } from "./Demo";
import {
  extractVarsFromUrl,
} from "./helpers";

const onGlobalPageLoad = async () => {

  let isRunning = true;

  const logger = new Logger("#loggerOutput");

  logger.log("[JS] page loaded");

  const onInitialGlobalPageError = (event: ErrorEvent) => {
    isRunning = false;
    logger.error(`[JS] exception, event=${event}`);
  };
  window.addEventListener("error", onInitialGlobalPageError);

  const errorText = document.querySelector<HTMLParagraphElement>("#errorText")!;
  const renderArea = document.querySelector<HTMLDivElement>("#renderArea")!;
  const mainCanvas = document.querySelector<HTMLCanvasElement>("#canvas")!;
  const buttons = {
    switchTo90cars: document.querySelector<HTMLButtonElement>("#try_with_90_cars")!,
    switchTo270cars: document.querySelector<HTMLButtonElement>("#try_with_270_cars")!,
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

  const onNextGlobalPageError = (event: ErrorEvent) => {
    onInitialGlobalPageError(event);
    showErrorText(`fatal error, event=${event}`);
  };
  window.removeEventListener("error", onInitialGlobalPageError);
  window.addEventListener("error", onNextGlobalPageError);

  //
  //
  // configuration's logic

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

  //
  //
  // buttons' logic

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
    buttons.switchTo270cars.disabled = true;
    setupActiveButton(buttons.switchTo90cars, 'blueButton', 30);
  }
  else {
    buttons.switchTo90cars.disabled = true;
    setupActiveButton(buttons.switchTo270cars, 'redButton', 90);
  }

  //
  //
  //


  const onProgress = (percent: number) => {
    const statusMsg = `Loading wasm [${percent}%]`;

    // remove the last logged entry if it was a progress updated
    if (logger.size > 0 && logger.peekLast()!.indexOf("Loading wasm [") >= 0)
      logger.popLast();

    logger.log(`[JS] ${statusMsg}`);
    showErrorText(statusMsg);
  };

  const myDemo = new Demo(mainCanvas, onProgress, showErrorText);

  try {

    await myDemo.initialise(
      config.width,
      config.height,
      config.totalCores,
      config.genomesPerCore,
      logger,
    );

    showCanvas();
  }
  catch (err) {
    logger.error(`[JS] dependencies check failed: message="${err.message}"`);
    showErrorText(`
      this browser isn't compatible<br>
      error message:<br>
      => ${err.message}
    `);
  }

  const deltaTime = Math.floor(1000 / 30); // 30fps
  const onFrame = () => {
    if (isRunning)
      setTimeout(onFrame, deltaTime);

    myDemo.update(deltaTime);
  };
  onFrame();
};

window.addEventListener("load", onGlobalPageLoad);
