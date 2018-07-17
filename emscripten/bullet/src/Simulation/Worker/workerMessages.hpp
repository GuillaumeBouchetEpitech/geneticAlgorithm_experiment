

#pragma once


#include "constants.hpp"


namespace workerClientMessages
{
	enum e_Type : char
	{
		eInitialiseSimulation = 0,
		eResetAndProcessSimulation,
		eProcessSimulation,
		eUnknown,
	};

	struct t_Message
	{
		e_Type	type = e_Type::eUnknown;

		t_Message() = default;
	};

	struct t_InitialiseSimulationMessage
		: public t_Message
	{
		// ctor

		t_InitialiseSimulationMessage()
			: t_Message()
		{
			type = e_Type::eInitialiseSimulation;
		}
	};

	struct t_ResetAndProcessSimulationMessage
		: public t_Message
	{
		struct t_neuralNetworkWeights
		{
			float	weights[D_CAR_WEIGHTS_NUMBER_TOTAL];
		}
		neuralNetworkWeights[D_CARS_NUMBER_PER_WORKER];

		// ctor

		t_ResetAndProcessSimulationMessage()
			: t_Message()
		{
			type = e_Type::eResetAndProcessSimulation;
		}
	};

	struct t_ProcessSimulationMessage
		: public t_Message
	{
		// ctor

		t_ProcessSimulationMessage()
			: t_Message()
		{
			type = e_Type::eProcessSimulation;
		}
	};
};

namespace workerServerMessages
{
	enum e_Type : char
	{
		eSimulationInitialised = 0,
		eSimulationResult,
		eUnknown
	};

	struct t_Message
	{
		e_Type	type = e_Type::eUnknown;

		t_Message() = default;
	};

	struct t_SimulationInitialisedMessage
		: public t_Message
	{
		// ctor

		t_SimulationInitialisedMessage()
			: t_Message()
		{
			type = e_Type::eSimulationInitialised;
		}
	};

	struct t_SimulationResultMessage
		: public t_Message
	{
		struct t_carResult
		{
			char	isAlive;
			float	chassisMat4[16];
			float	wheelsMat4[4][16];
			float	fitness;

			struct t_sensor
			{
				float	near[3];
				float	far[3];
				float	value;
			}
			sensors[15],
			groundSensor;
		}
		carsResult[D_CARS_NUMBER_PER_WORKER];

		// ctor

		t_SimulationResultMessage()
			: t_Message()
		{
			type = e_Type::eSimulationResult;
		}
	};
};

