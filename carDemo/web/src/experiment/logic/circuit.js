
import { MathUtils } from "../utils/index.js";

//

class Circuit {

	constructor(circuitData) {

		//
		// generate checkpoints

		this._checkpoints = [];
		for (const elem in circuitData) {

			this._checkpoints.push({
				p1: { x: circuitData[elem][0], y: circuitData[elem][1] },
				p2: { x: circuitData[elem][2], y: circuitData[elem][3] }
			});
		}

		//
		// generate walls

		this._walls = [];
		for (let ii = 1; ii < this._checkpoints.length; ++ii) {

			this._walls.push({
				p1: this._checkpoints[ii - 1].p1,
				p2: this._checkpoints[ii].p1
			});

			this._walls.push({
				p1: this._checkpoints[ii - 1].p2,
				p2: this._checkpoints[ii].p2
			});
		}

		//
		// duplicate checkpoints

		const nextCheckpoints = [];
		nextCheckpoints.push(this._checkpoints[0]);
		for (let ii = 1; ii < this._checkpoints.length; ++ii) {

			const line1 = this._checkpoints[ii - 1];
			const line2 = this._checkpoints[ii];

			for (let jj = 0.25; jj < 1.0; jj += 0.25) {

				nextCheckpoints.push({
					p1: MathUtils.lerp(line1.p1, line2.p1, jj),
					p2: MathUtils.lerp(line1.p2, line2.p2, jj)
				});
			}

			nextCheckpoints.push(line2);
		}
		this._checkpoints = nextCheckpoints;

		//
		// expand the walls (fix some rare collisions bugs)

		for (let ii = 0; ii < this._walls.length; ++ii) {

			const wall = this._walls[ii];

			wall.p1 = MathUtils.lerp(wall.p1, wall.p2, -0.01); // ~1% longer at the start
			wall.p2 = MathUtils.lerp(wall.p1, wall.p2, +1.01); // ~1% longer at the end
		}

		//
		// generate the start position/angle

		{
			const l1 = this._checkpoints[0];
			this._startPosition = {
				x: l1.p1.x + (l1.p2.x - l1.p1.x) * 0.5,
				y: l1.p1.y + (l1.p2.y - l1.p1.y) * 0.5
			};

			const l2 = this._checkpoints[1];
			const pos = {
				x: l2.p1.x + (l2.p2.x - l2.p1.x) * 0.5,
				y: l2.p1.y + (l2.p2.y - l2.p1.y) * 0.5
			};

			this._startAngle = Math.atan2(
				pos.y - this._startPosition.y,
				pos.x - this._startPosition.x
			);
		}

		//
		// generate the stop position/angle

		{
			const l1 = this._checkpoints[this._checkpoints.length-1];
			this._stopPosition = {
				x: l1.p1.x + (l1.p2.x - l1.p1.x) * 0.5,
				y: l1.p1.y + (l1.p2.y - l1.p1.y) * 0.5
			};

			const l2 = this._checkpoints[this._checkpoints.length-2];
			const pos = {
				x: l2.p1.x + (l2.p2.x - l2.p1.x) * 0.5,
				y: l2.p1.y + (l2.p2.y - l2.p1.y) * 0.5
			};

			this._stopAngle = Math.atan2(
				pos.y - this._stopPosition.y,
				pos.x - this._stopPosition.x
			);
		}
	}

	get checkpoints() { return this._checkpoints; }

	get walls() { return this._walls; }

	get startPosition() { return this._startPosition; }
	get startAngle() { return this._startAngle; }

	get stopPosition() { return this._stopPosition; }
	get stopAngle() { return this._stopAngle; }
}

//

export default Circuit;
