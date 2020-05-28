
#pragma once

#include "demo/defines.hpp"

#include "IState.hpp"

#include "demo/utilities/types.hpp"

#include <array>

class StateManager
{
    //
    //
    // singleton

private:
    static StateManager* _instance;

    StateManager();
    ~StateManager();

public:
    static void create();
    static void destroy();
    static StateManager* get();

    // singleton
    //
    //

public:
    // an enum class would be nice
    // but the last enum is used to set the size of a std::array
    // which doesn't seems to work with enum class
    enum class States: int
    {
#if defined D_WEB_WEBWORKER_BUILD
        eWorkersLoading = 0,
        eRunning,
#else
        eRunning = 0,
#endif
        ePaused,
        eTotal
    };

private:
    typedef std::array<IState*, toUnderlying(States::eTotal)> t_States;
    t_States _states;

    States _currentState;

public:
    // change state
    void changeState(States nextState);

public:
    void handleEvent(const SDL_Event&);
    void update(int);
    void render(const SDL_Window&);
    void resize(int, int);
    void visibility(bool);
};
