
#pragma once

#if 0

#include <cmath> // <= M_PI

#define D_WINDOW_WIDTH	int(800)
#define D_WINDOW_HEIGHT	int(600)

#define D_CIRCUIT_FILENAME	"assets/circuits/default.txt"

// #define D_WORKER_SCRIPT_URL	"asm.js/worker.js"
// #define D_WORKER_MAIN		"onMessage"


#define	D_WORKERS_NUMBER			int(3)
#define	D_CARS_NUMBER_PER_WORKER	int(30)
#define	D_CARS_NUMBER_TOTAL			int(D_WORKERS_NUMBER * D_CARS_NUMBER_PER_WORKER)


// #define	D_CAR_EYE_MAX_RANGE			int(D_CAR_EYE_VERTICAL_TOTAL * D_CAR_EYE_HORIZONTAL_TOTAL)
#define	D_CAR_STEER_MAX_VALUE		float(M_PI / 8.0f)
#define	D_CAR_SPEED_MAX_VALUE		float(20.0f)
#define	D_CAR_HEALTH_MAX_VALUE		int(200)


#define	D_NEURONS_NUMBER_INPUT		int(15)
#define	D_NEURONS_NUMBER_HIDDEN_1	int(10)
#define	D_NEURONS_NUMBER_HIDDEN_2	int(5)
#define	D_NEURONS_NUMBER_OUTPUT		int(2)
#define	D_NEURAL_NETWORK_USE_BIAS	true

#define	D_NEURONS_NUMBER_BIAS	(D_NEURAL_NETWORK_USE_BIAS ? 1 : 0)

#endif

#if 0

#define	D_CAR_WEIGHTS_NUMBER_TOTAL	int(\
	(D_NEURONS_NUMBER_INPUT + D_NEURONS_NUMBER_BIAS) * \
	(D_NEURONS_NUMBER_HIDDEN_1 + D_NEURONS_NUMBER_BIAS) \
	+ \
	(D_NEURONS_NUMBER_HIDDEN_1 + D_NEURONS_NUMBER_BIAS) * \
	(D_NEURONS_NUMBER_HIDDEN_2 + D_NEURONS_NUMBER_BIAS) \
	+ \
	(D_NEURONS_NUMBER_HIDDEN_2 + D_NEURONS_NUMBER_BIAS) * \
	(D_NEURONS_NUMBER_OUTPUT) \
)

#endif