
class Logger {

  private _textAreaElement: HTMLTextAreaElement;
  private _lines: string[];
  private _maxLines: number;

  constructor(textAreaElementId: string) {

    const textAreaElement = document.querySelector<HTMLTextAreaElement>(textAreaElementId);

    if (!textAreaElement)
      throw new Error(`DOM elements not found, id=${textAreaElementId}`);

    this._textAreaElement = textAreaElement;
    this._textAreaElement.value = ""; // <= clear any browser cache
    this._lines = [];
    this._maxLines = 30;
  }

  log(...args: any[]): void {

    if (args.length == 0)
      return;

    const text = Array.prototype.slice.call(args).join(' ');

    console.log(text);

    this._pushText(text);
  }

  error(...args: any[]): void {

    if (args.length == 0)
      return;

    const text = Array.prototype.slice.call(args).join(' ');

    console.error(text);

    this._pushText(`[ERR] - ${text}`);
  }

  _pushText(text: string): void {

    this._lines.push(text);
    if (this._lines.length > this._maxLines)
      this._lines.splice(0, this._lines.length - this._maxLines);

    this._textAreaElement.value = `${this._lines.join("\n")}\n`;

    // force focus on last line
    this._textAreaElement.scrollTop = this._textAreaElement.scrollHeight;
  }

  get size() {
    return this._lines.length;
  }

  peekLast(): string | undefined {
    if (this._lines.length > 0)
      return this._lines[this._lines.length - 1];
    return undefined;
  }

  popLast(): void {
    if (this._lines.length > 0)
      this._lines.splice(this._lines.length - 1, 1);
  }
}

export default Logger;
