#!/bin/bash

#
# INIT

DIR_THIRDPARTY=$PWD/src/thirdparty
BULLET3_DIR=bullet3
BULLET3_URL=bulletphysics/bullet3
BULLET3_TAG=2.87
GLM_DIR=glm
GLM_URL=g-truc/glm
GLM_TAG=0.9.9.2
TINYOBJ_DIR=tinyobjloader
TINYOBJ_URL=syoyo/tinyobjloader
TINYOBJ_TAG=v1.0.6
STB_DIR=stb
STB_URL=nothings/stb
STB_TAG=master

mkdir -p $DIR_THIRDPARTY

clone_repo() {

    CURR_URL=$1
    CURR_TAG=$2
    CURR_DIR=$3

    if [ -d "$DIR_THIRDPARTY/$CURR_DIR" ]
    then
        echo ""
        echo "Directory $DIR_THIRDPARTY/$CURR_DIR exists, replace?"
        echo "=> no:  1 (default)"
        echo "=> yes: 2"
        echo ""

        read USER_INPUT_PROJECT

        case $USER_INPUT_PROJECT in
        2)
            echo "replacing"
            echo ""
            must_install=yes
            ;;
        *)
            echo "not replacing"
            echo ""
            must_install=no
            ;;
        esac
    else
        must_install=yes
    fi


    case $must_install in
    yes)
        echo ""
        echo "installing"
        echo ""

        cd $DIR_THIRDPARTY

        # reset
        rm -rf $CURR_DIR

        # install
        git clone --depth 1 --branch $CURR_TAG https://github.com/$CURR_URL $CURR_DIR
        ;;
    esac
}

clone_repo $BULLET3_URL $BULLET3_TAG $BULLET3_DIR
clone_repo $GLM_URL     $GLM_TAG     $GLM_DIR
clone_repo $TINYOBJ_URL $TINYOBJ_TAG $TINYOBJ_DIR
clone_repo $STB_URL     $STB_TAG     $STB_DIR





