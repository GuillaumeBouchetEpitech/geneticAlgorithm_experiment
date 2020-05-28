
#
##

SDK_VERSION=sdk-1.39.14
# SDK_VERSION=sdk-1.39.4
# SDK_VERSION=sdk-1.39.3
# SDK_VERSION=sdk-1.39.2


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
