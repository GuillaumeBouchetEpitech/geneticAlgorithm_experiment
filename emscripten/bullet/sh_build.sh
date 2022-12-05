#!/bin/bash

echo ""
echo "Build target?"
echo "=> native:        1 (default)"
echo "=> web (pthread): 2"
echo "=> web (worker):  3"
echo "=> full release:  4"
echo ""

read USER_INPUT_PLATFORM

case $USER_INPUT_PLATFORM in
2)
    echo ""
    echo "selected target: web (pthread)"
    echo ""
    selected_platform=web_pthread
    ;;
3)
    echo ""
    echo "selected target: web (worker)"
    echo ""
    selected_platform=web_worker
    ;;
4)
    echo ""
    echo "selected target: full release"
    echo ""
    selected_platform=full_release

    echo ""
    echo "=> cleanup"
    echo ""
    make build_platform="native_pthread" build_mode="release" fclean_ml
    make build_platform="web_wasm_webworker" build_mode="release" fclean_ml

    make build_platform="native_pthread" build_mode="release" fclean
    make build_platform="web_wasm_pthread" build_mode="release" fclean
    make build_platform="web_wasm_webworker" build_mode="release" fclean

    echo ""
    echo "=> building library/ies"
    echo ""
    make build_platform="native_pthread" build_mode="release" machine_learning -j4
    make build_platform="web_wasm_webworker" build_mode="release" machine_learning -j4

    echo ""
    echo "=> building projects"
    echo ""
    make build_platform="native_pthread" build_mode="release" all -j4
    make build_platform="web_wasm_pthread" build_mode="release" all -j4
    make build_platform="web_wasm_webworker" build_mode="release" all -j4

    echo ""
    echo "=> building completed"
    echo ""

    exit 0;
    ;;
*)
    echo ""
    echo "selected target: experiment"
    echo ""
    selected_platform=native
    ;;
esac

echo ""
echo "Build mode?"
echo "=> release: 1 (default)"
echo "=> debug:   2"
echo ""

read USER_INPUT_MODE

case $USER_INPUT_MODE in
2)
    echo ""
    echo "selected mode: debug"
    echo ""
    selected_mode=debug
    ;;
*)
    echo ""
    echo "selected mode: release"
    echo ""
    selected_mode=release
    ;;
esac

echo ""
echo "Rebuild?"
echo "=> no:  1 (default)"
echo "=> yes: 2"
echo ""

read USER_INPUT_REBUILD

case $USER_INPUT_REBUILD in
2)
    echo ""
    echo "rebuilding: yes"
    echo ""
    must_clean=yes
    ;;
*)
    echo ""
    echo "rebuilding: no"
    echo ""
    must_clean=no
    ;;
esac

#
#
#

case $selected_platform in
native)
    ARG_SEL_PLATFORM="native_pthread"
    ;;
web_pthread)
    ARG_SEL_PLATFORM="web_wasm_pthread"
    ;;
web_worker)
    ARG_SEL_PLATFORM="web_wasm_webworker"
    ;;
esac

case $selected_mode in
debug)
    ARG_SEL_MODE="debug"
    ;;
release)
    ARG_SEL_MODE="release"
    ;;
esac

#
#
#

# enable_emscripten() {

#     echo ""
#     echo "EMSDK ENV"

#     OLD_PWD=$PWD
#     cd ../tools/emsdk
#     . ./emsdk_env.sh
#     cd $OLD_PWD

#     echo ""
#     echo "EM++"

#     em++ --version
# }

# case $selected_platform in
# web_pthread)
#     enable_emscripten
#     ;;
# web_worker)
#     enable_emscripten
#     ;;
# esac

#
#
#

echo ""
echo "=> building"
echo ""
case $must_clean in
yes)
    make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" fclean
    ;;
esac

# careful: the `-j4` option will mess with the sdl2 and sdl2_image wasm port
make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" machine_learning -j4
make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" all -j4
