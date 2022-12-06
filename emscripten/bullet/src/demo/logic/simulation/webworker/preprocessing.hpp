
#pragma once

#define D_WORKER_SCRIPT_URL "wasm/webworker/worker.js"
#define D_WORKER_MAIN "onMessage"

#define D_WORKER_MAIN_FUNC_NAME_FUNC(func_name) _##func_name
#define D_WORKER_MAIN_FUNC_NAME_STR(func_name) "_" #func_name

// hacky way to force the name of the function to be from the macro
// "D_WORKER_MAIN"
#define D_WORKER_MAIN_FUNC D_WORKER_MAIN_FUNC_NAME_FUNC(D_WORKER_MAIN)
#define D_WORKER_MAIN_STR D_WORKER_MAIN_FUNC_NAME_STR(D_WORKER_MAIN)
