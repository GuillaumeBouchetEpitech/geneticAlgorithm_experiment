
#pragma once

#include "demo/defines.hpp"

namespace Messages
{
    enum class Client : int
    {
        LoadWorker = 0,
        ResetAndProcessSimulation,
        ProcessSimulation,
        Unknown,
    };

    enum class Server : int
    {
        WebWorkerLoaded = 0,
        SimulationResult,
        Unknown
    };
};
