
const glm = require("gl-matrix");

const initialiseWebGLContext = require("./wrapper/initialiseWebGLContext.js");
const ShaderBuilder = require("./wrapper/ShaderBuilder.js");
const BasicGeometry = require("./wrapper/BasicGeometry.js");
const generateVertices = require("./wrapper/generateVertices.js");

const shaderSources = require("./shaderSources/basic.js");

class Renderer {

    constructor(canvas, width, height) {

        this._canvas = canvas;
        this._width = width;
        this._height = height;

        window.gl = initialiseWebGLContext(this._canvas);

        gl.clearColor(0.0, 0.0, 1.0, 1.0);

        // gl.clearDepth(1.0);
        // gl.enable(gl.DEPTH_TEST);
        // gl.depthFunc(gl.LESS);
        gl.disable(gl.DEPTH_TEST);

        // gl.disable(gl.BLEND);

        // gl.disable(gl.CULL_FACE);

        this._basicShader = ShaderBuilder.build(gl, {
            sources: {
                vertex: shaderSources.vertex,
                fragment: shaderSources.fragment
            },
            attributes: [ "aVertexPosition", "aVertexColor" ],
            uniforms: [ "uComposedMatrix" ],
        });

        const squareSize = 2;
        const hSquareSize = squareSize / 2;

        const hwidth = this._width / 40;
        const hheight = this._height / 40;

        this._matrices = {
            model: glm.mat4.create(),
            view: glm.mat4.create(),
            projection: glm.mat4.create(),
            composed: glm.mat4.create()
        };

        glm.mat4.ortho(this._matrices.projection, -hwidth,hwidth, -hheight,hheight, -1000,1000);

        // glm.mat4.lookAt(this._matrices.view, [1,1,1], [0,0,0], [0,0,1]);
        glm.mat4.lookAt(this._matrices.view, [0,0,1], [0,0,0], [0,1,0]);



        const vertices = {
            rectangle: generateVertices.createRectangleVertices(hSquareSize, hSquareSize, [1, 1, 1]),
            circle: generateVertices.createCircleVertices(hSquareSize, 16, [1, 0, 1]),
            smallCircle: generateVertices.createCircleVertices(hSquareSize * 0.5, 16, [1, 0, 1]),
            largeRectangle: generateVertices.createRectangleVertices(hSquareSize * 15, hSquareSize, [1, 1, 1]),
            smallRectangle: generateVertices.createRectangleVertices(hSquareSize * 0.5, hSquareSize * 0.5, [1, 1, 1]),
        };

        this._geometries = {
            rectangle: new BasicGeometry(gl, this._basicShader, vertices.rectangle),
            smallRectangle: new BasicGeometry(gl, this._basicShader, vertices.smallRectangle),
            largeRectangle: new BasicGeometry(gl, this._basicShader, vertices.largeRectangle),
            circle: new BasicGeometry(gl, this._basicShader, vertices.circle),
            smallCircle: new BasicGeometry(gl, this._basicShader, vertices.smallCircle),
        };
    }

    resize(width, height) {

        this._canvas.width = this._width = width;
        this._canvas.height = this._height = height;

        gl.viewport(0, 0, width, height);

        const hwidth = width / 40;
        const hheight = height / 40;

        glm.mat4.ortho(this._matrices.projection, -hwidth,hwidth, -hheight,hheight, -1000,1000);
    }

    get geometries() {
        return this._geometries;
    }

    lookAt(x, y) {
        const eye = [x, y, 100];
        const target = [x, y, 0];
        const up = [0, 1, 0];
        glm.mat4.lookAt(this._matrices.view, eye, target, up);
    }

    render(scene) {

        // gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        gl.clear(gl.COLOR_BUFFER_BIT);

        gl.useProgram(this._basicShader.program);

        const rawPos = {};
        const rawQuat = {};

        for (let ii = 0; ii < scene.length; ++ii) {

            const sceneElement = scene[ii];

            sceneElement.physicBody.getPositionAndRotation(rawPos, rawQuat);
            const pos = [rawPos.x, rawPos.y, 0];
            const quat = [0, 0, rawQuat.z, rawQuat.w];
            glm.mat4.fromRotationTranslation(this._matrices.model, quat, pos);

            glm.mat4.multiply(this._matrices.model, this._matrices.view, this._matrices.model);
            glm.mat4.multiply(this._matrices.composed, this._matrices.projection, this._matrices.model);

            gl.uniformMatrix4fv(this._basicShader.uniforms.uComposedMatrix, false, this._matrices.composed);

            sceneElement.geometry.render(gl);
        }
    }
}

module.exports = Renderer;
