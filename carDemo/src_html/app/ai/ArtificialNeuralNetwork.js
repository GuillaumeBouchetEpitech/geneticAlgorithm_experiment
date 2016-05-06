

define([],function()
{

	//

	var createArtificialNeuralNetwork = function (topology)
	{
		var prev_layer_num_neuron = topology[0]; // inputs

		// console.log("topology", topology);

		this._layerHidden = [];
		for (var i = 1; i < topology.length - 1; ++i) // exclude frist&last -> input&output
		{
			var curr_layer_num_neuron = topology[i]; // number of neuron on the hidden layer

			var layer = [];
			for (var j = 0; j < curr_layer_num_neuron; ++j)
			{
				var neuron = [];

				for (var k = 0; k < prev_layer_num_neuron; ++k)
					neuron.push( Math.random() - Math.random() ); // [-1 < x < +1]

				layer.push(neuron);
			}

			this._layerHidden.push(layer);

			prev_layer_num_neuron = curr_layer_num_neuron; // save the size -> this layer feed the next one
		}

		// console.log("this._layerHidden", this._layerHidden);

		this._layerOutput = [];
		for (var i = 0; i < topology[topology.length - 1]; ++i)
		{
			var neuron = [];

			for (var j = 0; j < prev_layer_num_neuron; ++j)
				neuron.push( Math.random() - Math.random() ); // [-1 < x < +1]

			this._layerOutput.push(neuron);
		}

		// console.log("this._layerHidden", this._layerHidden);
		// console.log("this._layerOutput", this._layerOutput);

	};

	//

	createArtificialNeuralNetwork.prototype.process = function(arr_input)
	{
		if (arr_input.length != 5)
			throw new Error( "received invalid number of inputs" );

		var output = [];

		//
		// process hidden layer

		var	hidden_input = arr_input;

		// Cycle over all the neurons and sum their weights against the inputs.
		for (var i = 0; i < this._layerHidden.length; ++i)
		{
			var current_layer = this._layerHidden[i];

			var hidden_output = [];

			for (var j = 0; j < current_layer.length; ++j)
			{
				var activation = 0.0;

				// Sum the weights to the activation value.
				for (var k = 0; k < hidden_input.length; ++k)
					activation += hidden_input[k] * current_layer[j][k];

				hidden_output.push( activation );
			}

			hidden_input = hidden_output;
		}

		// process hidden layer
		//

		//
		// process output layer

		// Cycle over all the neurons and sum their weights against the inputs.
		for (var i = 0; i < this._layerOutput.length; ++i)
		{
			var activation = 0.0;

			// Sum the weights to the activation value.
			for (var j = 0; j < hidden_input.length; ++j)
				activation += hidden_input[j] * this._layerOutput[i][j];

			output.push( activation );
		}

		// process output layer
		//

		return output;
	};

	//

	createArtificialNeuralNetwork.prototype.setWeights = function(arr_weights)
	{
		// if (arr_weights..length != m_topology.getTotalWeights())
		// 	throw std::invalid_argument( "received invalid number of weights" );

		var weights_inc = 0;

		for (var i = 0; i < this._layerHidden.length; ++i)
			for (var j = 0; j < this._layerHidden[i].length; ++j)
				for (var k = 0; k < this._layerHidden[i][j].length; ++k)
					this._layerHidden[i][j][k] = arr_weights[ weights_inc++ ];

		for (var i = 0; i < this._layerOutput.length; ++i)
			for (var j = 0; j < this._layerOutput[i].length; ++j)
				this._layerOutput[i][j] = arr_weights[ weights_inc++ ];
	}

	//

	createArtificialNeuralNetwork.prototype.getWeights = function()
	{
		var out_weights = [];

		for (var i = 0; i < this._layerHidden.length; ++i)
			for (var j = 0; j < this._layerHidden[i].length; ++j)
				for (var k = 0; k < this._layerHidden[i][j].length; ++k)
					out_weights.push( this._layerHidden[i][j][k] );

		for (var i = 0; i < this._layerOutput.length; ++i)
			for (var j = 0; j < this._layerOutput[i].length; ++j)
				out_weights.push( this._layerOutput[i][j] );

		return out_weights;
	}

	//

	return createArtificialNeuralNetwork;
})
