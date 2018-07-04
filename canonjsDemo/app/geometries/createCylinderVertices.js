
'use strict'

define(function() {

    function createCylinderVertices(radius, length, arr_color) {

        // var side_x = arr_size[0] / 2;
        // var side_y = arr_size[1] / 2;
        // var side_z = arr_size[2] / 2;

        var quality = 16;

        var vertices = [
            //  side_x,  side_y,  side_z, // 0
            // -side_x,  side_y,  side_z,
            //  side_x, -side_y,  side_z,
            // -side_x, -side_y,  side_z,

            //  side_x,  side_y, -side_z, // 4
            // -side_x,  side_y, -side_z,
            //  side_x, -side_y, -side_z,
            // -side_x, -side_y, -side_z
        ];

        for (var i = 0; i < quality; ++i)
        {
            var ratio = i / quality;
            vertices.push(radius * Math.cos(Math.PI * 2 * ratio), -length/2, radius * Math.sin(Math.PI * 2 * ratio));
        }

        for (var i = 0; i < quality; ++i)
        {
            var ratio = i / quality;
            vertices.push(radius * Math.cos(Math.PI * 2 * ratio), +length/2, radius * Math.sin(Math.PI * 2 * ratio));
        }

        vertices.push(0, -length/2, 0);
        vertices.push(0, +length/2, 0);

        //

        var indices = [
            // 0,1,  1,3,  3,2,  2,0,
            // 4,5,  5,7,  7,6,  6,4,
            // 0,4,  1,5,  3,7,  2,6
        ];

        // wheel side 1
        for (var i = 0; i < quality; ++i)
            indices.push(i, (i + 1) % quality);

        // wheel side 2
        for (var i = 0; i < quality; ++i)
            indices.push(quality+i, quality+(i + 1) % quality);

        // inter wheels
        // for (var i = 0; i < quality; ++i)
        //     indices.push(i, quality+i);
        indices.push(0, quality);

        //

        var fvertices = [];

        for (var i = 0; i < indices.length; ++i)
        {
            var curr_index = indices[i] * 3;

            for (var j = 0; j < 3; ++j)
                fvertices.push( vertices[curr_index + j] );

            for (var j = 0; j < 3; ++j)
                fvertices.push( arr_color[j] );
        }

        return fvertices;
    }

    return createCylinderVertices;
});