
#pragma once

class Scene
{
public:
    static void renderSimple();
    static void renderAll();

private:
    static void updateMatrices();

    static void clear();

    static void renderLeadingCarSensors();
    static void renderParticles();
    static void renderCars();
    static void renderWireframesGeometries(bool trails = true);
    static void renderAnimatedCircuit();
    static void renderHUD();

};
