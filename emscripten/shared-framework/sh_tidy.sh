

# cd src

# clang-tidy \
#   ./src/main.cpp \
#   -extra-arg=-std=c++17 \
#   -- \
#   -I./src \
#   -I../shared-framework/src \
#   -I/usr/include/SDL2/ \
#   -DMY_DEFINES \
#   ...


clang-tidy \
  ./src/framework/system/*.cpp \
  ./src/framework/graphic/*.cpp \
  ./src/framework/physic/*.cpp \
  ./src/framework/audio/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./src/ \
  -I/usr/include/SDL2/ \
  -DMY_DEFINES \
  ...

