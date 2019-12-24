
const Renderer = require('./graphic/Renderer.js');
const Simulation = require('./simulation/Simulation.js');

const MathUtils = require('./utilities/MathUtils.js');

//

class Experiment {

    constructor(canvas, circuitData) {

        this._renderer = new Renderer(canvas);
        this._simulation = new Simulation(circuitData);

        this._leadingCar = { index: -1, timeout: 0 };
        this._cameraOrigin = { x: 0, y: 0 };
    }

    update() {

        const fixedDelta = 0.025;
        const simulationDelta = fixedDelta * 10;

        for (let ii = 0; ii < 3; ++ii)
            this._simulation.update(simulationDelta);

        this._updateCamera(fixedDelta);
    }

    render() {

        this._renderer.clear();


        //
        // render circuit

        this._renderer.drawLines(this._simulation.circuit.checkpoints, "#ffffff", 3);
        this._renderer.drawLines(this._simulation.circuit.walls, "#0000ff", 2);

        this._renderer.drawPoint(
            this._simulation.circuit.startPosition.x,
            this._simulation.circuit.startPosition.y,
            "#ffffff",
            2
        );

        this._renderer.drawLine(
            this._simulation.circuit.startPosition.x,
            this._simulation.circuit.startPosition.y,
            this._simulation.circuit.startPosition.x + Math.cos(this._simulation.circuit.startAngle) * 50.0,
            this._simulation.circuit.startPosition.y + Math.sin(this._simulation.circuit.startAngle) * 50.0,
            "#ffffff",
            2
        );

        // render circuit
        //

        //
        // render cars

        const cars = this._simulation.cars;
        for (let ii = 0; ii < cars.length; ++ii)
            this._renderCar(cars[ii], "#00ff00", (ii == this._leadingCar.index));

        //
        // render trails

        const trail_colors = [ "#ffffff", "#00ff00", "#00cc00", "#009900", "#006600", "#003300" ];
        let trail_colors_index = 0;

        const trails = this._simulation.trails;
        for (let ii = (trails.length - 1); ii >= 0; --ii) { // from last to first

            const trail = trails[ii];
            const color = trail_colors[trail_colors_index++];

            this._renderer.drawLineStrip(trail, color);
        }

        //
        // render HUD

        this._renderHUD();
    }

    _updateCamera(delta) {

        this._leadingCar.timeout -= delta;

        if (this._leadingCar.index == -1 || this._leadingCar.timeout <= 0) {

            this._leadingCar.index = -1;
            this._leadingCar.timeout = 0.5;

            let currentBest;
            for (let ii = 0; ii < this._simulation.cars.length; ++ii) {

                const currentCar = this._simulation.cars[ii];

                if (!currentCar.alive)
                    continue;

                if (currentBest && currentBest.fitness > currentCar.fitness)
                    continue;

                currentBest = currentCar;
                this._leadingCar.index = ii;
            }
        }

        let target;
        if (this._leadingCar.index != -1)
            target = this._simulation.cars[this._leadingCar.index].position;
        else
            target = { x: 0, y: 0 };

        this._cameraOrigin = MathUtils.lerp(this._cameraOrigin, target, 0.1);

        this._renderer.setCenter(-this._cameraOrigin.x, -this._cameraOrigin.y);
    }

    _renderCar(car, alive_color, show_sensors) {

        const position = car.position;
        const angle = car.angle;

        const size_h = 25.0;
        const size_v = 12.5;

        const positions = [
            { x: position.x - size_h, y: position.y - size_v },
            { x: position.x + size_h, y: position.y - size_v },
            { x: position.x + size_h, y: position.y + size_v },
            { x: position.x - size_h, y: position.y + size_v }
        ];

        for (let ii = 0; ii < 4; ++ii)
            positions[ii] = MathUtils.rotateVec2(positions[ii], position, angle);

        const color = (car.alive ? alive_color :  "#ff0000");

        this._renderer.drawLine(positions[0].x, positions[0].y, positions[1].x, positions[1].y, color, 6);
        this._renderer.drawLine(positions[1].x, positions[1].y, positions[2].x, positions[2].y, color, 6);
        this._renderer.drawLine(positions[2].x, positions[2].y, positions[3].x, positions[3].y, color, 6);
        this._renderer.drawLine(positions[3].x, positions[3].y, positions[0].x, positions[0].y, color, 6);

        if (!car.alive || !show_sensors) // render sensors only if the car is alive
            return;

        const sensors = car.sensors;

        for (let ii = 0; ii < sensors.length; ++ii) {

            const sensor = sensors[ii];

            const impact = {
                x: sensor.line.p1.x + (sensor.line.p2.x - sensor.line.p1.x) * sensor.result,
                y: sensor.line.p1.y + (sensor.line.p2.y - sensor.line.p1.y) * sensor.result
            };

            this._renderer.drawLine(sensor.line.p1.x, sensor.line.p1.y, impact.x, impact.y, "#8888ff", 2);
            this._renderer.drawLine(impact.x, impact.y, sensor.line.p2.x, sensor.line.p2.y, "#ff8888", 2);

            this._renderer.drawPoint(impact.x, impact.y, "#ffff00", 2);
        }
    }

    _renderHUD() {

        let tmp_weights = null;

        const genomes = this._simulation.geneticAlgorithm._genomes;

        if (genomes.length > 0) {

            let index = 0;
            for (; index < genomes.length; ++index)
                if (genomes[index].car.alive)
                    break;

            if (index < genomes.length)
                tmp_weights = genomes[index].weights;
        }

        if (!tmp_weights)
            return;

        this._renderer.setOrigin(50, 200);

        let windex = 0;

        const topology = this._simulation._ann_topology

        for (let kk = 1; kk < topology.length; ++kk) {

            const prevLayer = topology[kk-1];
            const currLayer = topology[kk];

            const prevY = (1.0 - (prevLayer / 5)) * 185;
            const currY = (1.0 - (currLayer / 5)) * 185;

            for (let ii = 0; ii < prevLayer; ++ii)
                for (let jj = 0; jj < currLayer; ++jj) {

                    const currX = (kk-1) * 75;

                    const weightValue = tmp_weights[windex++];

                    let ratio = weightValue * 10;
                    if (ratio < 0)
                        ratio = -ratio;
                    if (ratio < 1)
                        ratio = 1;

                    if (weightValue > 0)
                        this._renderer.drawLine(currX, prevY+ii*75, currX+75, currY+jj*75, "#ff0000", ratio);
                    else
                        this._renderer.drawLine(currX, prevY+ii*75, currX+75, currY+jj*75, "#0000ff", ratio);
                }
        }

        for (let kk = 0; kk < topology.length; ++kk) {

            const currLayer = topology[kk];

            const currY = (1.0 - (currLayer / 5)) * 185;

            for (let ii = 0; ii < currLayer; ++ii) {

                const currX = (kk) * 75;

                this._renderer.drawCircle(currX, currY+ii*75, 13, "#ffff00");
                this._renderer.drawCircle(currX, currY+ii*75, 10, "#00ff00");
            }
        }
    }
}

//

module.exports = Experiment;
