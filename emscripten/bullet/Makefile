
# target_mode=native_pthread
# target_mode=web_asmjs_webworker
target_mode=web_asmjs_pthread

#

ifeq ($(target_mode),native_pthread)

$(info target_mode is valid, value=$(target_mode))

else ifeq ($(target_mode),web_asmjs_pthread)

$(info target_mode is valid, value=$(target_mode))

else ifeq ($(target_mode),web_asmjs_webworker)

$(info target_mode is valid, value=$(target_mode))

else

$(error unsupported value for "target_mode", value=$(target_mode))

endif

#

ifeq ($(target_mode),native_pthread)

	DIR_TARGET=	./bin
	DIR_LIB=	./lib/native

	NAME_NATIVE=			$(DIR_TARGET)/exec
	NAME_BULLET_LINEARMATH=	$(DIR_LIB)/lib_linearmath.a
	NAME_BULLET_COLLISION=	$(DIR_LIB)/lib_collision.a
	NAME_BULLET_DYNAMICS=	$(DIR_LIB)/lib_dynamics.a

else ifeq ($(target_mode),web_asmjs_pthread)

	DIR_TARGET=	./dist/asm.js/pthread
	DIR_LIB=	./lib/web

	NAME_WEB_PTHREAD=		$(DIR_TARGET)/index.js
	NAME_BULLET_LINEARMATH=	$(DIR_LIB)/lib_linearmath.bc
	NAME_BULLET_COLLISION=	$(DIR_LIB)/lib_collision.bc
	NAME_BULLET_DYNAMICS=	$(DIR_LIB)/lib_dynamics.bc

else ifeq ($(target_mode),web_asmjs_webworker)

	DIR_TARGET=	./dist/asm.js/webworker
	DIR_LIB=	./lib/web

	NAME_WEB_MAIN=			$(DIR_TARGET)/index.js
	NAME_WEB_WORKER=		$(DIR_TARGET)/worker.js
	NAME_BULLET_LINEARMATH=	$(DIR_LIB)/lib_linearmath.bc
	NAME_BULLET_COLLISION=	$(DIR_LIB)/lib_collision.bc
	NAME_BULLET_DYNAMICS=	$(DIR_LIB)/lib_dynamics.bc

endif



#### DIRS

DIR_SRC=		src
DIR_THIRDPARTY=	$(DIR_SRC)/thirdparty
DIR_BULLET=		$(DIR_THIRDPARTY)/bullet3/src

#### /DIRS



ifeq ($(target_mode),native_pthread)

DIR_OBJ=			./obj/native
OBJDIR_NATIVE=		$(DIR_OBJ)

else ifeq ($(target_mode),web_asmjs_pthread)

DIR_OBJ=			./obj/web
OBJDIR_WEB_PTHREAD=	$(DIR_OBJ)/pthread

else ifeq ($(target_mode),web_asmjs_webworker)

DIR_OBJ=			./obj/web
OBJDIR_WEB_MAIN=	$(DIR_OBJ)/main
OBJDIR_WEB_WORKER=	$(DIR_OBJ)/worker

endif

OBJDIR_BULLET=	$(DIR_OBJ)/bullet
OBJDIR_BULLET_LINEARMATH=	$(OBJDIR_BULLET)/linearmath
OBJDIR_BULLET_COLLISION=	$(OBJDIR_BULLET)/collision
OBJDIR_BULLET_DYNAMICS=		$(OBJDIR_BULLET)/dynamic



#### SRC

ifeq ($(target_mode),native_pthread)

