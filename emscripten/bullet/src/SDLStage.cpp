
#include "SDLStage.hpp"

#include "../Utility/TraceLogger.hpp"


// #include <SDL2/SDL_opengles2.h>


SDLStage::SDLStage(int width, int height)
	: m_ticksPerFrame(1000 / 60)
{

	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	m_pWindow = SDL_CreateWindow("Test",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (!m_pWindow)
	{
		D_MYLOG("Could not create the window: " << SDL_GetError());
		return;
	}

	m_glContextID = SDL_GL_CreateContext(m_pWindow);
	if (!m_glContextID)
    {
        D_MYLOG("Failed to create GL context: " << SDL_GetError());
        SDL_DestroyWindow(m_pWindow);
        SDL_Quit();
        return;
    }

    if (SDL_GL_MakeCurrent(m_pWindow, m_glContextID) < 0)
    {
        D_MYLOG("Failed to make GL context current: " << SDL_GetError());
        SDL_GL_DeleteContext(m_glContextID);
        SDL_DestroyWindow(m_pWindow);
        SDL_Quit();
        return;
    }

    D_MYLOG("GL_VERSION: " << glGetString(GL_VERSION));

    SDL_GL_SetSwapInterval(1);

	glViewport(0, 0, width, height);
}

SDLStage::~SDLStage()
{
	SDL_GL_DeleteContext(m_glContextID);
	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

void SDLStage::handleEvent(SDL_Event &event)
{
	switch (event.type)
	{
		case SDL_WINDOWEVENT:
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_SHOWN:
				m_visible = true;
				break;

			case SDL_WINDOWEVENT_HIDDEN:
				m_visible = false;
				break;

			case SDL_WINDOWEVENT_SIZE_CHANGED:
			{
				int local_width = 0, local_height = 0;
				SDL_GL_GetDrawableSize(m_pWindow, &local_width, &local_height);
				// glViewport(0, 0, local_width, local_height);

				if (m_resizeCallback)
					m_resizeCallback(local_width, local_height);
			}
			break;
			}
		}
		break;
	}

	if (m_eventListener)
		m_eventListener(event);
}

void SDLStage::render()
{
	if (!m_visible)
		return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_renderCallback)
		m_renderCallback(*m_pWindow);

	SDL_GL_SwapWindow(m_pWindow);
}

void SDLStage::update(int deltaTime)
{	
	if (m_updateCallback)
		m_updateCallback(deltaTime);
}
