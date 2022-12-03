#!/bin/bash

echo ""
echo "SHELL      => \""$SHELL"\""
echo "PWD        => \""$PWD"\""
echo "EMSDK      => \""$EMSDK"\""
echo "EMSCRIPTEN => \""$EMSCRIPTEN"\""
echo ""

echo ""
echo "Build target?"
echo "=> native:   1 (default)"
echo "=> web wasm: 2"
echo ""

read USER_INPUT_PLATFORM

case $USER_INPUT_PLATFORM in
2)
    echo ""
    echo "selected target: web (pthread)"
    echo ""
    selected_platform=web_wasm
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
    ARG_SEL_PLATFORM="native"
    ;;
web_wasm)
    ARG_SEL_PLATFORM="web_wasm"
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

# echo ""
# echo "EMSDK ENV"

# OLD_PWD=$PWD
# # cd ./tools/emsdk
# cd ../tools/emsdk
# . ./emsdk_env.sh
# cd $OLD_PWD

echo ""
echo "EM++"

em++ --version

echo ""
echo "=> building"
echo ""

case $must_clean in
yes)
    make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" fclean
    ;;
esac

make build_platform="${ARG_SEL_PLATFORM}" build_mode="${ARG_SEL_MODE}" all -j6
