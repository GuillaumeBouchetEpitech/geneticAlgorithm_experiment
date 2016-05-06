

define(
	[
		'../utils/collision.js'
	],
	function(
		utils
	)
{

	//

	var createCircuit = function (elem_id)
	{
		// console.log("createCircuit");

		// retrieve data from element id

	    var elem = document.getElementById(elem_id).innerHTML;
	    var data = JSON.parse(elem);

	    // console.log(data);

	    //
		// generate checkpoints

		this._checkpoints = [];
		for (elem in data)
		{
			this._checkpoints.push({
				  p1: {x:data[elem][0], y:data[elem][1]}
				, p2: {x:data[elem][2], y:data[elem][3]}
			});
		}

		//
		// generate walls

		this._walls = [];
		for (var i = 1; i < this._checkpoints.length; ++i)
		{
			this._walls.push({
				  p1: this._checkpoints[i-1].p1
				, p2: this._checkpoints[i].p1
			});

			this._walls.push({
				  p1: this._checkpoints[i-1].p2
				, p2: this._checkpoints[i].p2
			});
		}

		//
		// duplicate checkpoints

		var	tmp_checkpoints = [];
		tmp_checkpoints.push(this._checkpoints[0]);
		for (var i = 1; i < this._checkpoints.length; ++i)
		{
			var l1 = this._checkpoints[i - 1];
			var l2 = this._checkpoints[i];

			for (var f = 0.25; f < 1.0; f += 0.25)
			{
				tmp_checkpoints.push({
					  p1: utils.lerp(l1.p1, l2.p1, f)
					, p2: utils.lerp(l1.p2, l2.p2, f)
				});
			}

			tmp_checkpoints.push(l2);
		}
		this._checkpoints = tmp_checkpoints;

		//
		// expand the walls (fix collisions bugs)

		for (var i = 0; i < this._walls.length; ++i)
		{
			this._walls[i].p1 = utils.lerp(this._walls[i].p1, this._walls[i].p2, -0.01);
			this._walls[i].p2 = utils.lerp(this._walls[i].p1, this._walls[i].p2, 1.01);
		}

		///
		///

		{
			var l1 = this._checkpoints[0];
			this._start_position = {
				  x: l1.p1.x + (l1.p2.x - l1.p1.x) * 0.5
				, y: l1.p1.y + (l1.p2.y - l1.p1.y) * 0.5
			}

			var l2 = this._checkpoints[1];
			var pos = {
				  x: l2.p1.x + (l2.p2.x - l2.p1.x) * 0.5
				, y: l2.p1.y + (l2.p2.y - l2.p1.y) * 0.5
			};

			this._start_angle = Math.atan2(
				pos.y - this._start_position.y,
				pos.x - this._start_position.x
			);
		}
	}

	//

	return createCircuit;
})
