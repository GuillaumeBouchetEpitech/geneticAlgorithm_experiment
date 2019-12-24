
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

syoyo/tinyobjloader



mkdir -p $DIR_THIRDPARTY

#
# BULLET

cd $DIR_THIRDPARTY

rm -rf $BULLET3_DIR

git clone \
    --depth 1 \
    --branch $BULLET3_TAG \
    https://github.com/$BULLET3_URL \
    $BULLET3_DIR

#
# GLM

cd $DIR_THIRDPARTY

rm -rf $GLM_DIR

git clone \
     --depth 1 \
     --branch $GLM_TAG \
     https://github.com/$GLM_URL \
     $GLM_DIR

#
# TINYOBJ

cd $DIR_THIRDPARTY

rm -rf $TINYOBJ_DIR

git clone \
    --depth 1 \
    --branch $TINYOBJ_TAG \
    https://github.com/$TINYOBJ_URL \
    $TINYOBJ_DIR

