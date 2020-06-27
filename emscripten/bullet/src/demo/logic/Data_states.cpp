
#include "demo/defines.hpp"

#include "Data.hpp"

#include "demo/states/StateManager.hpp"

#include <iomanip>
#include <sstream>

void Data::initialiseStates()
{
#if defined D_WEB_WEBWORKER_BUILD

    logic.simulation->setOnWorkersReadyCallback([]() {

        Data::get()->logic.state.countdown = 1000;
    });

#endif
}

