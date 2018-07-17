
function loadScript(src) {
	return new Promise((resolve, reject) => {
		const scriptElement = document.createElement('script');
		scriptElement.src = src;
		scriptElement.onload = resolve;
		scriptElement.onerror = reject;
		document.head.appendChild(scriptElement);
	});
}

window.onload = () => {

	console.log("page loaded");

	const statusElement = document.getElementById('status');
	const progressElement = document.getElementById('progress');
	const spinnerElement = document.getElementById('spinner');
	const myFpsmeterDomElementUpdate = document.getElementById('fpsmeterUpdate');
	const myFpsmeterDomElemendRender = document.getElementById('fpsmeterRender');

	const myFpsmeterUpdate = new window.FPSMeter(
		myFpsmeterDomElementUpdate,
		window.FPSMeter.theme.transparent
	);
	const myFpsmeterRender = new window.FPSMeter(
		myFpsmeterDomElemendRender,
		window.FPSMeter.theme.transparent
	);
	myFpsmeterUpdate.toggle();
	myFpsmeterRender.toggle();

	const Module = {
		preRun: [],
		postRun: [],
		locateFile: (url) => `asm.js/${url}`,
		print: (function() {
			const element = document.getElementById('output');
			if (element)
				element.value = ''; // clear browser cache
			return function(text) {
				if (arguments.length > 1)
					text = Array.prototype.slice.call(arguments).join(' ');
				console.log(text);
				if (element) {
					element.value += text + "\n";
					element.scrollTop = element.scrollHeight; // focus on bottom
				}
			};
		})(),
		printErr: function(text) {
			if (arguments.length > 1)
				text = Array.prototype.slice.call(arguments).join(' ');
			console.error(text);
		},
		canvas: (function() {
			const canvas = document.getElementById('canvas');

			function onWebGlContextLost(event) {

				event.preventDefault();

				const text = 'WebGL context lost. You will need to reload the page.';
				alert(text);
				console.error(text);
			}

			canvas.addEventListener("webglcontextlost", onWebGlContextLost, false);

			return canvas;
		})(),
		setStatus: function(text) {

			if (!Module.setStatus.last)
				Module.setStatus.last = { time: Date.now(), text: '' };

			if (text === Module.setStatus.text)
				return;

			var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
			var now = Date.now();

			if (m && now - Date.now() < 30)
				return; // if this is a progress update, skip it if too soon

			if (m) {
				text = m[1];
				progressElement.value = parseInt(m[2]) * 100;
				progressElement.max = parseInt(m[4]) * 100;
				progressElement.hidden = false;
				spinnerElement.hidden = false;
			} else {
				progressElement.value = null;
				progressElement.max = null;
				progressElement.hidden = true;
				if (!text)
					spinnerElement.style.display = 'none';
			}
			statusElement.innerHTML = text;
		},
		totalDependencies: 0,
		monitorRunDependencies: function(left) {
			this.totalDependencies = Math.max(this.totalDependencies, left);
			Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
		}
	};

	Module.setStatus('Downloading...');

	window.Module = Module;
	window.myFpsmeterUpdate = myFpsmeterUpdate;
	window.myFpsmeterRender = myFpsmeterRender;

	window.onerror = function(event) {
		Module.setStatus('Exception thrown, see JavaScript console');
		spinnerElement.style.display = 'none';
		Module.setStatus = function(text) {
			if (text)
				Module.printErr('[post-exception status] ' + text);
		};
	};

	loadScript("./asm.js/index.js")
		.then(() => {
			console.log("script loaded");
		})
		.catch((err) => {
			console.error("script loading error:", err);
		});
};
