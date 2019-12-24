
require("fpsmeter");

const Experiment = require('./experiment/Experiment.js');

//

window.addEventListener("load", onGlobalLoad, false);
window.addEventListener("error", onGlobalError, false);

function onGlobalError(err) {

    alert(`error=${err.message}`);
}

function onGlobalLoad() {

    //
    // init

	const canvas = addCanvas(0, 0, 800, 600);
    const currentExperiment = new Experiment(canvas);

    //

    const fpsmeters = {
        main: addFpsMeter(0, 0),
        update: addFpsMeter(0, 50, true),
        render: addFpsMeter(0, 100, true)
    };

    //
    // main loop

    let isRunning = true;

    function handleGlobalError(err) {

        // this will stop the main loop when an error occur
        // -> depending of what's wrong, it eat up the cpu and freeze some browser(s)
        // -> it also help to debug as it won't run and fail again to log many time the same error
        isRunning = false;
    };

    window.addEventListener("error", handleGlobalError, false);

    function tick() {

        if (isRunning)
            requestAnimationFrame(tick);

            fpsmeters.main.tick();
            fpsmeters.main.tickStart();

        //

        fpsmeters.update.tickStart();

        currentExperiment.update();

        fpsmeters.update.tick();

        //

        fpsmeters.render.tickStart();

        currentExperiment.render();

        fpsmeters.render.tick();
    };

    tick();
}

function addFpsMeter(x, y, toggleIt) {

    const domElement = document.createElement("div");
    domElement.style.position = "absolute";
    domElement.style.left = `${x || 0}px`;
    domElement.style.top = `${y || 0}px`;

    document.body.appendChild(domElement);

    const theme = FPSMeter.theme.transparent;
    const newFpsMeter = new FPSMeter( domElement, theme );

    // switch from FPS to ms
    if (toggleIt)
        newFpsMeter.toggle();

    return newFpsMeter;
}

function addCanvas(x, y, width, height) {

    const canvas = document.createElement("canvas");
    canvas.style.position = "absolute";
    canvas.style.left = `${x || 0}px`;
    canvas.style.top = `${y || 0}px`;
    canvas.width = `${width || 800}`;
    canvas.height = `${height || 600}`;

    document.body.appendChild(canvas);

    return canvas; 
}
