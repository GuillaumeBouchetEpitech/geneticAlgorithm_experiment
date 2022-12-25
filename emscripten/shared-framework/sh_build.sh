#!/bin/bash

func_do_clean() {

    mode=$1
    platform=$2

    echo ""
    echo "=> cleaning"
    echo "  mode=${mode}"
    echo "  platform=${platform}"
    echo ""

    make build_mode="${mode}" build_platform="${platform}" fclean
}

func_do_build() {

    mode=$1
    platform=$2

    echo ""
    echo "=> building"
    echo "  mode=${mode}"
    echo "  platform=${platform}"
    echo ""

    make build_mode="${mode}" build_platform="${platform}" all -j4
}

func_try_to_clean() {

    case $must_clean in
    yes)
        mode=$1
        platform=$2

        func_do_clean $mode $platform
        ;;
    esac
}

func_set_build_flags() {

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
}

func_ask_build_target () {

    echo ""
    echo "Build target?"
    echo "=> native:            1 (default)"
    echo "=> web wasm:          2"
    echo "=> full hard release: 3"
    echo "=> full soft release: 4"
    echo ""

    read USER_INPUT_PLATFORM

    case $USER_INPUT_PLATFORM in
    2)
        echo ""
        echo "selected target: web wasm"
        echo ""
        selected_platform=web_wasm
        ;;
    3)
        echo ""
        echo "selected target: full hard release"
        echo ""

        echo ""
        echo "=> cleanup"
        echo ""

        func_do_clean release native
        func_do_clean release web_wasm

        echo ""
        echo "=> building"
        echo ""

        func_do_build release native
        func_do_build release web_wasm

        echo ""
        echo "=> building completed"
        echo ""

        exit 0
        ;;
    4)
        echo ""
        echo "selected target: full soft release"
        echo ""

        echo ""
        echo "=> building"
        echo ""

        func_do_build release native
        func_do_build release web_wasm

        echo ""
        echo "=> building completed"
        echo ""

        exit 0
        ;;
    *)
        echo ""
        echo "selected target: experiment"
        echo ""
        selected_platform=native
        ;;
    esac
}

func_ask_build_mode() {

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
}

func_ask_rebuild() {

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
}

func_ask_build_target
func_ask_build_mode
func_ask_rebuild

func_set_build_flags

func_try_to_clean $ARG_SEL_MODE $ARG_SEL_PLATFORM
func_do_build $ARG_SEL_MODE $ARG_SEL_PLATFORM
