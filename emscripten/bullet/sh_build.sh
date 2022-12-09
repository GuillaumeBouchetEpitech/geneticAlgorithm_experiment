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
    make build_mode="${mode}" build_platform="${platform}" fclean_ml
}

func_do_build() {

    mode=$1
    platform=$2

    echo ""
    echo "=> building"
    echo "  mode=${mode}"
    echo "  platform=${platform}"
    echo ""

    make build_mode="${mode}" build_platform="${platform}" machine_learning -j4
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
}

func_ask_build_target () {

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

        func_do_clean release native_pthread
        func_do_clean release web_wasm_webworker
        func_do_clean release web_wasm_pthread

        echo ""
        echo "=> building"
        echo ""

        func_do_build release native_pthread
        func_do_build release web_wasm_webworker
        func_do_build release web_wasm_pthread

        echo ""
        echo "=> building completed"
        echo ""

        exit 0;
        ;;
    *)
        echo ""
        echo "selected target: native"
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
