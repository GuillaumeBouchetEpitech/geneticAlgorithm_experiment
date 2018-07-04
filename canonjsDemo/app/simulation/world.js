
'use strict'

define(
    [
          'physic/cannon.min' // in /lib
    ],
    function(
          unused_CANNON // <- use CANNON
    )
{
    var world = new CANNON.World();

    // world.broadphase = new CANNON.GridBroadphase(new CANNON.Vec3(0,0,0), new CANNON.Vec3(100,100,100), 100, 100, 100); // <- not working
    // world.broadphase = new CANNON.NaiveBroadphase(); // working
    // world.broadphase = new CANNON.SAPBroadphase(world);

    // world.solver.tolerance = 0.001;

    world.gravity.set(0, 0, -9.82); // m/s²

    world._GROUP_sensor = 1 << 0;
    world._GROUP_wall = 1 << 1;

    //
    //

    //
    //

    var fixedTimeStep = 1.0 / 60.0; // seconds
    var maxSubSteps = 3;
    var lastTime;

    world._my_update = function (time)
    {
        world.step(fixedTimeStep, 0, 0);
        // if (lastTime !== undefined)
        // {
        //     var dt = (time - lastTime) / 1000;
        //     world.step(fixedTimeStep, dt, maxSubSteps);
        // }
        // lastTime = time;
    }

    //
    //

    //
    //

    return world;
});