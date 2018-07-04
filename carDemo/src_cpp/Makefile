

NAME=		exec


#### MAIN

PATH_MAIN=	./src
MAIN=		$(PATH_MAIN)/main.cpp	\
			$(PATH_MAIN)/Experiment/Experiment.cpp	\
			$(PATH_MAIN)/Experiment/Renderer/RendererSFML.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/Simulation.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/utils/t_line.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/utils/t_vec2f.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/utils/collision.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/utils/utils.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/utils/randomiser.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/utils/TraceLogger.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/ai/GeneticAlgorithm.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/ai/NeuralNetwork.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/ai/NeuralNetworkTopology.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/logic/Car.cpp	\
			$(PATH_MAIN)/Experiment/Simulation/logic/Circuit.cpp


#### /MAIN



#### SFML

SFML_VERSION=	2.4.2
# SFML_PATH=		/home/barbie/Documents/Programming/Library/sfml/SFML-$(SFML_VERSION)
SFML_PATH=		/home/barbie/Documents/Programming/Library/sfml/sources/SFML-$(SFML_VERSION)
SFML_INC_PATH=	$(SFML_PATH)/include
SFML_LIB_PATH=	$(SFML_PATH)/lib
SFML_LIB=		-Wl,--rpath=$(SFML_LIB_PATH)							\
				$(SFML_LIB_PATH)/libsfml-graphics.so.$(SFML_VERSION)	\
				$(SFML_LIB_PATH)/libsfml-window.so.$(SFML_VERSION)		\
				$(SFML_LIB_PATH)/libsfml-system.so.$(SFML_VERSION)

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
