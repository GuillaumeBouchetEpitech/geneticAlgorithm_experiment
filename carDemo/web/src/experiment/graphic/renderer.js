
//

class Renderer {

	constructor(canvas) {

		this._canvas = canvas;

		this._ctx = this._canvas.getContext("2d");

		this._center_x = 0;
		this._center_y = 0;
	}

	clear(in_x, in_y, in_w, in_h, color) {

		this._ctx.fillStyle = color || "black";

		const x = in_x || 0;
		const y = in_y || 0;
		const w = in_w || 800;
		const h = in_h || 600;

		this._ctx.fillRect(x, y, w, h);
	};

	setCenter(x, y) {
		this._center_x = x;
		this._center_y = y;
	};


	drawLine(x1, y1, x2, y2, color, size) {
		this._ctx.strokeStyle = color;
		this._ctx.lineWidth = size || 1;
		this._ctx.beginPath();
		this._ctx.moveTo(this._center_x+x1, this._center_y+y1);
		this._ctx.lineTo(this._center_x+x2, this._center_y+y2);
		this._ctx.stroke();
	};

	drawThickLine(x1, y1, x2, y2, color) {

		this.drawLine(x1, y1, x2, y2, color, 4);
	};

	drawLineStrip(lines, color, size) {

		this._ctx.strokeStyle = color;
		this._ctx.lineWidth = size || 1;
		this._ctx.beginPath();

		this._ctx.moveTo(this._center_x + lines[0].x, this._center_y + lines[0].y);
		for (let ii = 1; ii < lines.length; ++ii)
			this._ctx.lineTo(this._center_x + lines[ii].x, this._center_y + lines[ii].y);

		this._ctx.stroke();
	};

	drawLines(lines, color, size) {

		this._ctx.strokeStyle = color;
		this._ctx.lineWidth = size || 1;
		this._ctx.beginPath();

		for (let ii = 0; ii < lines.length; ++ii) {

			this._ctx.moveTo(this._center_x + lines[ii].p1.x, this._center_y + lines[ii].p1.y);
			this._ctx.lineTo(this._center_x + lines[ii].p2.x, this._center_y + lines[ii].p2.y);
		}

		this._ctx.stroke();
	};

	drawPoint(x, y, color, size) {

		const pointSize = 10;
		this.drawLine(x - pointSize, y - pointSize, x + pointSize, y + pointSize, color, size);
		this.drawLine(x - pointSize, y + pointSize, x + pointSize, y - pointSize, color, size);
	};

	drawCircle(x, y, radius, color) {

		this._ctx.beginPath();
		this._ctx.arc(this._center_x + x, this._center_y + y, radius, 0, 2 * Math.PI, false);
		this._ctx.fillStyle = color;
		this._ctx.fill();
	};
}

export default Renderer;
