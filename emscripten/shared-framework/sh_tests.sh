

reset

# make build_mode="debug" build_platform="native" framework fclean
make build_mode="debug" build_platform="native" framework -j6

make build_mode="debug" build_platform="native" fclean_tests
make build_mode="debug" build_platform="native" build-tests -j6

# ./bin/exec
valgrind --leak-check=full ./bin/exec
