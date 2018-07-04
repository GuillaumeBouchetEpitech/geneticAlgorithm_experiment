
'use strict'

define(
    [
          '../gl-context.js'

        , 'physic/cannon.min' // in /lib

        , './world.js'

        , '../geometries/geometryColor.js'

        , '../geometries/geometryExperimental.js'
    ],
    function(
          gl

        , unused_CANNON // <- use CANNON

        , world

        , createGeometryColor

        , createGeometryExperimental
    )
{
    function createCircuit(arr_checkpoints)
    {
        //
        //
        // polygonise checkpoints

        this._arr_geoms = [];

        var indices = [
            0, 2, 1, // triangle 2
            1, 2, 3, // triangle 3
        ];

        var geom_vertices = [];
        var geom_vertices_wall = [];
        var geom_vertices_exp = [];
        var geom_vertices_Wexp = [];
        var geom_vertices_W2exp = [];


        function calculate_normal(v0, v1, v2)
        {
            var px = v1[0] - v0[0];
            var py = v1[1] - v0[1];
            var pz = v1[2] - v0[2];

            var qx = v2[0] - v0[0];
            var qy = v2[1] - v0[1];
            var qz = v2[2] - v0[2];

            var nx = (py * qz) - (pz * qy);
            var ny = (pz * qx) - (px * qz);
            var nz = (px * qy) - (py * qx);

            var normal = [nx, ny, nz];
            var tmp_len = Math.sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
            normal[0] /= tmp_len;
            normal[1] /= tmp_len;
            normal[2] /= tmp_len;

            return normal;

        } // function calculate_normal(v0, v1, v2)


        var prev_normal = null;

        var geometry_step = 6;

        for (var index = 1; index < arr_checkpoints.length; index += geometry_step)
        {
            var vertices = [];
            var vertices_wall1 = [];
            var vertices_wall2 = [];
            var colors = [];
            var normals = [];

            //
            // repeatable

            var indices = [
                0, 2, 1, // triangle 2
                1, 2, 3, // triangle 3
            ];

            for (var index2 = 1; index2 < geometry_step; ++index2)
            {
                var step = index2 * 4;
                indices.push( indices[0]+step, indices[1]+step, indices[2]+step );
                indices.push( indices[3]+step, indices[4]+step, indices[5]+step );
            }

            for (var index5 = index; index5 < arr_checkpoints.length && index5 < index+geometry_step; ++index5)
            {
                // console.log(index5);
                var c1 = arr_checkpoints[index5-1];
                var c2 = arr_checkpoints[index5];

                var p11 = [ c1[0], c1[1], c1[2] ];
                var p12 = [ c1[3], c1[4], c1[5] ];
                var p21 = [ c2[0], c2[1], c2[2] ];
                var p22 = [ c2[3], c2[4], c2[5] ];

                var color1 = [ c1[6], c1[7], c1[8] ];
                var color2 = [ c2[6], c2[7], c2[8] ];

                //
                // calculate the floor normal

                var normal = calculate_normal(p12, p21, p22);

                normal[0] *= 10;
                normal[1] *= 10;
                normal[2] *= 10;

                if (!prev_normal)
                    prev_normal = normal;

                //
                // floor

                vertices.push( p11[0], p11[1], p11[2] );
                vertices.push( p12[0], p12[1], p12[2] );
                vertices.push( p21[0], p21[1], p21[2] );
                vertices.push( p22[0], p22[1], p22[2] );

                colors.push(color1[0], color1[1], color1[2]);
                colors.push(color1[0], color1[1], color1[2]);
                colors.push(color2[0], color2[1], color2[2]);
                colors.push(color2[0], color2[1], color2[2]);

                //
                // walls

                vertices_wall1.push( p11[0], p11[1], p11[2] );
                vertices_wall1.push( p11[0]+prev_normal[0], p11[1]+prev_normal[1], p11[2]+prev_normal[2] );
                vertices_wall1.push( p21[0], p21[1], p21[2] );
                vertices_wall1.push( p21[0]+normal[0], p21[1]+normal[1], p21[2]+normal[2] );

                vertices_wall2.push( p12[0], p12[1], p12[2] );
                vertices_wall2.push( p12[0]+prev_normal[0], p12[1]+prev_normal[1], p12[2]+prev_normal[2] );
                vertices_wall2.push( p22[0], p22[1], p22[2] );
                vertices_wall2.push( p22[0]+normal[0], p22[1]+normal[1], p22[2]+normal[2] );

                //

                normals.push(prev_normal[0], prev_normal[1], prev_normal[2]);
                normals.push(normal[0], normal[1], normal[2]);
                normals.push(prev_normal[0], prev_normal[1], prev_normal[2]);
                normals.push(normal[0], normal[1], normal[2]);

                //

                prev_normal = normal;
            }

            // repeatable
            //

            //

            var planeShape = new CANNON.Trimesh(vertices, indices);
            var planeBody = new CANNON.Body({mass:0});
            planeBody.addShape(planeShape);
            planeBody.collisionFilterGroup = -1;
            planeBody.collisionFilterMask = world._GROUP_sensor; // <- sensors will collide the floor
            world.addBody(planeBody);

            var planeShape = new CANNON.Trimesh(vertices_wall1, indices);
            var planeBody = new CANNON.Body({mass:0});
            planeBody.addShape(planeShape);
            planeBody.collisionFilterGroup = world._GROUP_wall;
            planeBody.collisionFilterMask = world._GROUP_sensor;
            world.addBody(planeBody);

            var planeShape = new CANNON.Trimesh(vertices_wall2, indices);
            var planeBody = new CANNON.Body({mass:0});
            planeBody.addShape(planeShape);
            planeBody.collisionFilterGroup = world._GROUP_wall;
            planeBody.collisionFilterMask = world._GROUP_sensor;
            world.addBody(planeBody);

            //
            //

            for (var index2 in indices)
            {
                var vertexi = indices[index2] * 3;

                if (vertexi == 0 || vertexi == 3)
                    geom_vertices.push( vertices[vertexi+0], vertices[vertexi+1], vertices[vertexi+2], 1,1,1 );
                else
                    geom_vertices.push( vertices[vertexi+0], vertices[vertexi+1], vertices[vertexi+2], colors[0],colors[1],colors[2] );
            }

            for (var index2 in indices)
            {
                var vertexi = indices[index2] * 3;

                geom_vertices_wall.push( vertices_wall1[vertexi+0], vertices_wall1[vertexi+1], vertices_wall1[vertexi+2], 0,0.5,0.5 );
            }

            for (var index2 in indices)
            {
                var vertexi = indices[index2] * 3;

                geom_vertices_wall.push( vertices_wall2[vertexi+0], vertices_wall2[vertexi+1], vertices_wall2[vertexi+2], 0,0.5,0.5 );
            }

            var curr_len = geom_vertices_exp.length / 10 / indices.length;

            for (var index2 in indices)
            {
                curr_len += 1.0 / indices.length

                var vertexi = indices[index2] * 3;

                if (vertexi == 0 || vertexi == 3)
                    geom_vertices_exp.push( vertices[vertexi+0], vertices[vertexi+1], vertices[vertexi+2], 1,1,1, normals[vertexi+0], normals[vertexi+1], normals[vertexi+2], curr_len );
                else
                    geom_vertices_exp.push( vertices[vertexi+0], vertices[vertexi+1], vertices[vertexi+2], colors[0],colors[1],colors[2], normals[vertexi+0], normals[vertexi+1], normals[vertexi+2], curr_len );

                geom_vertices_Wexp.push( vertices_wall1[vertexi+0], vertices_wall1[vertexi+1], vertices_wall1[vertexi+2], 0,0.5,0.5, normals[vertexi+0], normals[vertexi+2], normals[vertexi+1], curr_len );
                geom_vertices_W2exp.push( vertices_wall2[vertexi+0], vertices_wall2[vertexi+1], vertices_wall2[vertexi+2], 0,0.5,0.5, -normals[vertexi+0], -normals[vertexi+2], -normals[vertexi+1], curr_len );
            }

        } // for (var index = 1; index < arr_checkpoints.length; index += 2)

        var geom_cube2 = new createGeometryColor(geom_vertices, gl.TRIANGLES);
        this._arr_geoms.push(geom_cube2);

        var geom_cube2 = new createGeometryColor(geom_vertices_wall, gl.TRIANGLES);
        this._arr_geoms.push(geom_cube2);

        this._geom_exp = new createGeometryExperimental(geom_vertices_exp, gl.TRIANGLES);
        this._geom_Wexp = new createGeometryExperimental(geom_vertices_Wexp, gl.TRIANGLES);
        this._geom_W2exp = new createGeometryExperimental(geom_vertices_W2exp, gl.TRIANGLES);

    }

    //
    //

    createCircuit.prototype.render = function(shader_color)
    {
        gl.uniform1f(shader_color.uColorApha, 1.0);

        this._arr_geoms[0].render(shader_color);

        gl.uniform1f(shader_color.uColorApha, 0.2);

        this._arr_geoms[1].render(shader_color);

        gl.uniform1f(shader_color.uColorApha, 1.0);
    }

    //
    //

    createCircuit.prototype.render_exp = function(shader_exp)
    {
        gl.uniform1f(shader_exp.uColorApha, 1.0);

        this._geom_exp.render(shader_exp);

        gl.uniform1f(shader_exp.uColorApha, 0.2);

        this._geom_Wexp.render(shader_exp);
        this._geom_W2exp.render(shader_exp);

        gl.uniform1f(shader_exp.uColorApha, 1.0);
    }

    return createCircuit;
});
