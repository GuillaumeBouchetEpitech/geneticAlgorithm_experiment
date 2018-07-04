
import { Logger } from "../utils/index.js";

import ArtificialNeuralNetwork from "./artificialNeuralNetwork.js";

class GeneticAlgorithm {

	constructor(genomeSize, ann_topology) {

		this._logger = new Logger("logger");

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
		this._bestFitness = 0.0;
		this._bestGenome = null;
	}

	breedPopulation() {

		let progressWasMade = false;

		const bestGenomes = this._getBestGenomes(6);

		const children = [];

		if (this._bestFitness < bestGenomes[0].fitness)
			this._bestFitness = bestGenomes[0].fitness;

		if (this._bestGenome === null || this._bestGenome.fitness < this._bestFitness) {

			this._bestGenome = bestGenomes[0];
			progressWasMade = true;
		}

		{ // elitism

			const bestDude = {
				id: this._bestGenome.id,
				fitness: 0.0,
				weights: []
			};

			const weights = this._bestGenome.weights;
			for (let ii = 0; ii < weights.length; ++ii)
				bestDude.weights.push(weights[ii]);

			this._mutate(bestDude);

			children.push(bestDude);

		} // elitism

		// breed best genomes

		for (let ii = 0; ii < bestGenomes.length; ++ii)
			for (let jj = ii + 1; jj < bestGenomes.length; ++jj) {

				const	baby1 = {},
						baby2 = {};

				this._crossBreed(bestGenomes[ii], bestGenomes[jj], baby1, baby2);
				this._mutate(baby1);
				this._mutate(baby2);

				if (children.length < this._genomes.length)
					children.push(baby1);

				if (children.length < this._genomes.length)
					children.push(baby2);
			}

		// For the remainding n population, add some random kiddies.
		const remainingChildren = (this._genomes.length - children.length);

		const total_weights = this._ANNs[0].totalWeights;

		for (let ii = 0; ii < remainingChildren; ++ii) {

			const genome = {
				id: this._currentId++,
				fitness: 0.0,
				weights: []
			};

			for (let jj = 0; jj < total_weights; ++jj)
				genome.weights.push(Math.random() - Math.random());  // [-1 < x < +1]

			children.push(genome);
		}

		this._genomes = children;
		++this._currentGeneration;

		for (let ii = 0; ii < this._genomes.length; ++ii)
			this._ANNs[ii].weights = this._genomes[ii].weights;

		return progressWasMade;
	}

	_getBestGenomes(totalAsked) {

		// realistic total outputed genomes
		totalAsked = Math.min(this._genomes.length, totalAsked);

		const sortedGenomesIndex = [];
		for (let ii = 0; ii < this._genomes.length; ++ii)
			sortedGenomesIndex.push({ index: ii, fitness: this._genomes[ii].fitness });

		sortedGenomesIndex.sort((itemA, itemB) =>  itemB.fitness - itemA.fitness);

		const out = [];

		for (let ii = 0; ii < totalAsked; ++ii) {

			const index = sortedGenomesIndex[ii].index;

			out.push(this._genomes[index]);
		}

		return out;
	}

	_crossBreed(g1, g2, baby1, baby2) {

		const totalWeights = g1.weights.length;
		const crossover = Math.floor(Math.random() * totalWeights);

		baby1.id = this._currentId++;
		baby1.weights = [];

		baby2.id = this._currentId++;
		baby2.weights = [];

		for (let ii = 0; ii < crossover; ++ii) {
			baby1.weights[ii] = g1.weights[ii];
			baby2.weights[ii] = g2.weights[ii];
		}

		for (let ii = crossover; ii < totalWeights; ii++) {
			baby1.weights[ii] = g2.weights[ii];
			baby2.weights[ii] = g1.weights[ii];
		}
	}

	_mutate(genome) {

		for (let ii = 0; ii < genome.weights.length; ++ii)
			if (Math.random() < 0.1) {
				const randomValue = Math.random() - Math.random(); // [-1 < x < +1]
				genome.weights[ii] += (randomValue * 0.2);
			}
	}

	get genomes() { return this._genomes; }
	get ANNs() { return this._ANNs; }

}

//

export default GeneticAlgorithm;
