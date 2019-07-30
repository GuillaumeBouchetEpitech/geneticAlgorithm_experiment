
# node installDeps.js

# python3 py_install.py

#
# INIT

DIR_THIRDPARTY=$PWD/src/thirdparty
BULLET3_DIR=bullet3
BULLET3_URL=bulletphysics/bullet3
BULLET3_TAG=2.87
GLM_DIR=glm
GLM_URL=g-truc/glm
GLM_TAG=0.9.9.2

mkdir -p $DIR_THIRDPARTY

#
# BULLET

cd $DIR_THIRDPARTY

rm -rf $BULLET3_DIR

git clone \
    --branch $BULLET3_TAG \
    --depth 1 \
    https://github.com/$BULLET3_URL

#
# GLM

cd $DIR_THIRDPARTY

rm -rf $GLM_DIR

git clone \
    --branch $GLM_TAG \
    --depth 1 \
    https://github.com/$GLM_URL
