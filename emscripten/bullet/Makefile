

# cpp_mode=yes
cpp_mode=no


ifeq ($(cpp_mode),yes)

	NAME_MAIN=		exec
	NAME_BULLETLINEARMATH=	lib_linearmath.a
	NAME_BULLETCOLLISION=	lib_collision.a
	NAME_BULLETDYNAMICS=	lib_dynamics.a

else

	FOLDER_DIST=	dist/asm.js
	FOLDER_LIB=		lib

	NAME_MAIN=		$(FOLDER_DIST)/index.js
	NAME_WORKER=	$(FOLDER_DIST)/worker.js
	NAME_BULLETLINEARMATH=	$(FOLDER_LIB)/linearmath.bc
	NAME_BULLETCOLLISION=	$(FOLDER_LIB)/collision.bc
	NAME_BULLETDYNAMICS=	$(FOLDER_LIB)/dynamics.bc

endif



#### PATHS

PATH_SRC=		src
PATH_EXTERNAL=	$(PATH_SRC)/externals
PATH_BULLET=	$(PATH_EXTERNAL)/Bullet

# the path to glm is just the path to external
PATH_GLM=		$(PATH_EXTERNAL)

#### /PATHS


OBJDIR_MAIN = obj/main
OBJDIR_WORKER = obj/worker
OBJDIR_BULLET=	obj/bullet
OBJDIR_BULLETLINEARMATH=	$(OBJDIR_BULLET)/linearmath
OBJDIR_BULLETCOLLISION=	$(OBJDIR_BULLET)/collision
OBJDIR_BULLETDYNAMICS=	$(OBJDIR_BULLET)/dynamic



#### SRC

