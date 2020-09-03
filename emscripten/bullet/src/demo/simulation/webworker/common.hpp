
#pragma once

#include "demo/defines.hpp"

namespace messages
{
    enum class client : int
    {
        eLoadWorker = 0,
        eResetAndProcessSimulation,
        eProcessSimulation,
        eUnknown,
    };

    enum class server : int
    {
        eWebWorkerLoaded = 0,
        eSimulationResult,
        eUnknown
    };
};
