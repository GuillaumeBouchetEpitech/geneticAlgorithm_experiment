

# cd src

clang-tidy \
  ./src/main.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I./src \
  -I../shared-framework/src \
  -I/usr/include/SDL2/ \
  -DMY_DEFINES \
  ...

