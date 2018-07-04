

#ifndef D_RENDERERSFML_HPP
#define D_RENDERERSFML_HPP


#include "../Simulation/Simulation.hpp"


#include <functional>


class RendererSFML
{
private: // attributs
	Simulation&	m_Simulation;

public: // ctor/dtor
	RendererSFML(Simulation& simulation);

public: // method(s)
	void	run(std::function<void()> simulation_callback);
};



#endif // D_RENDERERSFML_HPP
