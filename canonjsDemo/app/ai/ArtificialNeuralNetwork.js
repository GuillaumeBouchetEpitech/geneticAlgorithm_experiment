
'use strict'

define([],function()
{

    //

    var createArtificialNeuralNetwork = function (topology)
    {
        //
        // extra check here

        if (topology.length < 2)
            throw new Error("received invalid number of layer");

        if (topology[0] <= 0) // input
            throw new Error("received invalid number of inputs");

        for (var i = 1; i < topology.length - 1; ++i) // exclude frist&last -> input&output
            if (topology[i] <= 0)
                throw new Error("received invalid number of hidden neurons");

        if (topology[topology.length-1] <= 0) // input
            throw new Error("received invalid number of outputs");

        // extra check here
        //


        //
        // compute totalWeights

        // save the topology for later extra check
        this._topology = topology;

        this._totalWeights = 0;

        var prev_layer_num_neuron = topology[0];
        for (var i = 1; i < topology.length - 1; ++i) // exclude frist&last -> input&output
        {
            var num_neuron = topology[i];

            this._totalWeights += prev_layer_num_neuron * num_neuron;
            prev_layer_num_neuron = num_neuron;
        }
        this._totalWeights += prev_layer_num_neuron * topology[topology.length-1];

        // compute totalWeights
        //

        //
        // build hidden layer(s)

        var prev_layer_num_neuron = topology[0]; // inputs

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

        // build hidden layer(s)
        //


        //
        // build output layer

        this._layerOutput = [];
        for (var i = 0; i < topology[topology.length - 1]; ++i)
        {
            var neuron = [];

            for (var j = 0; j < prev_layer_num_neuron; ++j)
                neuron.push( Math.random() - Math.random() ); // [-1 < x < +1]

            this._layerOutput.push(neuron);
        }

        // build output layer
        //
    };

    //

    createArtificialNeuralNetwork.prototype.process = function(arr_input)
    {
        if (arr_input.length != this._topology[0])
            throw new Error( "received invalid number of inputs" );

        var output = [];

        //
        // process hidden layer

        var hidden_input = arr_input;

        // Cycle over all the neurons and sum their weights against the inputs.
        for (var i in this._layerHidden)
        {
            var current_layer = this._layerHidden[i];

            var hidden_output = [];

            for (var j in current_layer)
            {
                var activation = 0.0;

                // Sum the weights to the activation value.
                for (var k in hidden_input)
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
        for (var i in this._layerOutput)
        {
            var activation = 0.0;

            // Sum the weights to the activation value.
            for (var j in hidden_input)
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
        if (arr_weights.length != this._totalWeights)
            throw new Error( "received invalid number of weights" );

        var weights_inc = 0;

        for (var i in this._layerHidden)
            for (var j in this._layerHidden[i])
                for (var k in this._layerHidden[i][j])
                    this._layerHidden[i][j][k] = arr_weights[ weights_inc++ ];

        for (var i in this._layerOutput)
            for (var j in this._layerOutput[i])
                this._layerOutput[i][j] = arr_weights[ weights_inc++ ];
    }

    //

    createArtificialNeuralNetwork.prototype.getWeights = function()
    {
        var out_weights = [];

        for (var i in this._layerHidden)
            for (var j in this._layerHidden[i])
                for (var k in this._layerHidden[i][j])
                    out_weights.push( this._layerHidden[i][j][k] );

        for (var i in this._layerOutput)
            for (var j in this._layerOutput[i])
                out_weights.push( this._layerOutput[i][j] );

        return out_weights;
    }

    //

    return createArtificialNeuralNetwork;
})
