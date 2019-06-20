
#pragma once

#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

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
