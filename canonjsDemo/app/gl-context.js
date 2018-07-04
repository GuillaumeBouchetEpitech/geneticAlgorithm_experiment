
'use strict'

define(
    [
        'webgl/WebGLUtils'
    ], function(
        WebGLUtils
    )
{

    // WEBGL

    var canvas = document.getElementById("main-canvas");

    var gl = WebGLUtils.setupWebGL(canvas);

    gl.viewportWidth = canvas.clientWidth;
    gl.viewportHeight = canvas.clientHeight;

    // CANVAS 2D

    var canvas_hud = document.getElementById("second-canvas");
    var ctx_hud = canvas_hud.getContext("2d");

    gl.hud = ctx_hud;

    // EXTENSIONS

    if (gl.getExtension) {

        gl._extension_vao =
            gl.getExtension('OES_vertex_array_object') ||
            gl.getExtension('MOZ_OES_vertex_array_object') ||
            gl.getExtension('WEBKIT_OES_vertex_array_object');
    }

    //

    return gl;
});