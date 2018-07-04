
import "./externals/fpsmeter.js";

import Experiment from "./experiment/index.js";

window.onload = () => {

    //
    // init

	const canvas = document.getElementById("main-canvas");

    const circuitDataText = document.getElementById("circuit-data").innerHTML;
    const circuitData = JSON.parse(circuitDataText);

    const myExperiment = new Experiment(canvas, circuitData);

    //

    const myFpsmeterUpdateElement = document.getElementById('fpsmeter_update');
    const myFpsmeterUpdate = new window.FPSMeter(
        myFpsmeterUpdateElement,
        window.FPSMeter.theme.transparent
    );
    myFpsmeterUpdate.toggle(); // <= switch from FPS to ms

    const myFpsmeterRenderElement = document.getElementById('fpsmeter_render');
    const myFpsmeterRender = new window.FPSMeter(
        myFpsmeterRenderElement,
        window.FPSMeter.theme.transparent
    );
    myFpsmeterRender.toggle(); // <= switch from FPS to ms

    //
    // main loop

    let isRunning = true;

    window.onerror = () => {

        // this will stop the main loop when an error occur
        // -> depending of what's wrong, it eat up the cpu and freeze some browser(s)
        // -> it also help to debug as it won't run and fail again to log many time the same error
        isRunning = false;
    };

    const tick = (in_event) => {

        if (isRunning)
            requestAnimationFrame( tick );

        //

        myFpsmeterUpdate.tickStart();

        myExperiment.update();

        myFpsmeterUpdate.tick();

        //

        myFpsmeterRender.tickStart();

        myExperiment.render();

        myFpsmeterRender.tick();
    };

    tick();
};
