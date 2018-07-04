

#include "Game.hpp"

#include "States/StateManager.hpp"

#include "Logic/Data.hpp"

#include "Utility/TraceLogger.hpp"


#ifdef EMSCRIPTEN
#	include <emscripten.h>
#endif



Game::Game()
{
	int width = 800;
	int height = 600;

	m_pStage = new SDLStage(width, height);

	// m_pStage->setCaption("OpenGL and Bullet");

	m_pStage->setEventListener([this](const SDL_Event &event)
	{
		StateManager::get()->handleEvent(event);
	});

	m_pStage->setUpdateCallback([this](int deltaTime)
	{
#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeter_update.tickStart(););
#endif

		StateManager::get()->update(deltaTime);

#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeter_update.tick(););
#endif
	});

	m_pStage->setRenderCallback([this](const SDL_Window& screen)
	{
#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeter_render.tickStart(););
#endif

		StateManager::get()->render(screen);

#ifdef EMSCRIPTEN
		EM_ASM(myFpsmeter_render.tick(););
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
	Data::get()->m_graphic.window_size = { width, height };

	StateManager::create();
}

Game::~Game()
{
	StateManager::destroy();
	Data::destroy();

	delete m_pStage;
}



#ifdef EMSCRIPTEN

void	Game::step(void* data)
{
	Game*	self = ((Game*)data);

	unsigned int time_current = SDL_GetTicks();
	unsigned int delta = time_current - self->m_time_start;

	// D_MYLOG("time_current=" << time_current);
	self->update(delta);

	self->m_time_start = time_current;
}

void	Game::run()
{
	emscripten_set_main_loop_arg(Game::step, (void*)this, 0, true);
}

#else

void	Game::run()
{
	while (m_active)
	{
		unsigned int time_current = SDL_GetTicks();
		unsigned int delta = time_current - m_time_start;

		// D_MYLOG("time_current=" << time_current);
		this->update(delta);

		m_time_start = time_current;
		int to_wait = 16 - (int)delta; // <= 16 = 16ms = ~1/60FPS
		if (to_wait > 0)
			SDL_Delay(to_wait);
	}
}

#endif

void	Game::update(long int deltaTime)
{
	// auto*	pWindow = Data::get()->Wrappers.m_pWindow;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				m_active = false;
				break;
			}
			case SDL_KEYUP:
			{
				int sym = event.key.keysym.sym;
				if (sym == SDLK_ESCAPE)
					m_active = false;
				break;
			}
		}

		// pWindow->ProcessEvent(&event);
		m_pStage->handleEvent(event);
	}

	m_pStage->update(deltaTime);

	// if (pWindow->visible)
	// pWindow->Render();
	m_pStage->render();
}
