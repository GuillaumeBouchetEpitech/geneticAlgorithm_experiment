

#pragma once


#include "Utility/NonCopyable.hpp"

#include "SDLStage.hpp"


class Experiment
	: public NonCopyable
{
private:
	bool m_running = true;
	SDLStage*	m_pStage;

	unsigned int	m_time_start = 0;

public:
	Experiment();
	~Experiment();

#ifdef EMSCRIPTEN

private:
	static void step(void* data);

#endif

public:
	void	run();

private:
	void	update(long int deltaTime);

};

