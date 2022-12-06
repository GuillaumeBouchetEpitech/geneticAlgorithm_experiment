

# clang-format *.cpp *.h *.hpp
#clang-format -i -style=WebKit ./src/*.cpp *.h *.hpp

#clang-format -i -style=WebKit *.cpp *.h

# find ./src/ -iname *.h -iname *.hpp -o -iname *.cpp | xargs clang-format -i

find ./src/ -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
