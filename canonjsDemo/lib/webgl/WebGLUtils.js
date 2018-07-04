

define(function () {


    /**
     * Provides requestAnimationFrame in a cross browser way.
     */
    window.requestAnimFrame = (function() {

        return  window.requestAnimationFrame ||
                window.webkitRequestAnimationFrame ||
                window.mozRequestAnimationFrame ||
                window.oRequestAnimationFrame ||
                window.msRequestAnimationFrame ||
                function(/* function FrameRequestCallback */ callback, /* DOMElement Element */ element) {
                    window.setTimeout(callback, 1000/60);
                };

    })();


    /**
     * Creates a webgl context. If creation fails it will
     * change the contents of the container of the <canvas>
     * tag to an error message with the correct links for WebGL.
     * @param {Element} canvas. The canvas element to create a
     *     context from.
     * @param {WebGLContextCreationAttirbutes} opt_attribs Any
     *     creation attributes you want to pass in.
     * @param {function:(msg)} opt_onError An function to call
     *     if there is an error during creation.
     * @return {WebGLRenderingContext} The created context.
     */
    var setupWebGL = function(canvas, opt_attribs, opt_onError) {

        if (canvas.addEventListener) {
            canvas.addEventListener("webglcontextcreationerror", function(e) {
                alert(e);
            }, false);
        }

        var context = create3DContext(canvas, opt_attribs);
        if (!context) {
            if (!window.WebGLRenderingContext) {
                alert("!window.WebGLRenderingContext");
            }
        }

        return context;
    };

    /**
     * Creates a webgl context.
     * @param {!Canvas} canvas The canvas tag to get context
     *     from. If one is not passed in one will be created.
     * @return {!WebGLContext} The created context.
     */
    var create3DContext = function(canvas, opt_attribs) {
        var names = ["webgl", "experimental-webgl", "webkit-3d", "moz-webgl"];
        var context = null;
        for (var ii = 0; ii < names.length; ++ii) {

            try {
                context = canvas.getContext(names[ii], opt_attribs);
            } catch(e) {}

            if (context)
                break;
        }
        return context;
    }



    return {
        create3DContext: create3DContext,
        setupWebGL: setupWebGL
    };

});

