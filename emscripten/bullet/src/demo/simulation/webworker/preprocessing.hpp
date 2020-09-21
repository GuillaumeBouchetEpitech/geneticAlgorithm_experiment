
#pragma once

// hacky way to force the name of the function to be from the macro "D_WORKER_MAIN"
#define WORKER_MAIN_FUNC_NAME_FUNC(func_name) _ ## func_name
#define WORKER_MAIN_FUNC_NAME_STR(func_name) "_" # func_name

#define WORKER_MAIN_FUNC  WORKER_MAIN_FUNC_NAME_FUNC(D_WORKER_MAIN)
#define WORKER_MAIN_STR   WORKER_MAIN_FUNC_NAME_STR(D_WORKER_MAIN)
