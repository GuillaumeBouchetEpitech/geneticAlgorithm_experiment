
#include "SDLWindowWrapper.hpp"

#include "../ErrorHandler.hpp"

#include <SDL2/SDL_image.h> // <= IMG_Init

#include <stdexcept>

#if defined __EMSCRIPTEN__
#	include <emscripten.h>
#	if defined __EMSCRIPTEN_PTHREADS__
#		include <emscripten/html5.h>
#	endif
#endif

SDLWindowWrapper::SDLWindowWrapper(int width, int height)
{

#if defined __EMSCRIPTEN__ && defined __EMSCRIPTEN_PTHREADS__

    {
        // fix an error when build for the web using pthread
        // => link: https://github.com/emscripten-core/emscripten/issues/7684

        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE prevCtx = emscripten_webgl_get_current_context();
        if (prevCtx == 0)
        {
            D_MYLOG("WebGLContext (pthread): context not set");
            D_MYLOG("WebGLContext (pthread) => attempting a workaround");

            EmscriptenWebGLContextAttributes attr;
            emscripten_webgl_init_context_attributes(&attr);
            attr.majorVersion = 2;
            attr.minorVersion = 0;

            // NOTE: it returns the WebGLContext if any provided
            EMSCRIPTEN_WEBGL_CONTEXT_HANDLE newCtx = emscripten_webgl_create_context(0, &attr);

            if (newCtx != 0)
            {
                emscripten_webgl_make_context_current(newCtx);

                EMSCRIPTEN_WEBGL_CONTEXT_HANDLE newCtx = emscripten_webgl_get_current_context();
                if (newCtx != 0)
                {
                    D_MYLOG("WebGLContext (pthread) => workaround succeeded");
                }
                else
                {
                    D_MYLOG("WebGLContext (pthread) => workaround failed");
                }
            }
            else
            {
                D_MYLOG("WebGLContext (pthread) => workaround failed");
            }
            
        }
    }

#endif

     if (SDL_Init(SDL_INIT_VIDEO) < 0)
        D_THROW(std::runtime_error, "Could not initialise SDL, error: " << SDL_GetError());

    IMG_Init(IMG_INIT_PNG); 

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    const int	posX = SDL_WINDOWPOS_UNDEFINED;
    const int	posY = SDL_WINDOWPOS_UNDEFINED;
    const int	flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    // SDL_SetRelativeMouseMode(SDL_TRUE);

    _window = SDL_CreateWindow("AI", posX, posY, width, height, flags);

    if (!_window)
        D_THROW(std::runtime_error, "Could not create the window: " << SDL_GetError());

    try
    {
        _glContextId = SDL_GL_CreateContext(_window);
        if (!_glContextId)
            D_THROW(std::runtime_error, "Failed to create GL context: " << SDL_GetError());

        try
        {
            if (SDL_GL_MakeCurrent(_window, _glContextId) < 0)
                D_THROW(std::runtime_error, "Failed to make GL context current: " << SDL_GetError());
        }
        catch (const std::exception& error)
        {
            SDL_GL_DeleteContext(_glContextId);

            throw error;
        }
    }
    catch (const std::exception& error)
    {
        SDL_DestroyWindow(_window);
        SDL_Quit();

        throw error;
    }

    _startTime = SDL_GetTicks();

    D_MYLOG("GL_VERSION: " << glGetString(GL_VERSION));

    glViewport(0, 0, width, height);
}

SDLWindowWrapper::~SDLWindowWrapper()
{
    stop();

    if (_glContextId)
        SDL_GL_DeleteContext(_glContextId);

    if (_window)
        SDL_DestroyWindow(_window);

    SDL_Quit();
}

//

#if defined __EMSCRIPTEN__

void	SDLWindowWrapper::step(void* pData)
{
    SDLWindowWrapper* self = static_cast<SDLWindowWrapper*>(pData);

    // self->process();

    const unsigned int currentTime = SDL_GetTicks(); // in millisecond
    const unsigned int deltaTime = currentTime - self->_startTime;

    self->process(deltaTime);

    self->_startTime = currentTime;
}

#endif

void	SDLWindowWrapper::run()
{
    if (_running)
        return;
    _running = true;

#if defined __EMSCRIPTEN__

    emscripten_set_main_loop_arg(SDLWindowWrapper::step, (void*)this, 0, true);

#else

    while (_running)
    {
        const unsigned int currentTime = SDL_GetTicks(); // in millisecond
        const unsigned int delta = currentTime - _startTime;

        // this->onUpdate(delta);
        process(delta);

        _startTime = currentTime;
        const int maxDelay = 16; // 16ms, duration of one frame at 60FPS
        const int mustWait = maxDelay - (int)delta;
        if (mustWait > 0)
            SDL_Delay(mustWait);
    }

#endif
}

void	SDLWindowWrapper::stop()
{
    if (!_running)
        return;

#if defined __EMSCRIPTEN__

    emscripten_cancel_main_loop();

#endif

    _running = false;
}

//

void	SDLWindowWrapper::process(unsigned int deltaTime)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {

        switch (event.type)
        {

#if not defined __EMSCRIPTEN__

            case SDL_QUIT:
            {
                return stop();
            }
            case SDL_KEYUP:
            {
                int symbol = event.key.keysym.sym;
                if (symbol == SDLK_ESCAPE)
                    return stop();
                break;
            }

#endif

            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {
                        int	width = 0;
                        int	height = 0;
                        SDL_GL_GetDrawableSize(_window, &width, &height);
                        if (width == 0 || height == 0)
                            SDL_GetWindowSize(_window, &width, &height);

                        if (width == 0)
                            width = 1;
                        if (height == 0)
                            height = 1;

                        onResize(width, height);
                        break;
                    }
                    case SDL_WINDOWEVENT_SHOWN:
                    {
                        onVisibilityChange(_visible = true);
                        break;
                    }
                    case SDL_WINDOWEVENT_HIDDEN:
                    {
                        onVisibilityChange(_visible = false);
                        break;
                    }
                }
            }
        }

        onEvent(event);
    }

    // const unsigned int currentTime = SDL_GetTicks();
    // const unsigned int deltaTime = currentTime - _startTime;

    onUpdate(deltaTime);

    // _startTime = currentTime;

    if (!_visible)
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    onRender(*_window);

    SDL_GL_SwapWindow(_window);
}
