
const Circuit = require("./logic/Circuit.js");
const Car = require("./logic/Car.js");
const GeneticAlgorithm = require("./machineLearning/GeneticAlgorithm.js");

const circuitData = require("./circuits/default.js");

//

class Simulation {

	constructor() {

		//
		// circuit

		const genomeSize = 200;
		this._ann_topology = [5, 3, 2];

		this._geneticAlgorithm = new GeneticAlgorithm( genomeSize, this._ann_topology );
		this._circuit = new Circuit(circuitData);

		//
		// cars

		const position = this._circuit.startPosition;
		const angle = this._circuit.startAngle;
		const checkpoints = this._circuit.checkpoints;

		this._cars = [];
		for (let ii = 0; ii < genomeSize; ++ii) {

			const car = new Car(position, angle, checkpoints);

			this._geneticAlgorithm.genomes[ii].car = car;

			this._cars.push(car);
		}

		this._trails = [];
	}

	update(delta) {

		let readyToBreed = true;

		for (let ii = 0; ii < this._cars.length; ++ii) {

			if (!this._cars[ii].alive)
				continue;

			this._cars[ii].update(delta, this._circuit.walls, this._geneticAlgorithm.ANNs[ii]);

			readyToBreed = false;
		}

		// end of the current generation?

		if (!readyToBreed)
			return; // no

		// rate the genome

		for (let ii = 0; ii < this._cars.length; ++ii)
			this._geneticAlgorithm.genomes[ii].fitness = this._cars[ii].fitness;

		const progressWasMade = this._geneticAlgorithm.breedPopulation();

		// save the best trail

		if (progressWasMade) {

			this._trails.push( this._geneticAlgorithm._bestGenome.car.trail );

			if (this._trails.length > 5)
				this._trails.shift();
		}

		// reset the cars

		for (let ii = 0; ii < this._cars.length; ++ii) {

			const car = this._cars[ii];

			car.reset(this._circuit.startPosition, this._circuit.startAngle);

			this._geneticAlgorithm.genomes[ii].car = car;
		}
	}

	get geneticAlgorithm() { return this._geneticAlgorithm; }
	get circuit() { return this._circuit; }
	get cars() { return this._cars; }
	get trails() { return this._trails; }

}

//

module.exports = Simulation;
