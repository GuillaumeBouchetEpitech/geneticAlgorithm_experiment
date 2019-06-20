
#pragma once

class Scene
{
public:
	static void renderSimple();
	static void renderAll();

	static void setUniforms();

	static void clear();

	static void renderLeadingCarSensors();
	static void renderCars();
	static void	renderCircuitSkeleton();
	static void	renderBestCarsTrails();
	static void	renderMonoColorGeometries(bool circuit = true, bool trails = true);
	static void	renderAnimatedCircuit();
	static void	renderHUD();

};
