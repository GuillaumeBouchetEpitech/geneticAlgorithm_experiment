
#pragma once

#if defined __EMSCRIPTEN__

#define D_WEB_BUILD

#include <emscripten.h>

#if defined __EMSCRIPTEN_PTHREADS__
#define D_WEB_PTHREAD_BUILD
#else
#define D_WEB_WEBWORKER_BUILD
#endif
#else
#define D_NATIVE_PTHREAD_BUILD
#endif
