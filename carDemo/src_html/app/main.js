
define(
    [
          'requestAnimationFrame'
        , './graphic/Renderer.js'

        , './utils/collision.js'

        , './simulation/simulation.js'

        , 'fpsmeter' // in /lib
    ],
    function(

          requestAnimationFrame
        , createRenderer

        , utils

        , createSimulation

        , unused_fpsmeter // <- use window.FPSMeter
    )
{


    var Renderer = new createRenderer("main-canvas");

    var simulation = new createSimulation("circuit-data");



    //
    // FPS METER

    var myFpsmeter_elem = document.getElementById('canvasesdiv');
    var myFpsmeter = new window.FPSMeter(
        myFpsmeter_elem,
        window.FPSMeter.theme.transparent
    );

    // FPS METER
    //




    //

    var last_time = 0

    ///
    ///


    tick();

    function tick(in_event) {


        myFpsmeter.tickStart();


        window.requestAnimFrame( tick );

            var current_time = performance.now() || (new Date()).getTime();

            if (!last_time)
                last_time = current_time;

            var elapsed = current_time - last_time;

            last_time = current_time;

        ///


        // UPDATE

        for (var i = 0; i < 5; ++i)
            simulation.update(0.125);

        // /UPDATE


        Renderer.clear();

        // Renderer.drawLine(0,0,200,200, "#ff0000");

        //
        // render circuit

        var checkpoints = simulation._circuit._checkpoints
        for (index in checkpoints)
        {
            var line = checkpoints[index];
            Renderer.drawLine( line.p1.x, line.p1.y, line.p2.x, line.p2.y, "#ffffff");
        }

        var walls = simulation._circuit._walls
        for (index in walls)
        {
            var line = walls[index];
            Renderer.drawLine( line.p1.x, line.p1.y, line.p2.x, line.p2.y, "#0000ff");
        }

        // Renderer.drawPoint(
        //       simulation._circuit._start_position.x
        //     , simulation._circuit._start_position.y
        //     , "#000000"
        // );

        // Renderer.drawLine(
        //       simulation._circuit._start_position.x
        //     , simulation._circuit._start_position.y
        //     , simulation._circuit._start_position.x+Math.cos(simulation._circuit._start_angle) * 50.0
        //     , simulation._circuit._start_position.y+Math.sin(simulation._circuit._start_angle) * 50.0
        //     , "#000000"
        // );

        // render circuit
        //

        //
        // render car

        var cars = simulation._cars
        for (index in cars)
        {
            var car = cars[index];

            var position = car._position;
            var angle = car._angle;

            var size_h = 25.0;
            var size_v = 12.5;

            var positions = [
                {x:position.x - size_h, y:position.y - size_v},
                {x:position.x + size_h, y:position.y - size_v},
                {x:position.x + size_h, y:position.y + size_v},
                {x:position.x - size_h, y:position.y + size_v}
            ];

            for (var i = 0; i < 4; ++i)
                positions[i] = utils.rotateVec2(positions[i], position, angle);

            var color = (car._alive ? "#00ff00" :  "#ff0000");

            Renderer.drawThickLine( positions[0].x, positions[0].y, positions[1].x, positions[1].y, color);
            Renderer.drawThickLine( positions[1].x, positions[1].y, positions[2].x, positions[2].y, color);
            Renderer.drawThickLine( positions[2].x, positions[2].y, positions[3].x, positions[3].y, color);
            Renderer.drawThickLine( positions[3].x, positions[3].y, positions[0].x, positions[0].y, color);

            if (!car._alive)
                continue;

            var sensors = car._sensors;

            for (var i = 0; i < sensors.length; ++i)
            {
                var sensor = sensors[i];

                Renderer.drawLine( sensor.line.p1.x, sensor.line.p1.y, sensor.line.p2.x, sensor.line.p2.y, "#0000ff");

                var pos = {
                      x: sensor.line.p1.x + (sensor.line.p2.x - sensor.line.p1.x) * sensor.value
                    , y: sensor.line.p1.y + (sensor.line.p2.y - sensor.line.p1.y) * sensor.value
                };

                Renderer.drawPoint(pos.x, pos.y, "#0000ff");
            }

        }

        // render car
        //

        //
        // render trails

        var trail_colors = [ "#ffffff", "#cccccc", "#aaaaaa", "#777777", "#444444" ];
        var trail_colors_index = 0;

        var trails = simulation._trails;
        for (var index = (trails.length - 1); index >= 0; --index) // from last to first
        {
            var trail = trails[index];
            var color = trail_colors[trail_colors_index++];

            Renderer.drawLines(trail, color);
        }

        // render trails
        //



        myFpsmeter.tick();


    }

});
