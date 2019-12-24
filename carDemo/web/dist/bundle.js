(function(){function r(e,n,t){function o(i,f){if(!n[i]){if(!e[i]){var c="function"==typeof require&&require;if(!f&&c)return c(i,!0);if(u)return u(i,!0);var a=new Error("Cannot find module '"+i+"'");throw a.code="MODULE_NOT_FOUND",a}var p=n[i]={exports:{}};e[i][0].call(p.exports,function(r){var n=e[i][1][r];return o(n||r)},p,p.exports,r,e,n,t)}return n[i].exports}for(var u="function"==typeof require&&require,i=0;i<t.length;i++)o(t[i]);return o}return r})()({1:[function(require,module,exports){
/*!
 * FPSMeter 0.3.1 - 9th May 2013
 * https://github.com/Darsain/fpsmeter
 *
 * Licensed under the MIT license.
 * http://opensource.org/licenses/MIT
 */
;(function (w, undefined) {
	'use strict';

	/**
	 * Create a new element.
	 *
	 * @param  {String} name Element type name.
	 *
	 * @return {Element}
	 */
	function newEl(name) {
		return document.createElement(name);
	}

	/**
	 * Apply theme CSS properties to element.
	 *
	 * @param  {Element} element DOM element.
	 * @param  {Object}  theme   Theme object.
	 *
	 * @return {Element}
	 */
	function applyTheme(element, theme) {
		for (var name in theme) {
			try {
				element.style[name] = theme[name];
			} catch (e) {}
		}
		return element;
	}

	/**
	 * Return type of the value.
	 *
	 * @param  {Mixed} value
	 *
	 * @return {String}
	 */
	function type(value) {
		if (value == null) {
			return String(value);
		}

		if (typeof value === 'object' || typeof value === 'function') {
			return Object.prototype.toString.call(value).match(/\s([a-z]+)/i)[1].toLowerCase() || 'object';
		}

		return typeof value;
	}

	/**
	 * Check whether the value is in an array.
	 *
	 * @param  {Mixed} value
	 * @param  {Array} array
	 *
	 * @return {Integer} Array index or -1 when not found.
	 */
	function inArray(value, array) {
		if (type(array) !== 'array') {
			return -1;
		}
		if (array.indexOf) {
			return array.indexOf(value);
		}
		for (var i = 0, l = array.length; i < l; i++) {
			if (array[i] === value) {
				return i;
			}
		}
		return -1;
	}

	/**
	 * Poor man's deep object extend.
	 *
	 * Example:
	 *   extend({}, defaults, options);
	 *
	 * @return {Void}
	 */
	function extend() {
		var args = arguments;
		for (var key in args[1]) {
			if (args[1].hasOwnProperty(key)) {
				switch (type(args[1][key])) {
					case 'object':
						args[0][key] = extend({}, args[0][key], args[1][key]);
						break;

					case 'array':
						args[0][key] = args[1][key].slice(0);
						break;

					default:
						args[0][key] = args[1][key];
				}
			}
		}
		return args.length > 2 ?
			extend.apply(null, [args[0]].concat(Array.prototype.slice.call(args, 2))) :
			args[0];
	}

	/**
	 * Convert HSL color to HEX string.
	 *
	 * @param  {Array} hsl Array with [hue, saturation, lightness].
	 *
	 * @return {Array} Array with [red, green, blue].
	 */
	function hslToHex(h, s, l) {
		var r, g, b;
		var v, min, sv, sextant, fract, vsf;

		if (l <= 0.5) {
			v = l * (1 + s);
		} else {
			v = l + s - l * s;
		}

		if (v === 0) {
			return '#000';
		} else {
			min = 2 * l - v;
			sv = (v - min) / v;
			h = 6 * h;
			sextant = Math.floor(h);
			fract = h - sextant;
			vsf = v * sv * fract;
			if (sextant === 0 || sextant === 6) {
				r = v;
				g = min + vsf;
				b = min;
			} else if (sextant === 1) {
				r = v - vsf;
				g = v;
				b = min;
			} else if (sextant === 2) {
				r = min;
				g = v;
				b = min + vsf;
			} else if (sextant === 3) {
				r = min;
				g = v - vsf;
				b = v;
			} else if (sextant === 4) {
				r = min + vsf;
				g = min;
				b = v;
			} else {
				r = v;
				g = min;
				b = v - vsf;
			}
			return '#' + componentToHex(r) + componentToHex(g) + componentToHex(b);
		}
	}

	/**
	 * Helper function for hslToHex.
	 */
	function componentToHex(c) {
		c = Math.round(c * 255).toString(16);
		return c.length === 1 ? '0' + c : c;
	}

	/**
	 * Manage element event listeners.
	 *
	 * @param  {Node}     element
	 * @param  {Event}    eventName
	 * @param  {Function} handler
	 * @param  {Bool}     remove
	 *
	 * @return {Void}
	 */
	function listener(element, eventName, handler, remove) {
		if (element.addEventListener) {
			element[remove ? 'removeEventListener' : 'addEventListener'](eventName, handler, false);
		} else if (element.attachEvent) {
			element[remove ? 'detachEvent' : 'attachEvent']('on' + eventName, handler);
		}
	}

	// Preferred timing funtion
	var getTime;
	(function () {
		var perf = w.performance;
		if (perf && (perf.now || perf.webkitNow)) {
			var perfNow = perf.now ? 'now' : 'webkitNow';
			getTime = perf[perfNow].bind(perf);
		} else {
			getTime = function () {
				return +new Date();
			};
		}
	}());

	// Local WindowAnimationTiming interface polyfill
	var cAF = w.cancelAnimationFrame || w.cancelRequestAnimationFrame;
	var rAF = w.requestAnimationFrame;
	(function () {
		var vendors = ['moz', 'webkit', 'o'];
		var lastTime = 0;

		// For a more accurate WindowAnimationTiming interface implementation, ditch the native
		// requestAnimationFrame when cancelAnimationFrame is not present (older versions of Firefox)
		for (var i = 0, l = vendors.length; i < l && !cAF; ++i) {
			cAF = w[vendors[i]+'CancelAnimationFrame'] || w[vendors[i]+'CancelRequestAnimationFrame'];
			rAF = cAF && w[vendors[i]+'RequestAnimationFrame'];
		}

		if (!cAF) {
			rAF = function (callback) {
				var currTime = getTime();
				var timeToCall = Math.max(0, 16 - (currTime - lastTime));
				lastTime = currTime + timeToCall;
				return w.setTimeout(function () { callback(currTime + timeToCall); }, timeToCall);
			};

			cAF = function (id) {
				clearTimeout(id);
			};
		}
	}());

	// Property name for assigning element text content
	var textProp = type(document.createElement('div').textContent) === 'string' ? 'textContent' : 'innerText';

	/**
	 * FPSMeter class.
	 *
	 * @param {Element} anchor  Element to append the meter to. Default is document.body.
	 * @param {Object}  options Object with options.
	 */
	function FPSMeter(anchor, options) {
		// Optional arguments
		if (type(anchor) === 'object' && anchor.nodeType === undefined) {
			options = anchor;
			anchor = document.body;
		}
		if (!anchor) {
			anchor = document.body;
		}

		// Private properties
		var self = this;
		var o = extend({}, FPSMeter.defaults, options || {});

		var el = {};
		var cols = [];
		var theme, heatmaps;
		var heatDepth = 100;
		var heating = [];

		var thisFrameTime = 0;
		var frameTime = o.threshold;
		var frameStart = 0;
		var lastLoop = getTime() - frameTime;
		var time;

		var fpsHistory = [];
		var durationHistory = [];

		var frameID, renderID;
		var showFps = o.show === 'fps';
		var graphHeight, count, i, j;

		// Exposed properties
		self.options = o;
		self.fps = 0;
		self.duration = 0;
		self.isPaused = 0;

		/**
		 * Tick start for measuring the actual rendering duration.
		 *
		 * @return {Void}
		 */
		self.tickStart = function () {
			frameStart = getTime();
		};

		/**
		 * FPS tick.
		 *
		 * @return {Void}
		 */
		self.tick = function () {
			time = getTime();
			thisFrameTime = time - lastLoop;
			frameTime += (thisFrameTime - frameTime) / o.smoothing;
			self.fps = 1000 / frameTime;
			self.duration = frameStart < lastLoop ? frameTime : time - frameStart;
			lastLoop = time;
		};

		/**
		 * Pause display rendering.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.pause = function () {
			if (frameID) {
				self.isPaused = 1;
				clearTimeout(frameID);
				cAF(frameID);
				cAF(renderID);
				frameID = renderID = 0;
			}
			return self;
		};

		/**
		 * Resume display rendering.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.resume = function () {
			if (!frameID) {
				self.isPaused = 0;
				requestRender();
			}
			return self;
		};

		/**
		 * Update options.
		 *
		 * @param {String} name  Option name.
		 * @param {Mixed}  value New value.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.set = function (name, value) {
			o[name] = value;
			showFps = o.show === 'fps';

			// Rebuild or reposition elements when specific option has been updated
			if (inArray(name, rebuilders) !== -1) {
				createMeter();
			}
			if (inArray(name, repositioners) !== -1) {
				positionMeter();
			}
			return self;
		};

		/**
		 * Change meter into rendering duration mode.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.showDuration = function () {
			self.set('show', 'ms');
			return self;
		};

		/**
		 * Change meter into FPS mode.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.showFps = function () {
			self.set('show', 'fps');
			return self;
		};

		/**
		 * Toggles between show: 'fps' and show: 'duration'.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.toggle = function () {
			self.set('show', showFps ? 'ms' : 'fps');
			return self;
		};

		/**
		 * Hide the FPSMeter. Also pauses the rendering.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.hide = function () {
			self.pause();
			el.container.style.display = 'none';
			return self;
		};

		/**
		 * Show the FPSMeter. Also resumes the rendering.
		 *
		 * @return {Object} FPSMeter instance.
		 */
		self.show = function () {
			self.resume();
			el.container.style.display = 'block';
			return self;
		};

		/**
		 * Check the current FPS and save it in history.
		 *
		 * @return {Void}
		 */
		function historyTick() {
			for (i = o.history; i--;) {
				fpsHistory[i] = i === 0 ? self.fps : fpsHistory[i-1];
				durationHistory[i] = i === 0 ? self.duration : durationHistory[i-1];
			}
		}

		/**
		 * Returns heat hex color based on values passed.
		 *
		 * @param  {Integer} heatmap
		 * @param  {Integer} value
		 * @param  {Integer} min
		 * @param  {Integer} max
		 *
		 * @return {Integer}
		 */
		function getHeat(heatmap, value, min, max) {
			return heatmaps[0|heatmap][Math.round(Math.min((value - min) / (max - min) * heatDepth, heatDepth))];
		}

		/**
		 * Update counter number and legend.
		 *
		 * @return {Void}
		 */
		function updateCounter() {
			// Update legend only when changed
			if (el.legend.fps !== showFps) {
				el.legend.fps = showFps;
				el.legend[textProp] = showFps ? 'FPS' : 'ms';
			}
			// Update counter with a nicely formated & readable number
			count = showFps ? self.fps : self.duration;
			el.count[textProp] = count > 999 ? '999+' : count.toFixed(count > 99 ? 0 : o.decimals);
		}

		/**
		 * Render current FPS state.
		 *
		 * @return {Void}
		 */
		function render() {
			time = getTime();
			// If renderer stopped reporting, do a simulated drop to 0 fps
			if (lastLoop < time - o.threshold) {
				self.fps -= self.fps / Math.max(1, o.smoothing * 60 / o.interval);
				self.duration = 1000 / self.fps;
			}

			historyTick();
			updateCounter();

			// Apply heat to elements
			if (o.heat) {
				if (heating.length) {
					for (i = heating.length; i--;) {
						heating[i].el.style[theme[heating[i].name].heatOn] = showFps ?
							getHeat(theme[heating[i].name].heatmap, self.fps, 0, o.maxFps) :
							getHeat(theme[heating[i].name].heatmap, self.duration, o.threshold, 0);
					}
				}

				if (el.graph && theme.column.heatOn) {
					for (i = cols.length; i--;) {
						cols[i].style[theme.column.heatOn] = showFps ?
							getHeat(theme.column.heatmap, fpsHistory[i], 0, o.maxFps) :
							getHeat(theme.column.heatmap, durationHistory[i], o.threshold, 0);
					}
				}
			}

			// Update graph columns height
			if (el.graph) {
				for (j = 0; j < o.history; j++) {
					cols[j].style.height = (showFps ?
						(fpsHistory[j] ? Math.round(graphHeight / o.maxFps * Math.min(fpsHistory[j], o.maxFps)) : 0) :
						(durationHistory[j] ? Math.round(graphHeight / o.threshold * Math.min(durationHistory[j], o.threshold)) : 0)
					) + 'px';
				}
			}
		}

		/**
		 * Request rendering loop.
		 *
		 * @return {Int} Animation frame index.
		 */
		function requestRender() {
			if (o.interval < 20) {
				frameID = rAF(requestRender);
				render();
			} else {
				frameID = setTimeout(requestRender, o.interval);
				renderID = rAF(render);
			}
		}

		/**
		 * Meter events handler.
		 *
		 * @return {Void}
		 */
		function eventHandler(event) {
			event = event || window.event;
			if (event.preventDefault) {
				event.preventDefault();
				event.stopPropagation();
			} else {
				event.returnValue = false;
				event.cancelBubble = true;
			}
			self.toggle();
		}

		/**
		 * Destroys the current FPSMeter instance.
		 *
		 * @return {Void}
		 */
		self.destroy = function () {
			// Stop rendering
			self.pause();
			// Remove elements
			removeMeter();
			// Stop listening
			self.tick = self.tickStart = function () {};
		};

		/**
		 * Remove meter element.
		 *
		 * @return {Void}
		 */
		function removeMeter() {
			// Unbind listeners
			if (o.toggleOn) {
				listener(el.container, o.toggleOn, eventHandler, 1);
			}
			// Detach element
			anchor.removeChild(el.container);
		}

		/**
		 * Sets the theme, and generates heatmaps when needed.
		 */
		function setTheme() {
			theme = FPSMeter.theme[o.theme];

			// Generate heatmaps
			heatmaps = theme.compiledHeatmaps || [];
			if (!heatmaps.length && theme.heatmaps.length) {
				for (j = 0; j < theme.heatmaps.length; j++) {
					heatmaps[j] = [];
					for (i = 0; i <= heatDepth; i++) {
						heatmaps[j][i] = hslToHex(0.33 / heatDepth * i, theme.heatmaps[j].saturation, theme.heatmaps[j].lightness);
					}
				}
				theme.compiledHeatmaps = heatmaps;
			}
		}

		/**
		 * Creates and attaches the meter element.
		 *
		 * @return {Void}
		 */
		function createMeter() {
			// Remove old meter if present
			if (el.container) {
				removeMeter();
			}

			// Set theme
			setTheme();

			// Create elements
			el.container = applyTheme(newEl('div'), theme.container);
			el.count = el.container.appendChild(applyTheme(newEl('div'), theme.count));
			el.legend = el.container.appendChild(applyTheme(newEl('div'), theme.legend));
			el.graph = o.graph ? el.container.appendChild(applyTheme(newEl('div'), theme.graph)) : 0;

			// Add elements to heating array
			heating.length = 0;
			for (var key in el) {
				if (el[key] && theme[key].heatOn) {
					heating.push({
						name: key,
						el: el[key]
					});
				}
			}

			// Graph
			cols.length = 0;
			if (el.graph) {
				// Create graph
				el.graph.style.width = (o.history * theme.column.width + (o.history - 1) * theme.column.spacing) + 'px';

				// Add columns
				for (i = 0; i < o.history; i++) {
					cols[i] = el.graph.appendChild(applyTheme(newEl('div'), theme.column));
					cols[i].style.position = 'absolute';
					cols[i].style.bottom = 0;
					cols[i].style.right = (i * theme.column.width + i * theme.column.spacing) + 'px';
					cols[i].style.width = theme.column.width + 'px';
					cols[i].style.height = '0px';
				}
			}

			// Set the initial state
			positionMeter();
			updateCounter();

			// Append container to anchor
			anchor.appendChild(el.container);

			// Retrieve graph height after it was appended to DOM
			if (el.graph) {
				graphHeight = el.graph.clientHeight;
			}

			// Add event listeners
			if (o.toggleOn) {
				if (o.toggleOn === 'click') {
					el.container.style.cursor = 'pointer';
				}
				listener(el.container, o.toggleOn, eventHandler);
			}
		}

		/**
		 * Positions the meter based on options.
		 *
		 * @return {Void}
		 */
		function positionMeter() {
			applyTheme(el.container, o);
		}

		/**
		 * Construct.
		 */
		(function () {
			// Create meter element
			createMeter();
			// Start rendering
			requestRender();
		}());
	}

	// Expose the extend function
	FPSMeter.extend = extend;

	// Expose the FPSMeter class
	window.FPSMeter = FPSMeter;

	// Default options
	FPSMeter.defaults = {
		interval:  100,     // Update interval in milliseconds.
		smoothing: 10,      // Spike smoothing strength. 1 means no smoothing.
		show:      'fps',   // Whether to show 'fps', or 'ms' = frame duration in milliseconds.
		toggleOn:  'click', // Toggle between show 'fps' and 'ms' on this event.
		decimals:  1,       // Number of decimals in FPS number. 1 = 59.9, 2 = 59.94, ...
		maxFps:    60,      // Max expected FPS value.
		threshold: 100,     // Minimal tick reporting interval in milliseconds.

		// Meter position
		position: 'absolute', // Meter position.
		zIndex:   10,         // Meter Z index.
		left:     '5px',      // Meter left offset.
		top:      '5px',      // Meter top offset.
		right:    'auto',     // Meter right offset.
		bottom:   'auto',     // Meter bottom offset.
		margin:   '0 0 0 0',  // Meter margin. Helps with centering the counter when left: 50%;

		// Theme
		theme: 'dark', // Meter theme. Build in: 'dark', 'light', 'transparent', 'colorful'.
		heat:  0,      // Allow themes to use coloring by FPS heat. 0 FPS = red, maxFps = green.

		// Graph
		graph:   0, // Whether to show history graph.
		history: 20 // How many history states to show in a graph.
	};

	// Option names that trigger FPSMeter rebuild or reposition when modified
	var rebuilders = [
		'toggleOn',
		'theme',
		'heat',
		'graph',
		'history'
	];
	var repositioners = [
		'position',
		'zIndex',
		'left',
		'top',
		'right',
		'bottom',
		'margin'
	];
}(window));
;(function (w, FPSMeter, undefined) {
	'use strict';

	// Themes object
	FPSMeter.theme = {};

	// Base theme with layout, no colors
	var base = FPSMeter.theme.base = {
		heatmaps: [],
		container: {
			// Settings
			heatOn: null,
			heatmap: null,

			// Styles
			padding: '5px',
			minWidth: '95px',
			height: '30px',
			lineHeight: '30px',
			textAlign: 'right',
			textShadow: 'none'
		},
		count: {
			// Settings
			heatOn: null,
			heatmap: null,

			// Styles
			position: 'absolute',
			top: 0,
			right: 0,
			padding: '5px 10px',
			height: '30px',
			fontSize: '24px',
			fontFamily: 'Consolas, Andale Mono, monospace',
			zIndex: 2
		},
		legend: {
			// Settings
			heatOn: null,
			heatmap: null,

			// Styles
			position: 'absolute',
			top: 0,
			left: 0,
			padding: '5px 10px',
			height: '30px',
			fontSize: '12px',
			lineHeight: '32px',
			fontFamily: 'sans-serif',
			textAlign: 'left',
			zIndex: 2
		},
		graph: {
			// Settings
			heatOn: null,
			heatmap: null,

			// Styles
			position: 'relative',
			boxSizing: 'padding-box',
			MozBoxSizing: 'padding-box',
			height: '100%',
			zIndex: 1
		},
		column: {
			// Settings
			width: 4,
			spacing: 1,
			heatOn: null,
			heatmap: null
		}
	};

	// Dark theme
	FPSMeter.theme.dark = FPSMeter.extend({}, base, {
		heatmaps: [{
			saturation: 0.8,
			lightness: 0.8
		}],
		container: {
			background: '#222',
			color: '#fff',
			border: '1px solid #1a1a1a',
			textShadow: '1px 1px 0 #222'
		},
		count: {
			heatOn: 'color'
		},
		column: {
			background: '#3f3f3f'
		}
	});

	// Light theme
	FPSMeter.theme.light = FPSMeter.extend({}, base, {
		heatmaps: [{
			saturation: 0.5,
			lightness: 0.5
		}],
		container: {
			color: '#666',
			background: '#fff',
			textShadow: '1px 1px 0 rgba(255,255,255,.5), -1px -1px 0 rgba(255,255,255,.5)',
			boxShadow: '0 0 0 1px rgba(0,0,0,.1)'
		},
		count: {
			heatOn: 'color'
		},
		column: {
			background: '#eaeaea'
		}
	});

	// Colorful theme
	FPSMeter.theme.colorful = FPSMeter.extend({}, base, {
		heatmaps: [{
			saturation: 0.5,
			lightness: 0.6
		}],
		container: {
			heatOn: 'backgroundColor',
			background: '#888',
			color: '#fff',
			textShadow: '1px 1px 0 rgba(0,0,0,.2)',
			boxShadow: '0 0 0 1px rgba(0,0,0,.1)'
		},
		column: {
			background: '#777',
			backgroundColor: 'rgba(0,0,0,.2)'
		}
	});

	// Transparent theme
	FPSMeter.theme.transparent = FPSMeter.extend({}, base, {
		heatmaps: [{
			saturation: 0.8,
			lightness: 0.5
		}],
		container: {
			padding: 0,
			color: '#fff',
			textShadow: '1px 1px 0 rgba(0,0,0,.5)'
		},
		count: {
			padding: '0 5px',
			height: '40px',
			lineHeight: '40px'
		},
		legend: {
			padding: '0 5px',
			height: '40px',
			lineHeight: '42px'
		},
		graph: {
			height: '40px'
		},
		column: {
			width: 5,
			background: '#999',
			heatOn: 'backgroundColor',
			opacity: 0.5
		}
	});
}(window, FPSMeter));
},{}],2:[function(require,module,exports){

const Renderer = require('./graphic/Renderer.js');
const Simulation = require('./simulation/Simulation.js');

const MathUtils = require('./utilities/MathUtils.js');

//

class Experiment {

    constructor(canvas, circuitData) {

        this._renderer = new Renderer(canvas);
        this._simulation = new Simulation(circuitData);

        this._leadingCar = { index: -1, timeout: 0 };
        this._cameraOrigin = { x: 0, y: 0 };
    }

    update() {

        const fixedDelta = 0.025;
        const simulationDelta = fixedDelta * 10;

        for (let ii = 0; ii < 3; ++ii)
            this._simulation.update(simulationDelta);

        this._updateCamera(fixedDelta);
    }

    render() {

        this._renderer.clear();


        //
        // render circuit

        this._renderer.drawLines(this._simulation.circuit.checkpoints, "#ffffff", 3);
        this._renderer.drawLines(this._simulation.circuit.walls, "#0000ff", 2);

        this._renderer.drawPoint(
            this._simulation.circuit.startPosition.x,
            this._simulation.circuit.startPosition.y,
            "#ffffff",
            2
        );

        this._renderer.drawLine(
            this._simulation.circuit.startPosition.x,
            this._simulation.circuit.startPosition.y,
            this._simulation.circuit.startPosition.x + Math.cos(this._simulation.circuit.startAngle) * 50.0,
            this._simulation.circuit.startPosition.y + Math.sin(this._simulation.circuit.startAngle) * 50.0,
            "#ffffff",
            2
        );

        // render circuit
        //

        //
        // render cars

        const cars = this._simulation.cars;
        for (let ii = 0; ii < cars.length; ++ii)
            this._renderCar(cars[ii], "#00ff00", (ii == this._leadingCar.index));

        //
        // render trails

        const trail_colors = [ "#ffffff", "#00ff00", "#00cc00", "#009900", "#006600", "#003300" ];
        let trail_colors_index = 0;

        const trails = this._simulation.trails;
        for (let ii = (trails.length - 1); ii >= 0; --ii) { // from last to first

            const trail = trails[ii];
            const color = trail_colors[trail_colors_index++];

            this._renderer.drawLineStrip(trail, color);
        }

        //
        // render HUD

        this._renderHUD();
    }

    _updateCamera(delta) {

        this._leadingCar.timeout -= delta;

        if (this._leadingCar.index == -1 || this._leadingCar.timeout <= 0) {

            this._leadingCar.index = -1;
            this._leadingCar.timeout = 0.5;

            let currentBest;
            for (let ii = 0; ii < this._simulation.cars.length; ++ii) {

                const currentCar = this._simulation.cars[ii];

                if (!currentCar.alive)
                    continue;

                if (currentBest && currentBest.fitness > currentCar.fitness)
                    continue;

                currentBest = currentCar;
                this._leadingCar.index = ii;
            }
        }

        let target;
        if (this._leadingCar.index != -1)
            target = this._simulation.cars[this._leadingCar.index].position;
        else
            target = { x: 0, y: 0 };

        this._cameraOrigin = MathUtils.lerp(this._cameraOrigin, target, 0.1);

        this._renderer.setCenter(-this._cameraOrigin.x, -this._cameraOrigin.y);
    }

    _renderCar(car, alive_color, show_sensors) {

        const position = car.position;
        const angle = car.angle;

        const size_h = 25.0;
        const size_v = 12.5;

        const positions = [
            { x: position.x - size_h, y: position.y - size_v },
            { x: position.x + size_h, y: position.y - size_v },
            { x: position.x + size_h, y: position.y + size_v },
            { x: position.x - size_h, y: position.y + size_v }
        ];

        for (let ii = 0; ii < 4; ++ii)
            positions[ii] = MathUtils.rotateVec2(positions[ii], position, angle);

        const color = (car.alive ? alive_color :  "#ff0000");

        this._renderer.drawLine(positions[0].x, positions[0].y, positions[1].x, positions[1].y, color, 6);
        this._renderer.drawLine(positions[1].x, positions[1].y, positions[2].x, positions[2].y, color, 6);
        this._renderer.drawLine(positions[2].x, positions[2].y, positions[3].x, positions[3].y, color, 6);
        this._renderer.drawLine(positions[3].x, positions[3].y, positions[0].x, positions[0].y, color, 6);

        if (!car.alive || !show_sensors) // render sensors only if the car is alive
            return;

        const sensors = car.sensors;

        for (let ii = 0; ii < sensors.length; ++ii) {

            const sensor = sensors[ii];

            const impact = {
                x: sensor.line.p1.x + (sensor.line.p2.x - sensor.line.p1.x) * sensor.result,
                y: sensor.line.p1.y + (sensor.line.p2.y - sensor.line.p1.y) * sensor.result
            };

            this._renderer.drawLine(sensor.line.p1.x, sensor.line.p1.y, impact.x, impact.y, "#8888ff", 2);
            this._renderer.drawLine(impact.x, impact.y, sensor.line.p2.x, sensor.line.p2.y, "#ff8888", 2);

            this._renderer.drawPoint(impact.x, impact.y, "#ffff00", 2);
        }
    }

    _renderHUD() {

        let tmp_weights = null;

        const genomes = this._simulation.geneticAlgorithm._genomes;

        if (genomes.length > 0) {

            let index = 0;
            for (; index < genomes.length; ++index)
                if (genomes[index].car.alive)
                    break;

            if (index < genomes.length)
                tmp_weights = genomes[index].weights;
        }

        if (!tmp_weights)
            return;

        this._renderer.setOrigin(50, 200);

        let windex = 0;

        const topology = this._simulation._ann_topology

        for (let kk = 1; kk < topology.length; ++kk) {

            const prevLayer = topology[kk-1];
            const currLayer = topology[kk];

            const prevY = (1.0 - (prevLayer / 5)) * 185;
            const currY = (1.0 - (currLayer / 5)) * 185;

            for (let ii = 0; ii < prevLayer; ++ii)
                for (let jj = 0; jj < currLayer; ++jj) {

                    const currX = (kk-1) * 75;

                    const weightValue = tmp_weights[windex++];

                    let ratio = weightValue * 10;
                    if (ratio < 0)
                        ratio = -ratio;
                    if (ratio < 1)
                        ratio = 1;

                    if (weightValue > 0)
                        this._renderer.drawLine(currX, prevY+ii*75, currX+75, currY+jj*75, "#ff0000", ratio);
                    else
                        this._renderer.drawLine(currX, prevY+ii*75, currX+75, currY+jj*75, "#0000ff", ratio);
                }
        }

        for (let kk = 0; kk < topology.length; ++kk) {

            const currLayer = topology[kk];

            const currY = (1.0 - (currLayer / 5)) * 185;

            for (let ii = 0; ii < currLayer; ++ii) {

                const currX = (kk) * 75;

                this._renderer.drawCircle(currX, currY+ii*75, 13, "#ffff00");
                this._renderer.drawCircle(currX, currY+ii*75, 10, "#00ff00");
            }
        }
    }
}

//

module.exports = Experiment;

},{"./graphic/Renderer.js":3,"./simulation/Simulation.js":4,"./utilities/MathUtils.js":10}],3:[function(require,module,exports){

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

},{}],4:[function(require,module,exports){

const Circuit = require("./logic/Circuit.js");
const Car = require("./logic/Car.js");
const GeneticAlgorithm = require("./machineLearning/GeneticAlgorithm.js");

const circuitData = require("./circuits/default.js");

//

class Simulation {

	constructor() {

		//
		// circuit

		const genomeSize = 200;
		this._ann_topology = [5, 3, 2];

		this._geneticAlgorithm = new GeneticAlgorithm( genomeSize, this._ann_topology );
		this._circuit = new Circuit(circuitData);

		//
		// cars

		const position = this._circuit.startPosition;
		const angle = this._circuit.startAngle;
		const checkpoints = this._circuit.checkpoints;

		this._cars = [];
		for (let ii = 0; ii < genomeSize; ++ii) {

			const car = new Car(position, angle, checkpoints);

			this._geneticAlgorithm.genomes[ii].car = car;

			this._cars.push(car);
		}

		this._trails = [];
	}

	update(delta) {

		let readyToBreed = true;

		for (let ii = 0; ii < this._cars.length; ++ii) {

			if (!this._cars[ii].alive)
				continue;

			this._cars[ii].update(delta, this._circuit.walls, this._geneticAlgorithm.ANNs[ii]);

			readyToBreed = false;
		}

		// end of the current generation?

		if (!readyToBreed)
			return; // no

		// rate the genome

		for (let ii = 0; ii < this._cars.length; ++ii)
			this._geneticAlgorithm.genomes[ii].fitness = this._cars[ii].fitness;

		const progressWasMade = this._geneticAlgorithm.breedPopulation();

		// save the best trail

		if (progressWasMade) {

			this._trails.push( this._geneticAlgorithm._bestGenome.car.trail );

			if (this._trails.length > 5)
				this._trails.shift();
		}

		// reset the cars

		for (let ii = 0; ii < this._cars.length; ++ii) {

			const car = this._cars[ii];

			car.reset(this._circuit.startPosition, this._circuit.startAngle);

			this._geneticAlgorithm.genomes[ii].car = car;
		}
	}

	get geneticAlgorithm() { return this._geneticAlgorithm; }
	get circuit() { return this._circuit; }
	get cars() { return this._cars; }
	get trails() { return this._trails; }

}

//

module.exports = Simulation;

},{"./circuits/default.js":5,"./logic/Car.js":6,"./logic/Circuit.js":7,"./machineLearning/GeneticAlgorithm.js":9}],5:[function(require,module,exports){

module.exports = [
    [ 50,     0,  150,     0],
    [ 50,  -100,  150,  -100],
    [ 50,  -300,  150,  -300],


    [ 50,  -400,  150,  -350],
    [150,  -500,  250,  -450],
    [150,  -600,  250,  -650],

    [ 50,  -700,  150,  -750],

    [-50,  -800,   50,  -850],
    [-50, -1000,   50,  -950],
    [ 50, -1100,  150, -1050],


    [ 50, -1300,  150, -1300],


    [150, -1400,  250, -1400],
    [ 50, -1500,  150, -1500],
    [-50, -1600,   50, -1600],
    [ 50, -1700,  150, -1700],

    [ 50, -1900,  150, -1900],

    [  0, -2000,  200, -2000],
    [  0, -2200,  200, -2200],
    [ 50, -2300,  150, -2300],

    [ 50, -2400,  150, -2400],

    [ 75, -2500,  125, -2500],
    [ 75, -2700,  125, -2700],
    [ 50, -2800,  150, -2800],

    [ 50, -2900,  150, -2900],




    [ 50,-3100,  150, -3000],
    [150,-3100,  250, -3000],
    [150,-3200,  250, -3300],

    [100,-3200,  100, -3300],

    [-50,-3200,   50, -3300],
    [-50,-3500,   50, -3400],
    [ 50,-3500,  150, -3400],

    [ 50,-3600,  150, -3600]
]

},{}],6:[function(require,module,exports){

const MathUtils = require('../../utilities/MathUtils.js');

//

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
		this._nextCheckpointIndex = 0;
		this._healthInTicks = this._maxHealthInTicks;

		this._totalTicks = 0;
		this._trail = [];
	}

	update(delta, walls, artificialNeuralNetwork) {

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
		// update

		const speedMax = 15;
		const steerMax = Math.PI / 32.0;

		const input = [];

		for (let ii = 0; ii < this._sensors.length; ++ii)
			input.push(this._sensors[ii].result);

		const output = artificialNeuralNetwork.process( input );

		// this._angle += MathUtils.clamp(output[0], -1, 1) * steerMax * delta;
		// const speed = MathUtils.clamp(output[1], -1, 1) * speedMax;
		this._angle += (output[0] * 2 - 1) * steerMax * delta;
		const speed = (output[1] * 2 - 1) * speedMax;

		this._position.x += (speed * Math.cos(this._angle)) * delta;
		this._position.y += (speed * Math.sin(this._angle)) * delta;

		this._trail.push({ x: this._position.x, y: this._position.y });

		//

		this._collideWalls(walls);

		if (this._alive) {

			this._updateSensors();
			this._collideSensors(walls);
			this._collideCheckpoints();

			++this._totalTicks;
		}
	}

	_collideWalls(walls) {

		for (let ii = 0; ii < walls.length; ++ii)
			if (MathUtils.collisionSegmentCercle(walls[ii].p1, walls[ii].p2, this._position, 10.0)) {

				this._alive = false;
				break;
			}
	}

	_updateSensors() {

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

		if (this._nextCheckpointIndex < this._checkpoints.length) {

			const nextCheckpoint = this._checkpoints[this._nextCheckpointIndex];

			if (MathUtils.collisionSegmentCercle(nextCheckpoint.p1, nextCheckpoint.p2, this._position, 5.0)) {

				this._healthInTicks = this._maxHealthInTicks;
				++this._fitness;
				++this._nextCheckpointIndex;
			}
		}

		if (this._nextCheckpointIndex >= this._checkpoints.length) {

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

//

module.exports = Car;

},{"../../utilities/MathUtils.js":10}],7:[function(require,module,exports){

const MathUtils = require('../../utilities/MathUtils.js');

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

module.exports = Circuit;

},{"../../utilities/MathUtils.js":10}],8:[function(require,module,exports){

function steeperSigmoid(x)
{
	return 1.0 / (1.0 + Math.exp(-4.9 * x));
}


class ArtificialNeuralNetwork {

	constructor(topology) {

		const lastLayerIndex = topology.length - 1;

		//
		// extra check here

		if (topology.length < 2)
			throw new Error("received invalid number of layer");

		if (topology[0] <= 0) // input
			throw new Error("received invalid number of inputs");

		for (let ii = 1; ii < lastLayerIndex; ++ii) // exclude first & last -> input & output
			if (topology[ii] <= 0)
				throw new Error("received invalid number of hidden neurons");

		if (topology[lastLayerIndex] <= 0) // input
			throw new Error("received invalid number of outputs");

		// save the topology for later checks

		this._topology = topology;

		// compute totalWeights

		this._totalWeights = 0;

		let previousLayerNeuronNumber = topology[0]; // inputs

		for (let ii = 1; ii < lastLayerIndex; ++ii) { // exclude first & last -> input & output

			const currentLayerNeuronNumber = topology[ii];

			this._totalWeights += previousLayerNeuronNumber * currentLayerNeuronNumber;
			previousLayerNeuronNumber = currentLayerNeuronNumber;
		}
		this._totalWeights += previousLayerNeuronNumber * topology[lastLayerIndex];

		// build hidden layer(s)

		previousLayerNeuronNumber = topology[0]; // inputs

		this._hiddenLayers = [];
		for (let ii = 1; ii < lastLayerIndex; ++ii) { // exclude first & last -> input & output

			const currentLayerNeuronNumber = topology[ii]; // number of neuron on the hidden layer

			const layer = [];

			this._buildLayer(layer, previousLayerNeuronNumber, currentLayerNeuronNumber);

			this._hiddenLayers.push(layer);

			previousLayerNeuronNumber = currentLayerNeuronNumber; // save the size -> this layer feed the next one
		}

		// build output layer

		this._outputLayer = [];

		this._buildLayer(this._outputLayer, previousLayerNeuronNumber, topology[lastLayerIndex]);
	}

	_buildLayer(layer, inputsNumber, neuronsNumber) {

		for (let ii = 0; ii < neuronsNumber; ++ii) {

			const connections = [];

			for (let jj = 0; jj < inputsNumber; ++jj) {

				const randomValue = Math.random() - Math.random(); // [-1 < value < +1]

				connections.push(randomValue);
			}

			layer.push(connections);
		}
	}

	process(inputs) {

		if (inputs.length != this._topology[0])
			throw new Error(`invalid inputs, input=${inputs.length}, expected=${this._topology[0]}`);

		const output = [];

		//
		// process hidden layer

		let	currentInputs = inputs;

		for (let kk = 0; kk < this._hiddenLayers.length; ++kk) {

			const currentOutput = [];

			this._processLayer(this._hiddenLayers[kk], currentInputs, currentOutput);

			currentInputs = currentOutput;
		}

		//
		// process output layer

		this._processLayer(this._outputLayer, currentInputs, output);

		return output;
	}

	_processLayer(layer, inputs, output) {

		// Cycle over all the connections and sum their weights against the inputs.
		for (let ii = 0; ii < layer.length; ++ii) {

			const connections = layer[ii];

			let activation = 0.0;

			// Sum the weights to the activation value.
			for (let jj = 0; jj < inputs.length; ++jj)
				activation += inputs[jj] * connections[jj];

			// if (Number.isNaN(activation))
			// 	activation = 0;

			// output.push(activation);
			output.push(steeperSigmoid(activation));
		}
	}

	setWeights(arr_weights) {

		if (arr_weights.length != this._totalWeights)
			throw new Error(`invalid weights, input=${arr_weights.length}, expected=${this._totalWeights}`);

		let weights_inc = 0;

		for (let kk = 0; kk < this._hiddenLayers.length; ++kk) {

			const layer = this._hiddenLayers[kk];

			for (let ii = 0; ii < layer.length; ++ii) {

				const connections = layer[ii];

				for (let jj = 0; jj < connections.length; ++jj)
					connections[jj] = arr_weights[weights_inc++];
			}
		}

		for (let ii = 0; ii < this._outputLayer.length; ++ii) {

			const connections = this._outputLayer[ii];

			for (let jj = 0; jj < connections.length; ++jj)
				connections[jj] = arr_weights[weights_inc++];
		}
	}

	set weights(arr_weights) {
		this.setWeights(arr_weights);
	}

	getWeights() {

		const out_weights = [];

		for (let ii = 0; ii < this._hiddenLayers.length; ++ii) {

			const layer = this._hiddenLayers[ii];

			for (let jj = 0; jj < layer.length; ++jj) {

				const connections = layer[jj];

				for (let kk = 0; kk < connections.length; ++kk)
					out_weights.push(connections[kk]);
			}
		}

		for (let jj = 0; jj < this._outputLayer.length; ++jj) {

			const connections = this._outputLayer[jj];

			for (let kk = 0; kk < connections.length; ++kk)
				out_weights.push(connections[kk]);
		}

		return out_weights;
	}

	get weights() {
		return this.getWeights();
	}

	get totalWeights() { return this._totalWeights; }
}

// export default ArtificialNeuralNetwork;
module.exports = ArtificialNeuralNetwork;

},{}],9:[function(require,module,exports){

// const utils = require('../utils/index.js');
// const Logger = utils.Logger;

const ArtificialNeuralNetwork = require("./ArtificialNeuralNetwork.js");

class GeneticAlgorithm {

	constructor(genomeSize, ann_topology) {

		// this._logger = new Logger("logger");

		this._currentId = 0;

		this._ANNs = [];
		for (let ii = 0; ii < genomeSize; ++ii)
			this._ANNs.push(new ArtificialNeuralNetwork(ann_topology));

		this._genomes = [];
		for (let ii = 0; ii < genomeSize; ++ii)
			this._genomes.push({
				id: this._currentId++,
				fitness: 0.0,
				weights: this._ANNs[ii].weights
			});

		this._currentGeneration = 1;
		this._bestGenome = null;
	}

	breedPopulation() {

		const bestGenomes = this._genomes.slice(0).sort((genA, genB) => {
			return genB.fitness - genA.fitness;
		});

		const isSmarter = (!this._bestGenome || this._bestGenome.fitness < bestGenomes[0].fitness);
		if (isSmarter)
			this._bestGenome = bestGenomes[0];

		const children = [];

		{ // elitism

			const eliteGenome = {
				id: this._bestGenome.id,
				fitness: 0.0,
				weights: []
			};

			// deep copy
			for (let ii = 0; ii < this._bestGenome.weights.length; ++ii)
				eliteGenome.weights.push(this._bestGenome.weights[ii]);

			this._mutate(eliteGenome);

			children.push(eliteGenome);

		} // elitism

		{ // breed best genomes

			const parentsPairs = [];
			for (let ii = 0; ii < bestGenomes.length; ++ii)
				for (let jj = ii + 1; jj < bestGenomes.length; ++jj)
					parentsPairs.push([ ii, jj ]);

			// sort pair by summed fitness
			parentsPairs.sort((pairA, pairB) => {

				// the higher the better
				return (
					bestGenomes[pairB[0]].fitness + bestGenomes[pairB[1]].fitness -
					bestGenomes[pairA[0]].fitness + bestGenomes[pairA[1]].fitness
				);
			});

			const maxOffsprings = this._genomes.length * 0.9 - children.length;

			for (let ii = 0; ii < maxOffsprings; ++ii) {

				const parentA = bestGenomes[parentsPairs[ii][0]];
				const parentB = bestGenomes[parentsPairs[ii][1]];
				const offspring = this._reproduce(parentA, parentB);
				this._mutate(offspring);
				children.push(offspring);
			}

		} // breed best genomes

		{
			const remainingChildren = (this._genomes.length - children.length);

			const totalWeights = this._ANNs[0].totalWeights;

			for (let ii = 0; ii < remainingChildren; ++ii) {

				const genome = {
					id: this._currentId++,
					fitness: 0.0,
					weights: []
				};

				for (let jj = 0; jj < totalWeights; ++jj)
					genome.weights.push(Math.random() - Math.random());  // [-1..1]

				children.push(genome);
			}
		}

		this._genomes = children;
		++this._currentGeneration;

		for (let ii = 0; ii < this._genomes.length; ++ii)
			this._ANNs[ii].weights = this._genomes[ii].weights;

		return isSmarter;
	}

	_reproduce(parentA, parentB) {

		const totalWeights = this._ANNs[0].totalWeights;

		const offspring = {
			id: this._currentId++,
			fitness: 0.0,
			weights: []
		};

		for (let ii = 0; ii < totalWeights; ++ii) {

			if (parentA.fitness < parentB.fitness) {
				// 70/30 chances for the fittest parent
				if (Math.random() > 0.30)
					offspring.weights[ii] = parentB.weights[ii];
				else
					offspring.weights[ii] = parentA.weights[ii];
			}
			else if (parentB.fitness < parentA.fitness) {
				// 70/30 chances for the fittest parent
				if (Math.random() > 0.30)
					offspring.weights[ii] = parentA.weights[ii];
				else
					offspring.weights[ii] = parentB.weights[ii];
			}
			else {
				// 50/50 chances for both parents
				if (Math.random() < 0.5)
					offspring.weights[ii] = parentA.weights[ii];
				else
					offspring.weights[ii] = parentB.weights[ii];
			}
		}

		return offspring;
	}

	_mutate(genome) {

		const mutationChance = 0.1;
		const mutationEffect = 0.2;

		for (let ii = 0; ii < genome.weights.length; ++ii)
			if (Math.random() < mutationChance) {
				const randomValue = (Math.random() - Math.random()); // [-1..1]
				genome.weights[ii] += (randomValue * mutationEffect);
			}
	}

	get genomes() { return this._genomes; }
	get ANNs() { return this._ANNs; }

}

//

// export default GeneticAlgorithm;
module.exports = GeneticAlgorithm;

},{"./ArtificialNeuralNetwork.js":8}],10:[function(require,module,exports){

class MathUtils {

	static collisionDroiteSeg(A, B, O, P) {

		const	AB = { x: B.x - A.x, y: B.y - A.y };
		const	AP = { x: P.x - A.x, y: P.y - A.y };
		const	AO = { x: O.x - A.x, y: O.y - A.y };

		return ((AB.x * AP.y - AB.y * AP.x) * (AB.x * AO.y - AB.y * AO.x) < 0.0);
	}

	static collisionSegSeg_partial(A, B, O, P) {

		if (!MathUtils.collisionDroiteSeg(A,B,O,P))
			return 1.0;

		const AB = { x: B.x - A.x, y: B.y - A.y };
		const OP = { x: P.x - O.x, y: P.y - O.y };

		return -(A.x * OP.y - O.x * OP.y - OP.x * A.y + OP.x * O.y) / (AB.x * OP.y - AB.y * OP.x);
	}

	static collisionSegSeg(A, B, O, P) {

		const k = MathUtils.collisionSegSeg_partial(A, B, O, P);

		return (!(k < 0.0 || k > 1.0));
	}

	/////////////////////////////////////////////
	/////////////////////////////////////////////
	/////////////////////////////////////////////

	static collisionPointCercle(P, C, radius) {

		const d2 = (P.x - C.x) * (P.x - C.x) + (P.y - C.y) * (P.y - C.y);

		return (d2 <= radius * radius);
	}

	static collisionDroiteCercle_BBox(A, B, C, radius) {

		const minAB = { x: Math.min(A.x, B.x), y: Math.min(A.y, B.y) };
		const maxAB = { x: Math.max(A.x, B.x), y: Math.max(A.y, B.y) };
		const minC = { x: C.x - radius, y: C.y - radius };
		const maxC = { x: C.x + radius, y: C.y + radius };

		return !(
			maxAB.x < minC.x ||
			minAB.x > maxC.x ||
			maxAB.y < minC.y ||
			minAB.y > maxC.y
		);
	}

	static collisionDroiteCercle(A, B, C, radius) {

		if (!MathUtils.collisionDroiteCercle_BBox(A, B, C, radius))
			return false;

		const U = { x: B.x - A.x, y: B.y - A.y };
		const AC = { x: C.x - A.x, y: C.y - A.y };

		const vNorm = Math.abs(U.x * AC.y - U.y * AC.x);
		const uNorm = Math.sqrt(U.x * U.x + U.y * U.y);
		const CI = vNorm / uNorm;

		return (CI < radius);
	}

	static collisionSegmentCercle(A, B, C, radius) {

		if (!MathUtils.collisionDroiteCercle(A, B, C, radius))
			return false;

		const AB = { x: B.x - A.x, y: B.y - A.y };
		const AC = { x: C.x - A.x, y: C.y - A.y };
		const BC = { x: C.x - B.x, y: C.y - B.y };

		const scalProd1 = AB.x * AC.x + AB.y * AC.y; // scalar product
		const scalProd2 = (-AB.x) * BC.x + (-AB.y) * BC.y; // scalar product

		if (scalProd1 >= 0 && scalProd2 >= 0)
			return true; // <= I is between A and B

		// last possibility, A ou B inside the circle C
		return (
			MathUtils.collisionPointCercle(A, C, radius) ||
			MathUtils.collisionPointCercle(B, C, radius)
		);
	}

	static rotateVec2(point, center, angle) {

		const cos_a = Math.cos(angle);
		const sin_a = Math.sin(angle);

		return {
			x: center.x + (point.x - center.x) * cos_a + (point.y - center.y) * sin_a,
			y: center.y + (point.x - center.x) * sin_a - (point.y - center.y) * cos_a
		};
	}

	static lerp(p1, p2, ratio) {
		return {
			x: p1.x + (p2.x - p1.x) * ratio,
			y: p1.y + (p2.y - p1.y) * ratio
		};
	}

	static clamp(value, min, max) {
		return Math.max(min, Math.min(max, value));
	}
}

//

module.exports = MathUtils;

},{}],11:[function(require,module,exports){

require("fpsmeter");

const Experiment = require('./experiment/Experiment.js');

//

window.addEventListener("load", onGlobalLoad, false);
window.addEventListener("error", onGlobalError, false);

function onGlobalError(err) {

    alert(`error=${err.message}`);
}

function onGlobalLoad() {

    //
    // init

	const canvas = addCanvas(0, 0, 800, 600);
    const currentExperiment = new Experiment(canvas);

    //

    const fpsmeters = {
        main: addFpsMeter(0, 0),
        update: addFpsMeter(0, 50, true),
        render: addFpsMeter(0, 100, true)
    };

    //
    // main loop

    let isRunning = true;

    function handleGlobalError(err) {

        // this will stop the main loop when an error occur
        // -> depending of what's wrong, it eat up the cpu and freeze some browser(s)
        // -> it also help to debug as it won't run and fail again to log many time the same error
        isRunning = false;
    };

    window.addEventListener("error", handleGlobalError, false);

    function tick() {

        if (isRunning)
            requestAnimationFrame(tick);

            fpsmeters.main.tick();
            fpsmeters.main.tickStart();

        //

        fpsmeters.update.tickStart();

        currentExperiment.update();

        fpsmeters.update.tick();

        //

        fpsmeters.render.tickStart();

        currentExperiment.render();

        fpsmeters.render.tick();
    };

    tick();
}

function addFpsMeter(x, y, toggleIt) {

    const domElement = document.createElement("div");
    domElement.style.position = "absolute";
    domElement.style.left = `${x || 0}px`;
    domElement.style.top = `${y || 0}px`;

    document.body.appendChild(domElement);

    const theme = FPSMeter.theme.transparent;
    const newFpsMeter = new FPSMeter( domElement, theme );

    // switch from FPS to ms
    if (toggleIt)
        newFpsMeter.toggle();

    return newFpsMeter;
}

function addCanvas(x, y, width, height) {

    const canvas = document.createElement("canvas");
    canvas.style.position = "absolute";
    canvas.style.left = `${x || 0}px`;
    canvas.style.top = `${y || 0}px`;
    canvas.width = `${width || 800}`;
    canvas.height = `${height || 600}`;

    document.body.appendChild(canvas);

    return canvas; 
}

},{"./experiment/Experiment.js":2,"fpsmeter":1}]},{},[11]);
