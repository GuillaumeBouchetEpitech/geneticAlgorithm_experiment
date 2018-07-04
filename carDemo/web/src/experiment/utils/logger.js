
class Logger {

	constructor(elem_id) {

		this._elem = document.getElementById( elem_id );

		if (!this._elem)
			throw new Error("failed to get the textarea");

		this._elem.innerHTML = "logger initialised";
		this._elem.cols = 60;
		this._elem.rows = 30;

		this._arr_data = [];
		this._max_data = 30;
	}

	clear() {
		this._elem.innerHTML = ""
		this._arr_data = [];
	}

	push_line(str_line) {

		this._arr_data.push(str_line);

		if (this._arr_data.length >= this._max_data)
			this._arr_data.shift();

		this._elem.innerHTML = this._arr_data.join("\n");
	}

}

export default Logger;
