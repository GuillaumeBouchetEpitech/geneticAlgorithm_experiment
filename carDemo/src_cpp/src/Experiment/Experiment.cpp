

#include "Experiment.hpp"

#include "Simulation/Simulation.hpp"

#include "Simulation/utils/TraceLogger.hpp"

#include "Renderer/RendererSFML.hpp"


Experiment::Experiment()
{
}

void	Experiment::run()
{
	Simulation	my_Simulation("./res/Map_test.txt");

	RendererSFML	my_RendererSFML(my_Simulation);

	my_RendererSFML.run([&]()
	{
		my_Simulation.update(0.25f);
	});
}


