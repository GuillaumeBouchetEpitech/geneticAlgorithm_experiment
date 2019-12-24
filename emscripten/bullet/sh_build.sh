#!/bin/bash

echo ""
echo "SHELL      => \""$SHELL"\""
echo "PWD        => \""$PWD"\""
echo "EMSDK      => \""$EMSDK"\""
echo "EMSCRIPTEN => \""$EMSCRIPTEN"\""
echo ""



# make build_platform="native_pthread" build_mode="debug"
# exit



# MY_ARGS="LOL "
# MY_ARGS="${MY_ARGS}build_platform=native_pthread"
# echo $MY_ARGS
# exit




echo ""
echo "Build what?"
echo "=> C++ Experiment: 1 (default)"
echo "=> C++ Bullet:     2"
echo ""

read USER_INPUT_PROJECT

case $USER_INPUT_PROJECT in
2)
    echo ""
    echo "selected project: bullet"
    echo ""
    selected_project=bullet
    ;;
*)
    echo ""
    echo "selected project: experiment"
    echo ""
    selected_project=experiment
    ;;
esac

echo ""
echo "Build target?"
echo "=> native:        1 (default)"
echo "=> web (pthread): 2"
echo "=> web (worker):  3"
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

echo ""
echo "EMSDK ENV"

OLD_PWD=$PWD
cd ./tools/emsdk
. ./emsdk_env.sh
cd $OLD_PWD

echo ""
echo "EM++"

em++ --version

case $selected_project in
bullet)
    echo ""
    echo "=> building Bullet"
    echo ""
    case $must_clean in
    yes)
        make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" bullet_fclean
        ;;
    esac
    make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" bullet -j7
    ;;
experiment)
    echo ""
    echo "=> building Experiment"
    echo ""
    case $must_clean in
    yes)
        make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" fclean
        ;;
    esac
    # careful: the `-j7` option will mess with the sdl2 and sdl2_image wasm port
    make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" all -j7

    # # currently the build script must be used several times to get all the ports correctly...
    # make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" all
    ;;
esac
