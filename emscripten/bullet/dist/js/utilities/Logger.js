
class Logger {

  constructor(textAreaElementId) {

    this._textAreaElement = document.getElementById(textAreaElementId);

    if (!this._textAreaElement)
      throw new Error(`DOM elements not found, id=${textAreaElementId}`);

    this._lines = [];
    this._maxLines = 30;

    this._textAreaElement.value = ""; // <= clear any browser cache
  }

  log(...args) {

    if (args.length == 0)
      return;

    const text = Array.prototype.slice.call(args).join(' ');

    console.log(text);

    this._pushText(text);
  }

  error(...args) {

    if (args.length == 0)
      return;

    const text = Array.prototype.slice.call(args).join(' ');

    console.error(text);

    this._pushText(`[ERR] - ${text}`);
  }

  _pushText(text) {

    this._lines.push(text);
    if (this._lines.length > this._maxLines)
      this._lines.splice(0, this._lines.length - this._maxLines);

    this._textAreaElement.value = `${this._lines.join("\n")}\n`;

    // force focus on last line
    this._textAreaElement.scrollTop = this._textAreaElement.scrollHeight;
  }

  peekLast() {
    if (this._lines.length > 0)
      return this._lines[this._lines.length - 1];
    return undefined;
  }

  popLast() {
    if (this._lines.length > 0)
      this._lines.splice(this._lines.length - 1, 1);
  }
}

export default Logger;
