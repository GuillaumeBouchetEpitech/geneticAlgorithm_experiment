
//

class Renderer {

	constructor(canvas) {

		this._canvas = canvas;

		this._ctx = this._canvas.getContext("2d");

		this._origin_x = 0;
		this._origin_y = 0;
	}

	clear(in_x, in_y, in_w, in_h, color) {

		this._ctx.fillStyle = color || "black";

		const x = in_x || 0;
		const y = in_y || 0;
		const w = in_w || 800;
		const h = in_h || 600;

		this._ctx.fillRect(x, y, w, h);
	}

	setOrigin(x, y) {
		this._origin_x = x;
		this._origin_y = y;
	}

	setCenter(x, y) {
		this._origin_x = x + this._canvas.width / 2;
		this._origin_y = y + this._canvas.height / 2;
	}

	applyClipping(x1, y1, x2, y2) {

		const box1 = {
			l: this._origin_x,
			t: this._origin_y,
			r: this._origin_x + this._canvas.width,
			h: this._origin_y + this._canvas.height,
		};

		const box2 = {
			l: this._origin_x + Math.min(x1, x2),
			t: this._origin_y + Math.min(y1, y2),
			r: this._origin_x + Math.max(x1, x2),
			b: this._origin_y + Math.max(y1, y2),
		};

		return (!
			(box2.l >= box1.r) || // trop à droite
			(box2.r <= box1.l) || // trop à gauche
			(box2.t >= box1.b) || // trop en bas
			(box2.b <= box1.t)    // trop en haut
		);
	}

	drawLine(x1, y1, x2, y2, color, size) {

		if (!this.applyClipping(x1, y1, x2, y2))
			return;

		this._ctx.strokeStyle = color;
		this._ctx.lineWidth = size || 1;
		this._ctx.beginPath();
		this._ctx.moveTo(this._origin_x+x1, this._origin_y+y1);
		this._ctx.lineTo(this._origin_x+x2, this._origin_y+y2);
		this._ctx.stroke();
	}

	drawThickLine(x1, y1, x2, y2, color) {

		this.drawLine(x1, y1, x2, y2, color, 4);
	}

	drawLineStrip(lines, color, size) {

		this._ctx.strokeStyle = color;
		this._ctx.lineWidth = size || 1;
		this._ctx.beginPath();

		this._ctx.moveTo(this._origin_x + lines[0].x, this._origin_y + lines[0].y);
		for (let ii = 1; ii < lines.length; ++ii)
			this._ctx.lineTo(this._origin_x + lines[ii].x, this._origin_y + lines[ii].y);

		this._ctx.stroke();
	}

	drawLines(lines, color, size) {

		this._ctx.strokeStyle = color;
		this._ctx.lineWidth = size || 1;
		this._ctx.beginPath();

		for (let ii = 0; ii < lines.length; ++ii) {

			this._ctx.moveTo(this._origin_x + lines[ii].p1.x, this._origin_y + lines[ii].p1.y);
			this._ctx.lineTo(this._origin_x + lines[ii].p2.x, this._origin_y + lines[ii].p2.y);
		}

		this._ctx.stroke();
	}

	drawPoint(x, y, color, size) {

		const pointSize = 10;
		this.drawLine(x - pointSize, y - pointSize, x + pointSize, y + pointSize, color, size);
		this.drawLine(x - pointSize, y + pointSize, x + pointSize, y - pointSize, color, size);
	}

	drawCircle(x, y, radius, color) {

		this._ctx.beginPath();
		this._ctx.arc(this._origin_x + x, this._origin_y + y, radius, 0, 2 * Math.PI, false);
		this._ctx.fillStyle = color;
		this._ctx.fill();
	}

	get width() {
		return this._canvas.width;
	}

	get height() {
		return this._canvas.height;
	}
}

// export default Renderer;
module.exports = Renderer;