SRC_MAIN=	$(wildcard	$(PATH_SRC)/*.cpp \
						$(PATH_SRC)/States/*.cpp \
						$(PATH_SRC)/Utility/*.cpp \
						$(PATH_SRC)/Logic/*.cpp \
						$(PATH_SRC)/Logic/Graphic/*.cpp \
						$(PATH_SRC)/Simulation/*.cpp \
						$(PATH_SRC)/Simulation/Circuit/*.cpp \
						$(PATH_SRC)/Simulation/Utility/*.cpp \
						$(PATH_SRC)/Simulation/AI/GeneticAlgorithm/*.cpp \
						$(PATH_SRC)/Simulation/AI/NeuralNetwork/*.cpp \
						$(PATH_SRC)/Simulation/AI/Utility/*.cpp \
						$(PATH_SRC)/Simulation/Worker/Client/*.cpp \
						$(PATH_SRC)/Simulation/Worker/*.cpp)

MAIN_OBJ=	$(patsubst %.cpp, $(OBJDIR_MAIN)/%.o, $(SRC_MAIN))

###

SRC_WORKER=	$(wildcard	$(PATH_SRC)/Simulation/Worker/*.cpp \
						$(PATH_SRC)/Simulation/Worker/Server/*.cpp \
						$(PATH_SRC)/Simulation/Circuit/*.cpp \
						$(PATH_SRC)/Simulation/Utility/*.cpp \
						$(PATH_SRC)/Simulation/AI/NeuralNetwork/*.cpp \
						$(PATH_SRC)/Simulation/AI/Utility/*.cpp \
						$(PATH_SRC)/Utility/*.cpp \
						$(PATH_SRC)/Logic/Physic/*.cpp)

WORKER_OBJ=	$(patsubst %.cpp, $(OBJDIR_WORKER)/%.o, $(SRC_WORKER))

#### /SRC


#### BULLET_SRC

#

#

#

SRC_BULLETLINEARMATH=	$(wildcard	$(PATH_BULLET)/LinearMath/*.cpp)

OBJ_BULLETLINEARMATH=	$(patsubst %.cpp, $(OBJDIR_BULLETLINEARMATH)/%.o, $(SRC_BULLETLINEARMATH))

#

SRC_BULLETCOLLISION=	$(wildcard	$(PATH_BULLET)/BulletCollision/BroadphaseCollision/*.cpp \
									$(PATH_BULLET)/BulletCollision/CollisionShapes/*.cpp \
									$(PATH_BULLET)/BulletCollision/CollisionDispatch/*.cpp \
									$(PATH_BULLET)/BulletCollision/NarrowPhaseCollision/*.cpp \
									$(PATH_BULLET)/BulletCollision/Gimpact/*.cpp)

OBJ_BULLETCOLLISION=	$(patsubst %.cpp, $(OBJDIR_BULLETCOLLISION)/%.o, $(SRC_BULLETCOLLISION))

#

SRC_BULLETDYNAMICS=	$(wildcard	$(PATH_BULLET)/BulletDynamics/Character/*.cpp \
								$(PATH_BULLET)/BulletDynamics/ConstraintSolver/*.cpp \
								$(PATH_BULLET)/BulletDynamics/Dynamics/*.cpp \
								$(PATH_BULLET)/BulletDynamics/Vehicle/*.cpp \
								$(PATH_BULLET)/BulletDynamics/ConstraintSolver/*.cpp)

OBJ_BULLETDYNAMICS=		$(patsubst %.cpp, $(OBJDIR_BULLETDYNAMICS)/%.o, $(SRC_BULLETDYNAMICS))

#

#

#### /BULLET_SRC



#######


ifeq ($(cpp_mode),yes)

	# CXX=g++
	# AR=ar

	# # CXXFLAGS=	-Wall -W -Wextra -Wunused -O3 -std=c++11	\

	# CXXFLAGS=	-O3 -std=c++11	\
	# 			-I$(PATH_SRC) \
	# 			-I$(PATH_GLM) \
	# 			-I$(PATH_BULLET)

	# CXXFLAGS_BULLET=	-O3 -std=c++11	\
	# 					-I$(PATH_SRC) \
	# 					-I$(PATH_BULLET)


	# LDFLAGS=	-O3 \
	# 			-lSDL2_image -lSDL2 -lGLESv2 -lEGL \
	# 			$(NAME_BULLETDYNAMICS) \
	# 			$(NAME_BULLETCOLLISION) \
	# 			$(NAME_BULLETLINEARMATH) \
	# 			-pthread

else

	EMSCRIPTEN_ROOT=/home/barbie/Documents/Programming/cpp/emscripten/emsdk_portable/emscripten/1.38.0

	FLAGS_RENDERING=	-s USE_SDL=2 -s USE_SDL_IMAGE=2
	FLAGS_MEMORY=	-s TOTAL_MEMORY=67108864
	FLAGS_ASSETS=	--use-preload-plugins --preload-file ./assets/

	### those flags are left unused on purpose
	### the "SharedArrayBuffer" feature is disabled everywhere for secutiry reasons (since Jan 2018)
	### this feature is requried for the browser emulation of the C++ multithreading to work
	# FLAGS_THREADING=	-s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s PTHREAD_HINT_NUM_CORES=4

	FLAGS_WORKER=	-s EXPORTED_FUNCTIONS="['_onMessage']" -s BUILD_AS_WORKER=1


	CXX=$(EMSCRIPTEN_ROOT)/em++
	AR=$(EMSCRIPTEN_ROOT)/emar

	# CXXFLAGS=	-Wall -W -Wextra -Wunused -O3 -std=c++11

	CXXFLAGS_MAIN=	-O3 -std=c++11 \
					$(FLAGS_RENDERING) \
					-I$(PATH_SRC) \
					-I$(PATH_GLM)

	CXXFLAGS_WORKER=	-O3 -std=c++11 \
						-I$(PATH_SRC) \
						-I$(PATH_GLM) \
						-I$(PATH_BULLET)

	CXXFLAGS_BULLET=	-O3 -std=c++11 \
						-I$(PATH_SRC) \
						-I$(PATH_BULLET)

	MAIN_LDFLAGS=	-O3 \
					$(FLAGS_RENDERING) \
					$(FLAGS_MEMORY) \
					$(FLAGS_ASSETS)

	WORKER_LDFLAGS=	-O3 \
					$(FLAGS_WORKER) \
					$(FLAGS_MEMORY) \
					$(FLAGS_ASSETS) \
					\
					$(NAME_BULLETDYNAMICS) \
					$(NAME_BULLETCOLLISION) \
					$(NAME_BULLETLINEARMATH)

endif


RM=			rm -f


#######


# all:		cpp

all:		main worker

main:		$(MAIN_OBJ)
			@mkdir -p `dirname $(NAME_MAIN)`
			$(CXX) $(CXXFLAGS_MAIN) $(MAIN_OBJ) -o $(NAME_MAIN) $(MAIN_LDFLAGS)

worker:		$(WORKER_OBJ)
			@mkdir -p `dirname $(NAME_WORKER)`
			$(CXX) $(CXXFLAGS_WORKER) $(WORKER_OBJ) -o $(NAME_WORKER) $(WORKER_LDFLAGS)

bullet:		linearmath collision dynamics

linearmath:	$(OBJ_BULLETLINEARMATH)
			$(AR) cr $(NAME_BULLETLINEARMATH) $(OBJ_BULLETLINEARMATH)

collision:	$(OBJ_BULLETCOLLISION)
			$(AR) cr $(NAME_BULLETCOLLISION) $(OBJ_BULLETCOLLISION)

dynamics:	$(OBJ_BULLETDYNAMICS)
			$(AR) cr $(NAME_BULLETDYNAMICS) $(OBJ_BULLETDYNAMICS)

#

# this will ensure the "obj" folder and compile the "cpp" file into ".o" file

$(OBJDIR_MAIN)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_MAIN) $< -c -o $@

$(OBJDIR_WORKER)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_WORKER) $< -c -o $@

$(OBJDIR_BULLETLINEARMATH)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

$(OBJDIR_BULLETCOLLISION)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

$(OBJDIR_BULLETDYNAMICS)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

#

clean:
			$(RM) $(MAIN_OBJ)
			$(RM) $(WORKER_OBJ)

fclean:		clean
			$(RM) $(NAME_MAIN)
			$(RM) $(NAME_WORKER)

clean_bullet:
				$(RM) $(OBJ_BULLETLINEARMATH)
				$(RM) $(OBJ_BULLETCOLLISION)
				$(RM) $(OBJ_BULLETDYNAMICS)

fclean_bullet:	clean_bullet
				$(RM) $(NAME_BULLETLINEARMATH)
				$(RM) $(NAME_BULLETCOLLISION)
				$(RM) $(NAME_BULLETDYNAMICS)

re:			fclean all

.PHONY:		all main worker clean fclean re bullet linearmath collision dynamics clean_bullet fclean_bullet
