

define([],function()
{

	//

	var createRenderer = function (elem_id)
	{
		this._canvas = document.getElementById( elem_id );

		if (!this._canvas)
			throw new Error("failed to get the canvas");

		this._ctx = this._canvas.getContext("2d");
	}

	//

	createRenderer.prototype.clear = function()
	{
		// this._ctx.clearRect(0,0,800,600);

		this._ctx.fillStyle="black";
		this._ctx.fillRect(0,0,800,600);
 	};

	//

	createRenderer.prototype.drawLine = function(x1, y1, x2, y2, color)
	{
		this._ctx.strokeStyle = color;
		this._ctx.lineWidth=1;
		this._ctx.beginPath();
		this._ctx.moveTo(x1,y1);
		this._ctx.lineTo(x2,y2);
		this._ctx.stroke();
	};

	//

	createRenderer.prototype.drawThickLine = function(x1, y1, x2, y2, color)
	{
		this._ctx.strokeStyle = color;
		this._ctx.lineWidth=4;
		this._ctx.beginPath();
		this._ctx.moveTo(x1,y1);
		this._ctx.lineTo(x2,y2);
		this._ctx.stroke();
	};

	//

	createRenderer.prototype.drawLines = function(arr_lines, color)
	{
		this._ctx.strokeStyle = color;
		this._ctx.lineWidth=1;
		this._ctx.beginPath();

		this._ctx.moveTo(arr_lines[0].x, arr_lines[0].y);
        for (var i = 1; i < arr_lines.length; ++i)
			this._ctx.lineTo(arr_lines[i].x, arr_lines[i].y);

		this._ctx.stroke();
	};

	//

	createRenderer.prototype.drawPoint = function(x, y, color)
	{
		this.drawLine(x-20, y-20, x+20, y+20, color);
		this.drawLine(x-20, y+20, x+20, y-20, color);
	};

	//

	return createRenderer;
})
