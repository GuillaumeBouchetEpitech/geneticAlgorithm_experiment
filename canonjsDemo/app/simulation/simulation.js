
'use strict'

define(
    [
          '../gl-context.js'

        , './world.js'
        , './circuit.js'
        , './car.js'
        , '../ai/geneticAlgo.js'

        , 'utils/BSpline' // in /lib

        , '../geometries/geometryColor.js'

        // , '../ai/ArtificialIntelligence.js'
    ],
    function(
          gl

        , world
        , createCircuit
        , createCar
        , createGeneticAlgo

        , BSpline

        , createGeometryColor

        // , createArtificialIntelligence
    )
{

    //

    var createSimulation = function ()
    {
        var arr_checkpoints = [];




        { // building the circuit

            var points = [];
            var points2 = [];
            var colors = [];

            //
            // circuit skelton

                points.push([0,0,0]); points2.push([0,10,0]); colors.push([0,0,1]);
                points.push([90,0,0]); points2.push([90,0,0]); colors.push([0,0,1]);


                points.push([90,0,0]); points2.push([90,0,0]); colors.push([1,0,0]);

                { // turned looping

                    var color = [0,0.5,0]

                    points.push([40,0,40]); points2.push([40,0,40]); colors.push(color);
                    points.push([-40,0,40]); points2.push([-40,0,40]); colors.push(color);

                    points.push([-40,40,-10]); points2.push([-40,30,0]); colors.push(color);
                    points.push([20,40,0]); points2.push([0,40,-10]); colors.push(color);

                    points.push([20,20,0]); points2.push([35,30,0]); colors.push(color);
                    points.push([40,0,0]); points2.push([45,0,0]); colors.push(color);

                } // turned looping



                points.push([30,0,0]); points2.push([30,0,0]); colors.push([1,0,0]);


                { // looping

                    var color = [0,0,1]

                    var r = 60;
                    var d = -20;

                    points.push([r,0,0]); points2.push([r,0,0]); colors.push(color);
                    points.push([r,0,r]); points2.push([r,0,r]); colors.push(color);
                    points.push([-r,d,r]); points2.push([-r,d,r]); colors.push(color);
                    points.push([-r,d,-r]); points2.push([-r,d,-r]); colors.push(color);
                    points.push([r,d,-r]); points2.push([r,d,-r]); colors.push(color);
                    points.push([r,0,0]); points2.push([r,0,0]); colors.push(color);

                    points.push([20,0,0]); points2.push([20,0,0]); colors.push(color);

                } // looping


                points.push([30,0,0]); points2.push([30,0,0]); colors.push([1,0,0])


                { // infinity loop

                    var color = [0,0.5,0];

                    var pente = 7;

                    var height = 6;
                    var inner_radius = 50;
                    var outer_radius = 60;


                    points.push([30,0,+pente]); points2.push([30,0,0]); colors.push(color)

                    for (var i = 0; i < 2; ++i)
                    {
                        points.push([outer_radius,outer_radius,height]); points2.push([inner_radius,inner_radius,height]); colors.push(color)
                        points.push([-outer_radius,outer_radius,height]); points2.push([-inner_radius,inner_radius,height]); colors.push(color)

                        points.push([-outer_radius,-outer_radius,height]); points2.push([-inner_radius,-inner_radius,height]); colors.push(color)
                        points.push([outer_radius,-outer_radius,height]); points2.push([inner_radius,-inner_radius,height]); colors.push(color)
                    }

                    points.push([40,0,0]); points2.push([40,0,+pente*2]); colors.push([1,0,0])
 
                    for (var i = 0; i < 2; ++i)
                    {
                        points.push([inner_radius,-inner_radius,height]); points2.push([outer_radius,-outer_radius,height]); colors.push(color)
                        points.push([-inner_radius,-inner_radius,height]); points2.push([-outer_radius,-outer_radius,height]); colors.push(color)

                        points.push([-inner_radius,inner_radius,height]); points2.push([-outer_radius,outer_radius,height]); colors.push(color)
                        points.push([inner_radius,inner_radius,height]); points2.push([outer_radius,outer_radius,height]); colors.push(color)
                    }

                    points.push([30,0,0]); points2.push([30,0,-pente]); colors.push(color)

                } // infinity loop


                points.push([30,0,0]); points2.push([30,0,0]); colors.push([1,0,0])


                { // vertical corner

                    points.push([30,0,0]); points2.push([30,0,0]); colors.push([1,0,1])

                    points.push([30,0,30]); points2.push([30,0,30]); colors.push([1,0,1])

                    points.push([10,0,40]); points2.push([10,0,40]); colors.push([1,0,1])

                    points.push([10,30,30]); points2.push([10,20,15]); colors.push([1,0,1])
                    points.push([-10,30,-30]); points2.push([-10,20,-15]); colors.push([1,0,1])

                    points.push([-10,0,-40]); points2.push([-10,0,-40]); colors.push([1,0,1])

                    points.push([-30,0,-30]); points2.push([-30,0,-30]); colors.push([1,0,1])

                    points.push([-30,0,0]); points2.push([-30,0,0]); colors.push([1,1,1])

                } // vertical corner

                points.push([-30,0,0]); points2.push([-30,0,0]); colors.push([1,0,0])

            // circuit skelton
            //


            //
            // concatenate skelton coordinates

            var points_ex = [];
            var points2_ex = [];

            for (var index = 0; index < points.length; ++index)
            {
                var curr1 = points[index];
                var curr2 = points2[index];

                if (index > 0)
                {
                    // concatenate the point

                    var prev1 = points_ex[index-1];
                    var prev2 = points2_ex[index-1];

                    for (var prop in curr1)
                    {
                        curr1[prop] += prev1[prop];
                        curr2[prop] += prev2[prop];
                    }
                }

                points_ex.push(curr1);
                points2_ex.push(curr2);
            }

            // concatenate skelton coordinates
            //

            //

            { // geom (skelton)

                var geom_vertices = [];

                function lerp(val1, val2, ratio)
                {
                    return val1 + (val2 - val1) * ratio;
                }

                for (var index = 0; index < points_ex.length; ++index)
                {
                    var vertex1 = points_ex[index];
                    var vertex2 = points2_ex[index];

                    var v1 = [ lerp(vertex1[0], vertex2[0], -0.5), lerp(vertex1[1], vertex2[1], -0.5), lerp(vertex1[2], vertex2[2], -0.5) ];
                    var v2 = [ lerp(vertex1[0], vertex2[0], +1.5), lerp(vertex1[1], vertex2[1], +1.5), lerp(vertex1[2], vertex2[2], +1.5) ];
                    geom_vertices.push(v1[0], v1[1], v1[2], 1,1,1);
                    geom_vertices.push(v2[0], v2[1], v2[2], 1,1,1);

                    if (index > 0)
                    {
                        geom_vertices.push(points_ex[index-1][0], points_ex[index-1][1], points_ex[index-1][2], 1,1,1);
                        geom_vertices.push(points_ex[index][0], points_ex[index][1], points_ex[index][2], 1,1,1);

                        geom_vertices.push(points2_ex[index-1][0], points2_ex[index-1][1], points2_ex[index-1][2], 1,1,1);
                        geom_vertices.push(points2_ex[index][0], points2_ex[index][1], points2_ex[index][2], 1,1,1);
                    }
                }

                this._geom_skelton = new createGeometryColor(geom_vertices, gl.LINES);

            } // geom (skelton)

            //

            //
            // smooth skelton with BSpline 

            var tmp_checkpoint = [];

            var spline = new BSpline(points_ex,3);
            var spline2 = new BSpline(points2_ex,3);
            var spline_color = new BSpline(colors,3);

            for (var t = 0; t <= 1; t += 0.001) // small steps
            {
                var p = spline.calcAt(t);
                var p2 = spline2.calcAt(t);
                var color = spline_color.calcAt(t);

                var curr = [ p[0], p[1], p[2], p2[0], p2[1], p2[2], color[0],color[1],color[2] ];

                if (tmp_checkpoint.length > 0)
                {
                    // validate the point

                    var prev = tmp_checkpoint[tmp_checkpoint.length-1];

                    var diff = [
                        curr[0]-prev[0],
                        curr[1]-prev[1],
                        curr[2]-prev[2],
                        curr[3]-prev[3],
                        curr[4]-prev[4],
                        curr[5]-prev[5],
                    ];

                    var len1 = Math.sqrt(diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2]);
                    var len2 = Math.sqrt(diff[3]*diff[3] + diff[4]*diff[4] + diff[5]*diff[5]);

                    // do not add steps that are too close from each other
                    if (len1 < 2 && len2 < 2)
                        continue;
                }

                tmp_checkpoint.push(curr);
            }

            arr_checkpoints = tmp_checkpoint;

            // smooth skelton with BSpline 
            //

        } // building the circuit


        this._circuit = new createCircuit( arr_checkpoints );



        var genome_size = 30;
        this._ann_topology = [17, 4, 2];

        this._geneticAlgo = new createGeneticAlgo( genome_size, this._ann_topology );

        //

        // cars
        this._cars = [];
        for (var i = 0; i < genome_size; ++i)
        {
            var car = new createCar();

            this._geneticAlgo._genomes[i].car = car;

            car.reset();

            this._cars.push(car);
        }

        this._time_ratio = 0;
        this._magik_limit = 0;
    };

    //

    createSimulation.prototype.update = function(time)
    {
        world._my_update(time);

        var someone_is_alive = false;

        for (var i = 0; i < this._cars.length; ++i)
        {
            if (!this._cars[i]._alive)
                continue;

            this._cars[i].processANN( this._geneticAlgo._ANNs[i] );
            this._cars[i].update();

            someone_is_alive = true;
        }


        // end of the current generation?

        if (someone_is_alive)
            return; // no

        // rate the genome

        for (var i = 0; i < this._cars.length; ++i)
            this._geneticAlgo._genomes[i].fitness = this._cars[i]._fitness;

        this._geneticAlgo.BreedPopulation();

        // reset the cars

        for (var i = 0; i < this._cars.length; ++i)
        {
            var car = this._cars[i];

            this._geneticAlgo._genomes[i].car = car;

            car.reset();
        }
    };

    //

    createSimulation.prototype.render = function(in_shader_color, in_viewMatrix, car_index)
    {
        for (var i = 0; i < this._cars.length; ++i)
            this._cars[i].render(in_shader_color, in_viewMatrix);

        gl.uniformMatrix4fv(in_shader_color.uMVMatrix, false, in_viewMatrix); // lookAt only

        if (car_index >= 0)
        {
            gl.uniform1f(in_shader_color.uColorApha, 0.5);

            this._cars[car_index].render_sensors(in_shader_color);

            this._cars[car_index].render_vision();
        }

        this._geom_skelton.render(in_shader_color, false);
    };

    //

    createSimulation.prototype.render_exp = function(in_shader_exp)
    {
        var best_fitness = 0;
        for (var i = 0; i < this._cars.length; ++i)
            if (this._cars[i]._alive &&
                best_fitness < this._cars[i]._fitness)
                best_fitness = this._cars[i]._fitness;

        var target = best_fitness + 1;

        if (this._time_ratio < target)
        {
            this._time_ratio += 0.1;
            if (this._time_ratio > target)
                this._time_ratio = target;
        }
        else if (this._time_ratio > target)
        {
            this._time_ratio -= 0.3;
            if (this._time_ratio < target)
                this._time_ratio = target;
        }

        gl.uniform1f(in_shader_exp.uMagikIndex, this._time_ratio);

        if (this._magik_limit < target)
        {
            this._magik_limit += 0.3;
            if (this._magik_limit > target)
                this._magik_limit = target;
        }
        else if (this._magik_limit > target)
        {
            this._magik_limit -= 0.1;
            if (this._magik_limit < target)
                this._magik_limit = target;
        }

        gl.uniform1f(in_shader_exp.uMagikLimit, this._magik_limit);

        this._circuit.render_exp(in_shader_exp);
    }


    return createSimulation;
})
