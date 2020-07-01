
#pragma once

#include "demo/defines.hpp"

#include "IState.hpp"

#include "demo/utilities/types.hpp"

#include "demo/utilities/NonCopyable.hpp"

#include <array>

class StateManager
    : public NonCopyable
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
    enum class States: int
    {
#if defined D_WEB_WEBWORKER_BUILD
        eWorkersLoading = 0,
        eRunning,
#else
        eRunning = 0,
#endif
        ePaused,
        eStartGeneration,

        eTotal
    };

private:
    typedef std::array<IState*, toUnderlying(States::eTotal)> t_States;
    t_States _states;

    States _currentState;

public:
    void changeState(States nextState);
    States getState() const;

public:
    void handleEvent(const SDL_Event&);
    void update(int);
    void render(const SDL_Window&);
    void resize(int, int);
    void visibility(bool);
};
