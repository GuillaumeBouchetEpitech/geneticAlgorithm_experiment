
const { gl } = require("./WebGLContext.js");

class Shader {

    // _program: WebGLProgram;
    // _attributesMap: Map<string, number>;
    // _uniformsMap: Map<string, WebGLUniformLocation>;

    constructor(opt) {

        const vertexShader = this._getShader(gl.VERTEX_SHADER, opt.vertexSource);
        const fragmentShader = this._getShader(gl.FRAGMENT_SHADER, opt.fragmentSource);

        this._program = gl.createProgram();
        gl.attachShader(this._program, vertexShader);
        gl.attachShader(this._program, fragmentShader);
        gl.linkProgram(this._program);

        gl.deleteShader(vertexShader);
        gl.deleteShader(fragmentShader);

        if (!gl.getProgramParameter(this._program, gl.LINK_STATUS)) {

            // An error occurred while linking
            const lastError = gl.getProgramInfoLog(this._program);

            throw new Error(`Failed to initialised shaders, Error linking: ${lastError}`);
        }

        this._attributesMap = new Map();
        this._uniformsMap = new Map();

        gl.useProgram(this._program);

        opt.attributes.forEach((name) => {
            const newLocation = gl.getAttribLocation(this._program, name)

            if (newLocation < 0)
                throw new Error(`unknown shader attribute, name=${name}`);

            this._attributesMap.set(name, newLocation);
        });
        opt.uniforms.forEach((name) => {

            const newLocation = gl.getUniformLocation(this._program, name);

            if (newLocation < 0)
                throw new Error(`unknown shader uniform, name=${name}`);

            this._uniformsMap.set(name, newLocation);
        });

        gl.useProgram(null);
    }

    bind() {
        gl.useProgram(this._program);
    }

    unbind() {
        gl.useProgram(null);
    }

    getAttributes(name) {
        const attr = this._attributesMap.get(name);
        if (attr === undefined)
            throw new Error(`shader attribute not found, name=${name}`);
        return attr;
    }

    getUniforms(name) {
        const uniform = this._uniformsMap.get(name);
        if (uniform === undefined)
            throw new Error(`shader uniform not found, name=${name}`);
        return uniform;
    }

    _getShader(type, source) {

        if (type != gl.VERTEX_SHADER &&
            type != gl.FRAGMENT_SHADER)
            throw new Error("wrong shader type");

        const shader = gl.createShader(type);

        if (!shader) {

            if (type == gl.VERTEX_SHADER)
                throw new Error("vertex shader creation error");
            else
                throw new Error("fragment shader creation error");
        }

        gl.shaderSource(shader, source);
        gl.compileShader(shader);

        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {

            const inforLog = gl.getShaderInfoLog(shader) || "unknown";

            if (type == gl.VERTEX_SHADER)
                throw new Error(`vertex shader compilation error, info=${inforLog}`);
            else
                throw new Error(`fragment shader compilation error, info=${inforLog}`);

        }

        return shader;
    }
}

module.exports = Shader;
