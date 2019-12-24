
const vertexShaderSource = `#version 300 es

    precision mediump float;

    uniform mat4 uComposedMatrix;

    in vec2 aVertexPosition;
    in vec3 aVertexColor;

    out vec3 vColor;

    void main(void)
    {
        vColor = aVertexColor;

        gl_Position = uComposedMatrix * vec4(aVertexPosition, 0.0, 1.0);
    }
`;

const fragmentShaderSource = `#version 300 es

    precision mediump float;

    in vec3 vColor;

    out vec4 oFragColor;

    void main(void)
    {
        oFragColor = vec4(vColor, 1.0);
    }
`;

module.exports = {
    vertex: vertexShaderSource,
    fragment: fragmentShaderSource,
};
