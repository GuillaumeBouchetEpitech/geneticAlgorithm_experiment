

NAME=		exec


#### MAIN

PATH_MAIN=	./src
MAIN=		$(PATH_MAIN)/main.cpp	\
			$(PATH_MAIN)/Model/Simulation.cpp	\
			$(PATH_MAIN)/Model/TraceLogger.cpp	\
			$(PATH_MAIN)/Model/AI.cpp

#### /MAIN



#### SFML

SFML_PATH=		/home/barbie/Documents/Programming/Libraries/git/SFML-2.1
SFML_INC_PATH=	$(SFML_PATH)/include
SFML_LIB_PATH=	$(SFML_PATH)/lib
SFML_LIB=		-Wl,--rpath=$(SFML_LIB_PATH)				\
				$(SFML_LIB_PATH)/libsfml-graphics.so.2.1	\
				$(SFML_LIB_PATH)/libsfml-window.so.2.1		\
				$(SFML_LIB_PATH)/libsfml-system.so.2.1

#### /SFML


####

SRC=		$(MAIN)

OBJ=		$(SRC:.cpp=.o)

#######


# CXXFLAGS=	-Wall -W -Wextra -Wunused -g3 -std=c++11	\

CXXFLAGS=	-Wall -W -Wextra -Wunused -O3 -std=c++11	\
			-fopenmp									\
			-I./										\
			-I$(SFML_INC_PATH)

LDFLAGS=	$(SFML_LIB) -lGL -lGLU -lGLEW				\
			-fopenmp

#######

CXX=		g++
RM=			rm -f


#######


$(NAME):	$(OBJ)
			$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS)

all:		$(NAME)

clean:
			$(RM) $(OBJ)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
