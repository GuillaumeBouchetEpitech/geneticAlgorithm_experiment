
define(function (require) {

    return {
        createShaders: createShaders
    }

    // function createShaders(gl, vs_id, fs_id) {
    function createShaders(gl, opt) {

        if (!opt.vs_id)
            throw new Error('no vertex shader id');

        if (!opt.fs_id)
            throw new Error('no fragment shader id');

        //

        var vertexShader = getShader(gl, opt.vs_id);

        if (!vertexShader)
            throw new Error("Could not initialise vertexShader");

        //

        var fragmentShader = getShader(gl, opt.fs_id);

        if (!fragmentShader)
            throw new Error("Could not initialise fragmentShader");

        //

        this.shaderProgram = gl.createProgram();
        gl.attachShader(this.shaderProgram, vertexShader);
        gl.attachShader(this.shaderProgram, fragmentShader);
        gl.linkProgram(this.shaderProgram);

        if (!gl.getProgramParameter(this.shaderProgram, gl.LINK_STATUS))
        {
            // An error occurred while linking
            var lastError = gl.getProgramInfoLog(this.shaderProgram);

            throw new Error("Failed to initialised shaders, Error linking:" + lastError);
        }


        // arr_attrib: ['aVertexPosition','aVertexPosition','aVertexPosition'],
        // arr_uniform: ['uMVMatrix','uPMatrix']

        getAttribAndLocation(gl, this.shaderProgram, opt.arr_attrib, opt.arr_uniform);

        return this.shaderProgram;

        //

        function getAttribAndLocation(gl, shader, arr_attrib, arr_uniform) {

            gl.useProgram(shader);

            if (arr_attrib)
                for (var i = 0; i < arr_attrib.length; ++i)
                    shader[arr_attrib[i]] = gl.getAttribLocation(shader, arr_attrib[i]);

            if (arr_uniform)
                for (var i = 0; i < arr_uniform.length; ++i)
                    shader[arr_uniform[i]] = gl.getUniformLocation(shader, arr_uniform[i]);

            gl.useProgram(null);
        }

        //

        function getShader(gl, id) {

            var shaderScript = document.getElementById(id);

            if (!shaderScript) {
                throw new Error('unknown element id');
                return null;
            }

            var str = "";
            var k = shaderScript.firstChild;
            while (k) {
                if (k.nodeType == 3)
                    str += k.textContent;

                k = k.nextSibling;
            }

            var shader;
            if (shaderScript.type == "x-shader/x-fragment")
                shader = gl.createShader(gl.FRAGMENT_SHADER);
            else if (shaderScript.type == "x-shader/x-vertex")
                shader = gl.createShader(gl.VERTEX_SHADER);
            else {
                throw new Error('unknown shader type');
                return null;
            }

            gl.shaderSource(shader, str);
            gl.compileShader(shader);

            if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
                throw new Error(gl.getShaderInfoLog(shader));
                return null;
            }

            return shader;
        }

    }


});