# common src
SRC_NATIVE=			$(wildcard \
						$(DIR_SRC)/*.cpp \
						$(DIR_SRC)/demo/*.cpp \
						$(DIR_SRC)/demo/states/*.cpp \
						$(DIR_SRC)/demo/utilities/*.cpp \
						$(DIR_SRC)/demo/utilities/sdl/*.cpp \
						$(DIR_SRC)/demo/utilities/math/*.cpp \
						$(DIR_SRC)/demo/utilities/message/*.cpp \
						$(DIR_SRC)/demo/utilities/string/*.cpp \
						$(DIR_SRC)/demo/utilities/threading/*.cpp \
						$(DIR_SRC)/demo/logic/*.cpp \
						$(DIR_SRC)/demo/logic/graphic/*.cpp \
						$(DIR_SRC)/demo/logic/graphic/wrappers/*.cpp \
						$(DIR_SRC)/demo/logic/physic/*.cpp \
						$(DIR_SRC)/demo/simulation/*.cpp \
						$(DIR_SRC)/demo/simulation/pthread/*.cpp \
						$(DIR_SRC)/demo/simulation/logic/*.cpp \
						$(DIR_SRC)/demo/simulation/machineLearning/*.cpp)

else ifeq ($(target_mode),web_asmjs_pthread)

# common src
SRC_WEB_PTHREAD=	$(wildcard \
						$(DIR_SRC)/*.cpp \
						$(DIR_SRC)/demo/*.cpp \
						$(DIR_SRC)/demo/states/*.cpp \
						$(DIR_SRC)/demo/utilities/*.cpp \
						$(DIR_SRC)/demo/utilities/sdl/*.cpp \
						$(DIR_SRC)/demo/utilities/math/*.cpp \
						$(DIR_SRC)/demo/utilities/message/*.cpp \
						$(DIR_SRC)/demo/utilities/string/*.cpp \
						$(DIR_SRC)/demo/utilities/threading/*.cpp \
						$(DIR_SRC)/demo/logic/*.cpp \
						$(DIR_SRC)/demo/logic/graphic/*.cpp \
						$(DIR_SRC)/demo/logic/graphic/wrappers/*.cpp \
						$(DIR_SRC)/demo/logic/physic/*.cpp \
						$(DIR_SRC)/demo/simulation/*.cpp \
						$(DIR_SRC)/demo/simulation/pthread/*.cpp \
						$(DIR_SRC)/demo/simulation/logic/*.cpp \
						$(DIR_SRC)/demo/simulation/machineLearning/*.cpp)

else ifeq ($(target_mode),web_asmjs_webworker)

SRC_WEB_MAIN=	$(wildcard \
					$(DIR_SRC)/*.cpp \
					$(DIR_SRC)/demo/*.cpp \
					$(DIR_SRC)/demo/states/*.cpp \
					$(DIR_SRC)/demo/utilities/*.cpp \
					$(DIR_SRC)/demo/utilities/sdl/*.cpp \
					$(DIR_SRC)/demo/utilities/math/*.cpp \
					$(DIR_SRC)/demo/utilities/message/*.cpp \
					$(DIR_SRC)/demo/utilities/string/*.cpp \
					$(DIR_SRC)/demo/logic/*.cpp \
					$(DIR_SRC)/demo/logic/graphic/*.cpp \
					$(DIR_SRC)/demo/logic/graphic/wrappers/*.cpp \
					$(DIR_SRC)/demo/simulation/*.cpp \
					$(DIR_SRC)/demo/simulation/logic/*.cpp \
					$(DIR_SRC)/demo/simulation/machineLearning/*.cpp \
					$(DIR_SRC)/demo/simulation/webworker/*.cpp \
					$(DIR_SRC)/demo/simulation/webworker/producer/*.cpp)

SRC_WEB_WORKER=	$(wildcard \
					$(DIR_SRC)/demo/utilities/*.cpp \
					$(DIR_SRC)/demo/utilities/math/*.cpp \
					$(DIR_SRC)/demo/utilities/message/*.cpp \
					$(DIR_SRC)/demo/utilities/string/*.cpp \
					$(DIR_SRC)/demo/logic/physic/*.cpp \
					$(DIR_SRC)/demo/simulation/logic/*.cpp \
					$(DIR_SRC)/demo/simulation/machineLearning/*.cpp \
					$(DIR_SRC)/demo/simulation/webworker/consumer/*.cpp)

endif

SRC_BULLET_LINEARMATH=	$(wildcard \
							$(DIR_BULLET)/LinearMath/*.cpp)

SRC_BULLET_COLLISION=	$(wildcard \
							$(DIR_BULLET)/BulletCollision/BroadphaseCollision/*.cpp \
							$(DIR_BULLET)/BulletCollision/CollisionShapes/*.cpp \
							$(DIR_BULLET)/BulletCollision/CollisionDispatch/*.cpp \
							$(DIR_BULLET)/BulletCollision/NarrowPhaseCollision/*.cpp \
							$(DIR_BULLET)/BulletCollision/Gimpact/*.cpp)

SRC_BULLET_DYNAMICS=	$(wildcard \
							$(DIR_BULLET)/BulletDynamics/Character/*.cpp \
							$(DIR_BULLET)/BulletDynamics/Dynamics/*.cpp \
							$(DIR_BULLET)/BulletDynamics/Vehicle/*.cpp \
							$(DIR_BULLET)/BulletDynamics/ConstraintSolver/*.cpp)

#

ifeq ($(target_mode),native_pthread)

OBJ_NATIVE=		$(patsubst %.cpp, \
					$(OBJDIR_NATIVE)/%.o, \
					$(SRC_NATIVE))

else ifeq ($(target_mode),web_asmjs_pthread)

OBJ_WEB_PTHREAD=	$(patsubst %.cpp, \
						$(OBJDIR_WEB_PTHREAD)/%.o, \
						$(SRC_WEB_PTHREAD))

else ifeq ($(target_mode),web_asmjs_webworker)

OBJ_WEB_MAIN=	$(patsubst %.cpp, \
					$(OBJDIR_WEB_MAIN)/%.o, \
					$(SRC_WEB_MAIN))

OBJ_WEB_WORKER=	$(patsubst %.cpp, \
					$(OBJDIR_WEB_WORKER)/%.o, \
					$(SRC_WEB_WORKER))

endif

OBJ_BULLET_LINEARMATH=	$(patsubst %.cpp, \
							$(OBJDIR_BULLET_LINEARMATH)/%.o, \
							$(SRC_BULLET_LINEARMATH))

OBJ_BULLET_COLLISION=	$(patsubst %.cpp, \
							$(OBJDIR_BULLET_COLLISION)/%.o, \
							$(SRC_BULLET_COLLISION))

OBJ_BULLET_DYNAMICS=	$(patsubst %.cpp, \
							$(OBJDIR_BULLET_DYNAMICS)/%.o, \
							$(SRC_BULLET_DYNAMICS))

#

#

#### /BULLET_SRC



#######


OPTIMISATION_FLAG=	-O3
# OPTIMISATION_FLAG=	-g3

ifeq ($(target_mode),native_pthread)

	CXX=g++
	AR=ar

	CXXFLAGS_NATIVE=	$(OPTIMISATION_FLAG) \
						-std=c++14 \
						-Wall -W -Wextra -Wunused \
						-I$(DIR_SRC) \
						-I$(DIR_BULLET) \
						\
						-DD_CIRCUIT_FILENAME='"assets/circuits/default.txt"'

	LDFLAGS_NATIVE=		$(OPTIMISATION_FLAG) \
						-lSDL2_image \
						-lSDL2 \
						-lGLESv2 \
						-lEGL \
						-pthread \
						\
						$(NAME_BULLET_DYNAMICS) \
						$(NAME_BULLET_COLLISION) \
						$(NAME_BULLET_LINEARMATH)

	CXXFLAGS_BULLET=	$(OPTIMISATION_FLAG) \
						-std=c++14 \
						-I$(DIR_BULLET)

else ifeq ($(target_mode),web_asmjs_pthread)

	CXX=em++
	AR=emar

	COMMON_FLAGS=	$(OPTIMISATION_FLAG) \
					-s USE_SDL=2 \
					-s USE_SDL_IMAGE=2 \
					-s SDL2_IMAGE_FORMATS='["png"]' \
					-s FULL_ES3=1 \
					-s USE_WEBGL2=1 \
					-s GL_PREINITIALIZED_CONTEXT=1

	### those flags are left unused on purpose
	### the "SharedArrayBuffer" feature is disabled everywhere for secutiry reasons (since Jan 2018)
	### this feature is required for the browser emulation of the C++ multithreading to work
	COMMON_FLAGS+=	-s USE_PTHREADS=1 \
					-s PTHREAD_POOL_SIZE=4 \
					-s PTHREAD_HINT_NUM_CORES=4

	CXXFLAGS_WEB_PTHREAD=	$(COMMON_FLAGS) \
							-std=c++14 \
							-Wall -W -Wextra -Wunused \
							-I$(DIR_SRC) \
							-I$(DIR_BULLET) \
							\
							-DD_CIRCUIT_FILENAME='"assets/circuits/default.txt"'

	LDFLAGS_WEB_PTHREAD=	$(COMMON_FLAGS) \
							\
							$(NAME_BULLET_DYNAMICS) \
							$(NAME_BULLET_COLLISION) \
							$(NAME_BULLET_LINEARMATH)

	# value must be a power of 2 (16, 32, 64, etc.).
	# suffixes must be one of those: KB, MB, GB, TB.
	# Compile with ALLOW_MEMORY_GROWTH if you want a growable heap (slower though).
	LDFLAGS_WEB_PTHREAD+=	-s TOTAL_MEMORY=64Mb

	# assets loading
	# => This cannot be used with pthread
	# LDFLAGS_WEB_PTHREAD+=	--use-preload-plugins --preload-file ./assets/
	# => This works
	LDFLAGS_WEB_PTHREAD+=	--embed-file ./assets/

	# disable the internal emscripten version (is this still useful?)
	LDFLAGS_WEB_PTHREAD+=	-s USE_BULLET=0

	# no wasm, only asm.js for now
	LDFLAGS_WEB_PTHREAD+=	-s WASM=0

	# Link Time Optimisation (LTO)
	LDFLAGS_WEB_PTHREAD+=	--llvm-lto 1

	# # final file size optimisation
	# LDFLAGS_WEB_PTHREAD+=	-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
	# 						-s ELIMINATE_DUPLICATE_FUNCTIONS=1

	CXXFLAGS_BULLET=	$(OPTIMISATION_FLAG) \
						-std=c++14 \
						-I$(DIR_BULLET)

else ifeq ($(target_mode),web_asmjs_webworker)

	CXX=em++
	AR=emar

	COMMON_LDFLAGS=		$(OPTIMISATION_FLAG)

	# value must be a power of 2 (16, 32, 64, etc.).
	# suffixes must be one of those: KB, MB, GB, TB.
	# Compile with ALLOW_MEMORY_GROWTH if you want a growable heap (slower though).
	COMMON_LDFLAGS+=	-s TOTAL_MEMORY=32Mb

	# assets loading
	# => This cannot be used with pthread
	# COMMON_LDFLAGS+=	--use-preload-plugins --preload-file ./assets/
	# => This works
	COMMON_LDFLAGS+=	--embed-file ./assets/

	# # final file size optimisation
	# COMMON_LDFLAGS+=	-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
	# 					-s ELIMINATE_DUPLICATE_FUNCTIONS=1

	# Link Time Optimisation (LTO)
	COMMON_LDFLAGS+=	--llvm-lto 1

	# disable the internal emscripten version (is this still useful?)
	COMMON_LDFLAGS+=	-s USE_BULLET=0

	# no wasm, only asm.js for now
	COMMON_LDFLAGS+=	-s WASM=0

	CXXFLAGS_COMMON=	$(OPTIMISATION_FLAG) \
						-std=c++14 \
						-Wall -W -Wextra -Wunused \
						-I$(DIR_SRC) \
						\
						-DD_CIRCUIT_FILENAME='"assets/circuits/default.txt"' \
						-DD_WORKER_SCRIPT_URL='"asm.js/webworker/worker.js"' \
						-DD_WORKER_MAIN='"onMessage"'

	FLAGS_WEB_MAIN=		-s USE_SDL=2 \
						-s USE_SDL_IMAGE=2 \
						-s SDL2_IMAGE_FORMATS='["png"]' \
						-s FULL_ES3=1 \
						-s USE_WEBGL2=1 \
						-s GL_PREINITIALIZED_CONTEXT=1

	FLAGS_WEB_WORKER=	-s BUILD_AS_WORKER=1 \
						\
						$(NAME_BULLET_DYNAMICS) \
						$(NAME_BULLET_COLLISION) \
						$(NAME_BULLET_LINEARMATH)

	CXXFLAGS_WEB_MAIN=	$(CXXFLAGS_COMMON) \
						$(FLAGS_WEB_MAIN)

	CXXFLAGS_WEB_WORKER=	$(CXXFLAGS_COMMON) \
							-I$(DIR_BULLET)

	CXXFLAGS_BULLET=	$(CXXFLAGS_COMMON) \
						-I$(DIR_BULLET)

	LDFLAGS_WEB_MAIN=	$(COMMON_LDFLAGS) \
						$(FLAGS_WEB_MAIN)

	LDFLAGS_WEB_WORKER=	$(COMMON_LDFLAGS) \
						$(FLAGS_WEB_WORKER)

endif


RM=			rm -rf


#######

#
## RULE(S)

ifeq ($(target_mode),native_pthread)

all:	native_pthread

else ifeq ($(target_mode),web_asmjs_pthread)

all:	web_pthread

else ifeq ($(target_mode),web_asmjs_webworker)

all:	web_main web_worker

endif

ensurefolders:
				@mkdir -p $(DIR_TARGET)
				@mkdir -p $(DIR_LIB)

ifeq ($(target_mode),native_pthread)

native_pthread:	ensurefolders $(OBJ_NATIVE)
				@mkdir -p `dirname $(NAME_NATIVE)`	
				$(CXX) $(CXXFLAGS_NATIVE) $(OBJ_NATIVE) -o $(NAME_NATIVE) $(LDFLAGS_NATIVE)

else ifeq ($(target_mode),web_asmjs_pthread)

web_pthread:	ensurefolders $(OBJ_WEB_PTHREAD)
				@mkdir -p `dirname $(NAME_WEB_PTHREAD)`	
				$(CXX) $(CXXFLAGS_WEB_PTHREAD) $(OBJ_WEB_PTHREAD) -o $(NAME_WEB_PTHREAD) $(LDFLAGS_WEB_PTHREAD)

else ifeq ($(target_mode),web_asmjs_webworker)

web_main:		ensurefolders $(OBJ_WEB_MAIN)
				@mkdir -p `dirname $(NAME_WEB_MAIN)`	
				$(CXX) $(CXXFLAGS_WEB_MAIN) $(OBJ_WEB_MAIN) -o $(NAME_WEB_MAIN) $(LDFLAGS_WEB_MAIN)

web_worker:		ensurefolders $(OBJ_WEB_WORKER)
				@mkdir -p `dirname $(NAME_WEB_WORKER)`
				$(CXX) $(CXXFLAGS_WEB_WORKER) $(OBJ_WEB_WORKER) -o $(NAME_WEB_WORKER) $(LDFLAGS_WEB_WORKER)

endif

bullet:				ensurefolders bullet_linearmath bullet_collision bullet_dynamics

bullet_linearmath:	$(OBJ_BULLET_LINEARMATH)
					$(AR) cr $(NAME_BULLET_LINEARMATH) $(OBJ_BULLET_LINEARMATH)

bullet_collision:	$(OBJ_BULLET_COLLISION)
					$(AR) cr $(NAME_BULLET_COLLISION) $(OBJ_BULLET_COLLISION)

bullet_dynamics:	$(OBJ_BULLET_DYNAMICS)
					$(AR) cr $(NAME_BULLET_DYNAMICS) $(OBJ_BULLET_DYNAMICS)

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

ifeq ($(target_mode),native_pthread)

$(OBJDIR_NATIVE)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_NATIVE) $< -c -o $@

else ifeq ($(target_mode),web_asmjs_pthread)

$(OBJDIR_WEB_PTHREAD)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_WEB_PTHREAD) $< -c -o $@

else ifeq ($(target_mode),web_asmjs_webworker)

$(OBJDIR_WEB_MAIN)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_WEB_MAIN) $< -c -o $@

$(OBJDIR_WEB_WORKER)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_WEB_WORKER) $< -c -o $@

endif

$(OBJDIR_BULLET_LINEARMATH)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

$(OBJDIR_BULLET_COLLISION)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

$(OBJDIR_BULLET_DYNAMICS)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

#

ifeq ($(target_mode),native_pthread)

clean:
		$(RM) $(OBJDIR_NATIVE)

fclean:	clean
		$(RM) $(NAME_NATIVE)

else ifeq ($(target_mode),web_asmjs_pthread)

clean:
		$(RM) $(OBJDIR_WEB_PTHREAD)

fclean:	clean
		$(RM) $(NAME_WEB_PTHREAD)

else ifeq ($(target_mode),web_asmjs_webworker)

clean_web_main:
					$(RM) $(OBJDIR_WEB_MAIN)
clean_web_worker:
					$(RM) $(OBJDIR_WEB_WORKER)
clean:				clean_web_main clean_web_worker

fclean_web_main:	clean_web_main
					$(RM) $(NAME_WEB_MAIN)
fclean_web_worker:	clean_web_worker
					$(RM) $(NAME_WEB_WORKER)
fclean:				fclean_web_main fclean_web_worker

endif

bullet_clean:
				$(RM) $(OBJDIR_BULLET)
bullet_fclean:	bullet_clean
				$(RM) $(DIR_LIB)

re:			fclean all

.PHONY:		all \
			native_pthread \
			web_pthread \
			web_main \
			web_worker \
			main \
			worker \
			clean \
			clean_web_main \
			clean_web_worker \
			fclean \
			fclean_web_main \
			fclean_web_worker \
			re \
			bullet \
			bullet_linearmath \
			bullet_collision \
			bullet_dynamics \
			bullet_clean \
			bullet_fclean

## RULE(S)
#
