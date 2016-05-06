

define(
    [
          './circuit.js'
        , './car.js'
        , '../ai/geneticAlgo.js'
    ],
    function(
		  createCircuit
		, createCar
		, createGeneticAlgo
    )
{

	//

	var createSimulation = function (elem_id)
	{
		// circuit
		this._circuit = new createCircuit(elem_id);

		var genome_size = 30;

		this._geneticAlgo = new createGeneticAlgo( genome_size );


		// cars
		this._cars = [];
		for (var i = 0; i < genome_size; ++i)
		{
			var car = new createCar();

			this._geneticAlgo._genomes[i].car = car;

			car._position = {
				  x: this._circuit._start_position.x
				, y: this._circuit._start_position.y
			};

			car._angle = this._circuit._start_angle;

			car._checkpoints = [];
			for (var j = 0; j < this._circuit._checkpoints.length; ++j)
				car._checkpoints.push( this._circuit._checkpoints[j] );

			this._cars.push(car);
		}

		this._trails = [];
	};

	//

	createSimulation.prototype.update = function(step)
	{
		var someone_is_alive = false;

		for (var i = 0; i < this._cars.length; ++i)
		{
			if (!this._cars[i]._alive)
				continue;

			this._cars[i].update(step, this._circuit._walls, this._geneticAlgo._ANNs[i]);

			someone_is_alive = true;
		}

		if (someone_is_alive)
			return;

		for (var i = 0; i < this._cars.length; ++i)
			this._geneticAlgo._genomes[i].fitness = this._cars[i]._fitness;

		this._geneticAlgo.BreedPopulation();

		// save the best trail

		if (this._geneticAlgo._is_a_great_generation)
		{
			this._geneticAlgo._is_a_great_generation = false;

			this._trails.push( this._geneticAlgo._alpha_genome.car._trail );
			if (this._trails.length > 5)
				this._trails.splice(0, 1);
		}

		// reset the cars

		for (var i = 0; i < this._cars.length; ++i)
		{
			var car = this._cars[i];

			this._geneticAlgo._genomes[i].car = car;

			car._position = {
				  x: this._circuit._start_position.x
				, y: this._circuit._start_position.y
			};

			car._angle = this._circuit._start_angle;

			car._checkpoints = [];
			for (var j = 0; j < this._circuit._checkpoints.length; ++j)
				car._checkpoints.push( this._circuit._checkpoints[j] );

			car._alive = true;
			car._fitness = 0;
			car._total_update = 0;
			car._trail = [];
		}
	};

	//

	return createSimulation;
})
