
// const utils = require('../utils/index.js');
// const Logger = utils.Logger;

const ArtificialNeuralNetwork = require("./ArtificialNeuralNetwork.js");

class GeneticAlgorithm {

	constructor(genomeSize, ann_topology) {

		// this._logger = new Logger("logger");

		this._currentId = 0;

		this._ANNs = [];
		for (let ii = 0; ii < genomeSize; ++ii)
			this._ANNs.push(new ArtificialNeuralNetwork(ann_topology));

		this._genomes = [];
		for (let ii = 0; ii < genomeSize; ++ii)
			this._genomes.push({
				id: this._currentId++,
				fitness: 0.0,
				weights: this._ANNs[ii].weights
			});

		this._currentGeneration = 1;
		this._bestGenome = null;
	}

	breedPopulation() {

		const bestGenomes = this._genomes.slice(0).sort((genA, genB) => {
			return genB.fitness - genA.fitness;
		});

		const isSmarter = (!this._bestGenome || this._bestGenome.fitness < bestGenomes[0].fitness);
		if (isSmarter)
			this._bestGenome = bestGenomes[0];

		const children = [];

		{ // elitism

			const eliteGenome = {
				id: this._bestGenome.id,
				fitness: 0.0,
				weights: []
			};

			// deep copy
			for (let ii = 0; ii < this._bestGenome.weights.length; ++ii)
				eliteGenome.weights.push(this._bestGenome.weights[ii]);

			this._mutate(eliteGenome);

			children.push(eliteGenome);

		} // elitism

		{ // breed best genomes

			const parentsPairs = [];
			for (let ii = 0; ii < bestGenomes.length; ++ii)
				for (let jj = ii + 1; jj < bestGenomes.length; ++jj)
					parentsPairs.push([ ii, jj ]);

			// sort pair by summed fitness
			parentsPairs.sort((pairA, pairB) => {

				// the higher the better
				return (
					bestGenomes[pairB[0]].fitness + bestGenomes[pairB[1]].fitness -
					bestGenomes[pairA[0]].fitness + bestGenomes[pairA[1]].fitness
				);
			});

			const maxOffsprings = this._genomes.length * 0.9 - children.length;

			for (let ii = 0; ii < maxOffsprings; ++ii) {

				const parentA = bestGenomes[parentsPairs[ii][0]];
				const parentB = bestGenomes[parentsPairs[ii][1]];
				const offspring = this._reproduce(parentA, parentB);
				this._mutate(offspring);
				children.push(offspring);
			}

		} // breed best genomes

		{
			const remainingChildren = (this._genomes.length - children.length);

			const totalWeights = this._ANNs[0].totalWeights;

			for (let ii = 0; ii < remainingChildren; ++ii) {

				const genome = {
					id: this._currentId++,
					fitness: 0.0,
					weights: []
				};

				for (let jj = 0; jj < totalWeights; ++jj)
					genome.weights.push(Math.random() - Math.random());  // [-1..1]

				children.push(genome);
			}
		}

		this._genomes = children;
		++this._currentGeneration;

		for (let ii = 0; ii < this._genomes.length; ++ii)
			this._ANNs[ii].weights = this._genomes[ii].weights;

		return isSmarter;
	}

	_reproduce(parentA, parentB) {

		const totalWeights = this._ANNs[0].totalWeights;

		const offspring = {
			id: this._currentId++,
			fitness: 0.0,
			weights: []
		};

		for (let ii = 0; ii < totalWeights; ++ii) {

			if (parentA.fitness < parentB.fitness) {
				// 70/30 chances for the fittest parent
				if (Math.random() > 0.30)
					offspring.weights[ii] = parentB.weights[ii];
				else
					offspring.weights[ii] = parentA.weights[ii];
			}
			else if (parentB.fitness < parentA.fitness) {
				// 70/30 chances for the fittest parent
				if (Math.random() > 0.30)
					offspring.weights[ii] = parentA.weights[ii];
				else
					offspring.weights[ii] = parentB.weights[ii];
			}
			else {
				// 50/50 chances for both parents
				if (Math.random() < 0.5)
					offspring.weights[ii] = parentA.weights[ii];
				else
					offspring.weights[ii] = parentB.weights[ii];
			}
		}

		return offspring;
	}

	_mutate(genome) {

		const mutationChance = 0.1;
		const mutationEffect = 0.2;

		for (let ii = 0; ii < genome.weights.length; ++ii)
			if (Math.random() < mutationChance) {
				const randomValue = (Math.random() - Math.random()); // [-1..1]
				genome.weights[ii] += (randomValue * mutationEffect);
			}
	}

	get genomes() { return this._genomes; }
	get ANNs() { return this._ANNs; }

}

//

// export default GeneticAlgorithm;
module.exports = GeneticAlgorithm;
