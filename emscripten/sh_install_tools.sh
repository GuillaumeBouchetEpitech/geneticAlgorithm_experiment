
#
##

SDK_VERSION=3.1.26


rm -rf ./tools
mkdir -p ./tools
cd ./tools

git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

./emsdk install $SDK_VERSION
./emsdk activate --embedded $SDK_VERSION

. ./emsdk_env.sh

em++ --clear-cache

##
#
