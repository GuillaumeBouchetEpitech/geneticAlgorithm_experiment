
class BasicGeometry {

    constructor(gl, basicShader, vertices) {

        this._verticesBuffer = gl.createBuffer();
        this._vertexElements = 5;
        this._verticesLength = vertices.length / this._vertexElements;

        {
            this._vao = gl.createVertexArray();
            gl.bindVertexArray(this._vao);

            gl.bindBuffer(gl.ARRAY_BUFFER, this._verticesBuffer);
            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

            gl.enableVertexAttribArray(basicShader.attributes.aVertexPosition);
            gl.enableVertexAttribArray(basicShader.attributes.aVertexColor);

            const bpp = 4; // gl.FLOAT -> 4 bytes
            const stride = this._vertexElements * bpp;
            const positionAddress   = 0 * bpp;
            const colorAddress      = 2 * bpp;

            gl.vertexAttribPointer(basicShader.attributes.aVertexPosition, 2, gl.FLOAT, false, stride, positionAddress);
            gl.vertexAttribPointer(basicShader.attributes.aVertexColor, 3, gl.FLOAT, false, stride, colorAddress);

            gl.bindVertexArray(null);
        }
    }

    render(gl) {

        gl.bindBuffer(gl.ARRAY_BUFFER, this._verticesBuffer);
        gl.bindVertexArray(this._vao);

        gl.drawArrays(gl.TRIANGLES, 0, this._verticesLength);
    }
}

module.exports = BasicGeometry;
