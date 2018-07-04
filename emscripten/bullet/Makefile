

cpp_mode=yes
# cpp_mode=no


ifeq ($(cpp_mode),yes)

	NAME=		exec
	BULLETLINEARMATH_NAME=	lib_linearmath.a
	BULLETCOLLISION_NAME=	lib_collision.a
	BULLETDYNAMICS_NAME=	lib_dynamics.a

else

	NAME=		index.js
	BULLETLINEARMATH_NAME=	lib_linearmath.bc
	BULLETCOLLISION_NAME=	lib_collision.bc
	BULLETDYNAMICS_NAME=	lib_dynamics.bc

endif



#### PATHS

PATH_SRC=		src
PATH_EXTERNAL=	$(PATH_SRC)/externals
PATH_BULLET=	$(PATH_EXTERNAL)/Bullet

# the path to glm is just the path to external
PATH_GLM=		$(PATH_EXTERNAL)

#### /PATHS




#### SRC

SRC=	$(wildcard	$(PATH_SRC)/*.cpp \
					$(PATH_SRC)/States/*.cpp \
					$(PATH_SRC)/Utility/*.cpp \
					$(PATH_SRC)/Logic/*.cpp \
					$(PATH_SRC)/Logic/Graphic/*.cpp \
					$(PATH_SRC)/Logic/Graphic/Color/*.cpp \
					$(PATH_SRC)/Logic/Graphic/Inst/*.cpp \
					$(PATH_SRC)/Logic/Physic/*.cpp \
					$(PATH_SRC)/Logic/Threading/*.cpp \
					$(PATH_SRC)/Logic/Wrapper/*.cpp \
					$(PATH_SRC)/Simulation/*.cpp \
					$(PATH_SRC)/Simulation/Utility/*.cpp \
					$(PATH_SRC)/Simulation/AI/*.cpp \
					$(PATH_SRC)/Simulation/AI/Utility/*.cpp)

OBJ=	$(patsubst %.cpp, obj/%.o, $(SRC))

#### /SRC


#### BULLET_SRC

#

#

BULLET_OBJDIR=	bullet_obj

#

BULLETLINEARMATH_SRC=	$(wildcard	$(PATH_BULLET)/LinearMath/*.cpp)

BULLETLINEARMATH_OBJDIR=	$(BULLET_OBJDIR)/linearmath
BULLETLINEARMATH_OBJ=	$(patsubst %.cpp, $(BULLETLINEARMATH_OBJDIR)/%.o, $(BULLETLINEARMATH_SRC))

#

BULLETCOLLISION_SRC=	$(wildcard	$(PATH_BULLET)/BulletCollision/*.cpp \
									$(PATH_BULLET)/BulletCollision/BroadphaseCollision/*.cpp \
									$(PATH_BULLET)/BulletCollision/CollisionShapes/*.cpp \
									$(PATH_BULLET)/BulletCollision/CollisionDispatch/*.cpp \
									$(PATH_BULLET)/BulletCollision/NarrowPhaseCollision/*.cpp \
									$(PATH_BULLET)/BulletCollision/Gimpact/*.cpp)

BULLETCOLLISION_OBJDIR=	$(BULLET_OBJDIR)/collision
BULLETCOLLISION_OBJ=	$(patsubst %.cpp, $(BULLETCOLLISION_OBJDIR)/%.o, $(BULLETCOLLISION_SRC))

#

BULLETDYNAMICS_SRC=	$(wildcard	$(PATH_BULLET)/BulletDynamics/*.cpp \
								$(PATH_BULLET)/BulletDynamics/Character/*.cpp \
								$(PATH_BULLET)/BulletDynamics/ConstraintSolver/*.cpp \
								$(PATH_BULLET)/BulletDynamics/Dynamics/*.cpp \
								$(PATH_BULLET)/BulletDynamics/Vehicle/*.cpp \
								$(PATH_BULLET)/BulletDynamics/ConstraintSolver/*.cpp)

BULLETDYNAMICS_OBJDIR=	$(BULLET_OBJDIR)/dynamic
BULLETDYNAMICS_OBJ=		$(patsubst %.cpp, $(BULLETDYNAMICS_OBJDIR)/%.o, $(BULLETDYNAMICS_SRC))

#

#

#### /BULLET_SRC



#######


ifeq ($(cpp_mode),yes)

	CXX=g++
	AR=ar

	# CXXFLAGS=	-Wall -W -Wextra -Wunused -O3 -std=c++11	\

	CXXFLAGS=	-O3 -std=c++11	\
				-I$(PATH_SRC) \
				-I$(PATH_GLM) \
				-I$(PATH_BULLET)

	CXXFLAGS_BULLET=	-O3 -std=c++11	\
						-I$(PATH_SRC) \
						-I$(PATH_BULLET)


	LDFLAGS=	-O3 \
				-lSDL2_image -lSDL2 -lGLESv2 -lEGL \
				$(BULLETDYNAMICS_NAME) \
				$(BULLETCOLLISION_NAME) \
				$(BULLETLINEARMATH_NAME) \
				-pthread

else

	# CXX=/home/barbie/Documents/Programming/cpp/emscripten/emsdk_portable/emscripten/master/em++
	# AR=/home/barbie/Documents/Programming/cpp/emscripten/emsdk_portable/emscripten/master/emar
	CXX=/home/barbie/Documents/Programming/cpp/emscripten/emsdk_portable/emscripten/1.37.22/em++
	AR=/home/barbie/Documents/Programming/cpp/emscripten/emsdk_portable/emscripten/1.37.22/emar

	# CXXFLAGS=	-Wall -W -Wextra -Wunused -O3 -std=c++11	\

	# CXXFLAGS=	-O3 -std=c++11
	CXXFLAGS=	-O3 -std=c++11	\
				-s USE_SDL=2 -s USE_SDL_IMAGE=2 \
				-I$(PATH_SRC) \
				-I$(PATH_GLM) \
				-I$(PATH_BULLET)

	CXXFLAGS_BULLET=	-O3 -std=c++11	\
						-I$(PATH_SRC) \
						-I$(PATH_BULLET)

	LDFLAGS=	-O3 \
				-s USE_BULLET=0 -s USE_SDL=2 -s USE_SDL_IMAGE=2 \
				-s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4 -s PTHREAD_HINT_NUM_CORES=4 \
				-s TOTAL_MEMORY=67108864 \
				--use-preload-plugins --preload-file ./assets/ \
				$(EXTERNAL) \
				$(BULLETDYNAMICS_NAME) \
				$(BULLETCOLLISION_NAME) \
				$(BULLETLINEARMATH_NAME)

				# -s FULL_ES2=1	\

endif


RM=			rm -f


#######


# all:		cpp

all:		$(OBJ)
			$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS)

bullet:		linearmath collision dynamics

linearmath:	$(BULLETLINEARMATH_OBJ)
			$(AR) cr $(BULLETLINEARMATH_NAME) $(BULLETLINEARMATH_OBJ)

collision:	$(BULLETCOLLISION_OBJ)
			$(AR) cr $(BULLETCOLLISION_NAME) $(BULLETCOLLISION_OBJ)

dynamics:	$(BULLETDYNAMICS_OBJ)
			$(AR) cr $(BULLETDYNAMICS_NAME) $(BULLETDYNAMICS_OBJ)

#

# for every ".cpp" file, ensure the "obj" folder and compile in a ".o" file
obj/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $< -c -o $@

# for every ".cpp" file, ensure the "obj" folder and compile in a ".o" file
$(BULLETLINEARMATH_OBJDIR)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

# for every ".cpp" file, ensure the "obj" folder and compile in a ".o" file
$(BULLETCOLLISION_OBJDIR)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

# for every ".cpp" file, ensure the "obj" folder and compile in a ".o" file
$(BULLETDYNAMICS_OBJDIR)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

#

clean:
			$(RM) $(OBJ)

fclean:		clean
			$(RM) $(NAME)

clean_bullet:
			$(RM) $(BULLETLINEARMATH_OBJ)
			$(RM) $(BULLETCOLLISION_OBJ)
			$(RM) $(BULLETDYNAMICS_OBJ)

fclean_bullet:	clean_bullet
				$(RM) $(BULLETLINEARMATH_NAME)
				$(RM) $(BULLETCOLLISION_NAME)
				$(RM) $(BULLETDYNAMICS_NAME)

re:			fclean all

.PHONY:		all clean fclean re bullet linearmath collision dynamics clean_bullet fclean_bullet
