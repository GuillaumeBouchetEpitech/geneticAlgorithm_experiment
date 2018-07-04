
import { MathUtils } from "../utils/index.js";

class Car {

	constructor(position, angle, checkpoints) {

		this._checkpoints = checkpoints;

		this.reset(position, angle);

		this._sensors = [];

		this._updateSensors();
	}

	reset(position, angle) {

		this._position = {
			x: position.x,
			y: position.y
		};
		this._angle = angle;

		this._alive = true;
		this._fitness = 0;
		this._nextCheckpoint = 0;
		this._healthInTicks = this._maxHealthInTicks;

		this._totalTicks = 0;
		this._trail = [];
	}

	update(delta, walls, ann) {

		if (!this._alive)
			return;

		//
		// health

		if (this._healthInTicks > 0)
			--this._healthInTicks;
		if (this._healthInTicks == 0) {
			this._healthInTicks = this._maxHealthInTicks;
			this._alive = false;
		}

		//
		// min update

		const input = [];

		for (let ii = 0; ii < this._sensors.length; ++ii)
			input.push(this._sensors[ii].result);

		const output = ann.process( input );

		const leftTheta = output[0];
		const rightTheta = output[1];

		if (Number.isNaN(leftTheta))	leftTheta = 0;
		if (Number.isNaN(rightTheta))	rightTheta = 0;

		const speedMax = 15;
		const steerMax = Math.PI / 32.0;

		this._angle += Math.max(-steerMax, Math.min(steerMax, leftTheta * steerMax));
		const speed = Math.max(-speedMax, Math.min(speedMax, rightTheta * speedMax));

		this._position.x += (speed * Math.cos(this._angle)) * delta;
		this._position.y += (speed * Math.sin(this._angle)) * delta;

		this._trail.push({ x: this._position.x, y: this._position.y });

		//

		this._collideWalls(walls);
		this._updateSensors();
		this._collideSensors(walls);
		this._collideCheckpoints();

		++this._totalTicks;
	}

	_collideWalls(walls) {

		for (let ii = 0; ii < walls.length; ++ii)
			if (MathUtils.collisionSegmentCercle(walls[ii].p1, walls[ii].p2, this._position, 10.0)) {

				this._alive = false;
				break;
			}
	}

	_updateSensors(walls) {

		this._sensors.length = 0;
		for (let ii = 0; ii < this._sensorsAngle.length; ++ii) {

			this._sensors.push({
				line: {
					p1: { x: 0, y: 0 },
					p2: {
						x: 200.0 * Math.cos(this._sensorsAngle[ii]),
						y: 200.0 * Math.sin(this._sensorsAngle[ii])
					}
				},
				result: 1.0
			});
		}

		// translate/rotate

		for (let ii = 0; ii < this._sensors.length; ++ii) {

			const sensor = this._sensors[ii];

			sensor.line.p1.x += this._position.x;
			sensor.line.p2.x += this._position.x;
			sensor.line.p1.y += this._position.y;
			sensor.line.p2.y += this._position.y;

			sensor.line.p1 = MathUtils.rotateVec2(sensor.line.p1, this._position, this._angle);
			sensor.line.p2 = MathUtils.rotateVec2(sensor.line.p2, this._position, this._angle);
		}
	}

	_collideSensors(walls) {

		for (let ii = 0; ii < this._sensors.length; ++ii) {

			const sensor = this._sensors[ii];

			sensor.result = 1.0;

			for (let jj = 0; jj < walls.length; ++jj) {

				const wall = walls[jj];

				const result = MathUtils.collisionSegSeg_partial(sensor.line.p1, sensor.line.p2, wall.p1, wall.p2);

				if (result >= 0.0 && result < 1.0 && result < sensor.result)
					sensor.result = result;
			}
		}
	}

	_collideCheckpoints() {

		if (this._nextCheckpoint < this._checkpoints.length) {

			const nextCheckpoint = this._checkpoints[this._nextCheckpoint];

			if (MathUtils.collisionSegmentCercle(nextCheckpoint.p1, nextCheckpoint.p2, this._position, 5.0)) {

				this._healthInTicks = this._maxHealthInTicks;
				++this._fitness;
				++this._nextCheckpoint;
			}
		}

		if (this._nextCheckpoint >= this._checkpoints.length) {

			// this will add an extra reward if it complete the circuit
			// -> it help to reward the faster cars
			this._fitness += (1000.0 / this._totalTicks);

			this._alive = false;
		}
	}


	get position() { return this._position; }
	get angle() { return this._angle; }
	get alive() { return this._alive; }
	get fitness() { return this._fitness; }
	get trail() { return this._trail; }
	get sensors() { return this._sensors; }
};

const sensorsSpread = Math.PI / 8.0;

Car.prototype._maxHealthInTicks = 50;

Car.prototype._sensorsAngle = [
	sensorsSpread * -2,
	sensorsSpread * -1,
	0.0,
	sensorsSpread * +1,
	sensorsSpread * +2,
];

export default Car;
