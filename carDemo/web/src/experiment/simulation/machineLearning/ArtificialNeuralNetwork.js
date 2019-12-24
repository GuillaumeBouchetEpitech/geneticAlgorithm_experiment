
function steeperSigmoid(x)
{
	return 1.0 / (1.0 + Math.exp(-4.9 * x));
}


class ArtificialNeuralNetwork {

	constructor(topology) {

		const lastLayerIndex = topology.length - 1;

		//
		// extra check here

		if (topology.length < 2)
			throw new Error("received invalid number of layer");

		if (topology[0] <= 0) // input
			throw new Error("received invalid number of inputs");

		for (let ii = 1; ii < lastLayerIndex; ++ii) // exclude first & last -> input & output
			if (topology[ii] <= 0)
				throw new Error("received invalid number of hidden neurons");

		if (topology[lastLayerIndex] <= 0) // input
			throw new Error("received invalid number of outputs");

		// save the topology for later checks

		this._topology = topology;

		// compute totalWeights

		this._totalWeights = 0;

		let previousLayerNeuronNumber = topology[0]; // inputs

		for (let ii = 1; ii < lastLayerIndex; ++ii) { // exclude first & last -> input & output

			const currentLayerNeuronNumber = topology[ii];

			this._totalWeights += previousLayerNeuronNumber * currentLayerNeuronNumber;
			previousLayerNeuronNumber = currentLayerNeuronNumber;
		}
		this._totalWeights += previousLayerNeuronNumber * topology[lastLayerIndex];

		// build hidden layer(s)

		previousLayerNeuronNumber = topology[0]; // inputs

		this._hiddenLayers = [];
		for (let ii = 1; ii < lastLayerIndex; ++ii) { // exclude first & last -> input & output

			const currentLayerNeuronNumber = topology[ii]; // number of neuron on the hidden layer

			const layer = [];

			this._buildLayer(layer, previousLayerNeuronNumber, currentLayerNeuronNumber);

			this._hiddenLayers.push(layer);

			previousLayerNeuronNumber = currentLayerNeuronNumber; // save the size -> this layer feed the next one
		}

		// build output layer

		this._outputLayer = [];

		this._buildLayer(this._outputLayer, previousLayerNeuronNumber, topology[lastLayerIndex]);
	}

	_buildLayer(layer, inputsNumber, neuronsNumber) {

		for (let ii = 0; ii < neuronsNumber; ++ii) {

			const connections = [];

			for (let jj = 0; jj < inputsNumber; ++jj) {

				const randomValue = Math.random() - Math.random(); // [-1 < value < +1]

				connections.push(randomValue);
			}

			layer.push(connections);
		}
	}

	process(inputs) {

		if (inputs.length != this._topology[0])
			throw new Error(`invalid inputs, input=${inputs.length}, expected=${this._topology[0]}`);

		const output = [];

		//
		// process hidden layer

		let	currentInputs = inputs;

		for (let kk = 0; kk < this._hiddenLayers.length; ++kk) {

			const currentOutput = [];

			this._processLayer(this._hiddenLayers[kk], currentInputs, currentOutput);

			currentInputs = currentOutput;
		}

		//
		// process output layer

		this._processLayer(this._outputLayer, currentInputs, output);

		return output;
	}

	_processLayer(layer, inputs, output) {

		// Cycle over all the connections and sum their weights against the inputs.
		for (let ii = 0; ii < layer.length; ++ii) {

			const connections = layer[ii];

			let activation = 0.0;

			// Sum the weights to the activation value.
			for (let jj = 0; jj < inputs.length; ++jj)
				activation += inputs[jj] * connections[jj];

			// if (Number.isNaN(activation))
			// 	activation = 0;

			// output.push(activation);
			output.push(steeperSigmoid(activation));
		}
	}

	setWeights(arr_weights) {

		if (arr_weights.length != this._totalWeights)
			throw new Error(`invalid weights, input=${arr_weights.length}, expected=${this._totalWeights}`);

		let weights_inc = 0;

		for (let kk = 0; kk < this._hiddenLayers.length; ++kk) {

			const layer = this._hiddenLayers[kk];

			for (let ii = 0; ii < layer.length; ++ii) {

				const connections = layer[ii];

				for (let jj = 0; jj < connections.length; ++jj)
					connections[jj] = arr_weights[weights_inc++];
			}
		}

		for (let ii = 0; ii < this._outputLayer.length; ++ii) {

			const connections = this._outputLayer[ii];

			for (let jj = 0; jj < connections.length; ++jj)
				connections[jj] = arr_weights[weights_inc++];
		}
	}

	set weights(arr_weights) {
		this.setWeights(arr_weights);
	}

	getWeights() {

		const out_weights = [];

		for (let ii = 0; ii < this._hiddenLayers.length; ++ii) {

			const layer = this._hiddenLayers[ii];

			for (let jj = 0; jj < layer.length; ++jj) {

				const connections = layer[jj];

				for (let kk = 0; kk < connections.length; ++kk)
					out_weights.push(connections[kk]);
			}
		}

		for (let jj = 0; jj < this._outputLayer.length; ++jj) {

			const connections = this._outputLayer[jj];

			for (let kk = 0; kk < connections.length; ++kk)
				out_weights.push(connections[kk]);
		}

		return out_weights;
	}

	get weights() {
		return this.getWeights();
	}

	get totalWeights() { return this._totalWeights; }
}

// export default ArtificialNeuralNetwork;
module.exports = ArtificialNeuralNetwork;
