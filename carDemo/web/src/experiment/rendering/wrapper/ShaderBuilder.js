
class ShaderBuilder {

    // const shader = ShaderBuilder.build(gl, {
    //     sources: {
    //         vertex: "[...]",
    //         fragment: "[...]"
    //     },
    //     attributes: [ "[...]", "[...]" ],
    //     uniforms: [ "[...]", "[...]" ],
    // });

    static build(gl, def) {

        const vertexShader = this._getShader(gl, gl.VERTEX_SHADER, def.sources.vertex);
        const fragmentShader = this._getShader(gl, gl.FRAGMENT_SHADER, def.sources.fragment);

        const program = gl.createProgram();
        gl.attachShader(program, vertexShader);
        gl.attachShader(program, fragmentShader);
        gl.linkProgram(program);

        gl.deleteShader(vertexShader);
        gl.deleteShader(fragmentShader);

        gl.useProgram(program);

        const shaderData = { program };

        if (def.attributes && def.attributes.length > 0) {
            shaderData.attributes = {};
            def.attributes.forEach(name => {
                shaderData.attributes[name] = gl.getAttribLocation(program, name);
            });
        }

        if (def.uniforms && def.uniforms.length > 0) {
            shaderData.uniforms = {};
            def.uniforms.forEach(name => {
                shaderData.uniforms[name] = gl.getUniformLocation(program, name);
            });
        }

        return shaderData;
    }
    
    static _getShader(gl, type, sourceCode) {

        const shader = gl.createShader(type);
        gl.shaderSource(shader, sourceCode);

        gl.compileShader(shader);

        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            const shaderInfo = gl.getShaderInfoLog(shader);
            throw new Error(`Shader failed to compile: ${shaderInfo}`);
        }

        return shader;
    }
};

module.exports = ShaderBuilder;
