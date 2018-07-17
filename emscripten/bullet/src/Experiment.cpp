

#include "Experiment.hpp"

#include "States/StateManager.hpp"

#include "Logic/Data.hpp"

#include "Utility/TraceLogger.hpp"

#include "constants.hpp"

#ifdef EMSCRIPTEN
#	include <emscripten.h>
#endif



Experiment::Experiment()
{
	m_pStage = new SDLStage(D_WINDOW_WIDTH, D_WINDOW_HEIGHT);

	// m_pStage->setCaption("OpenGL and Bullet");

	m_pStage->setEventListener([this](const SDL_Event &event)
	{
		StateManager::get()->handleEvent(event);
	});

	m_pStage->setUpdateCallback([this](int deltaTime)
	{
#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeterUpdate.tickStart(););
#endif

		StateManager::get()->update(deltaTime);

#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeterUpdate.tick(););
#endif
	});

	m_pStage->setRenderCallback([this](const SDL_Window& screen)
	{
#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeterRender.tickStart(););
#endif

		StateManager::get()->render(screen);

#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeterRender.tick(););
#endif
	});

	m_pStage->setResizeCallback([this](int width, int height)
	{
		StateManager::get()->resize(width, height);
	});

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef __EMSCRIPTEN__
	glEnable( GL_TEXTURE_2D ); // <= the C++ version need it
#endif

	m_time_start = SDL_GetTicks();

	Data::create();
	Data::get()->graphic.camera.viewportSize = { D_WINDOW_WIDTH, D_WINDOW_HEIGHT };

	StateManager::create();
}

Experiment::~Experiment()
{
	StateManager::destroy();
	Data::destroy();

	delete m_pStage;
}



#ifdef EMSCRIPTEN

void	Experiment::step(void* pData)
{
	Experiment*	pExperiment = static_cast<Experiment*>(pData);

	unsigned int time_current = SDL_GetTicks();
	unsigned int delta = time_current - pExperiment->m_time_start;

	pExperiment->update(delta);

	pExperiment->m_time_start = time_current;
}

void	Experiment::run()
{
	emscripten_set_main_loop_arg(Experiment::step, (void*)this, 0, true);
}

#else

void	Experiment::run()
{
	while (m_running)
	{
		unsigned int time_current = SDL_GetTicks();
		unsigned int delta = time_current - m_time_start;

		this->update(delta);

		m_time_start = time_current;
		int to_wait = 16 - (int)delta; // <= 16 (16ms = ~1 / 60 FPS)
		if (to_wait > 0)
			SDL_Delay(to_wait);
	}
}

#endif

void	Experiment::update(long int deltaTime)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				m_running = false;
				break;
			}
			case SDL_KEYUP:
			{
				int sym = event.key.keysym.sym;
				if (sym == SDLK_ESCAPE)
					m_running = false;
				break;
			}
		}

		m_pStage->handleEvent(event);
	}

	m_pStage->update(deltaTime);

	m_pStage->render();
}
