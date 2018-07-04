

#pragma once


#include "Utility/NonCopyable.hpp"

#include "SDLStage.hpp"


class Game
	: public NonCopyable
{
private:
	bool m_active = true;
	SDLStage*	m_pStage;

	unsigned int	m_time_start = 0;

public:
	Game();
	~Game();

#ifdef EMSCRIPTEN

private:
	static void step(void* data);

#endif

public:
	void	run();

private:
	void	update(long int deltaTime);

};

