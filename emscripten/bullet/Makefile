
build_platform=native_pthread
# build_platform=web_wasm_pthread
# build_platform=web_wasm_webworker

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native_pthread)

$(info build_platform is valid, value=$(build_platform))

else ifeq ($(build_platform),web_wasm_pthread)

$(info build_platform is valid, value=$(build_platform))

else ifeq ($(build_platform),web_wasm_webworker)

$(info build_platform is valid, value=$(build_platform))

else

$(error unsupported value for "build_platform", value=$(build_platform))

endif

#

ifeq ($(build_platform),native_pthread)

DIR_TARGET=	./bin
DIR_LIB=	./lib/native
DIR_SHARED_LIB=	../shared-framework/lib/native

NAME_MACHINE_LEARNING=			$(DIR_LIB)/lib_machinelearning.a

NAME_NATIVE=			$(DIR_TARGET)/exec
NAME_FRAMEWORK=			$(DIR_SHARED_LIB)/lib_framework.a
NAME_BULLET_LINEARMATH=	$(DIR_SHARED_LIB)/lib_linearmath.a
NAME_BULLET_COLLISION=	$(DIR_SHARED_LIB)/lib_collision.a
NAME_BULLET_DYNAMICS=	$(DIR_SHARED_LIB)/lib_dynamics.a

else ifeq ($(build_platform),web_wasm_pthread)

DIR_TARGET=	./dist/wasm/pthread

NAME_WEB_PTHREAD=		$(DIR_TARGET)/index.js

DIR_LIB=	./lib/web
DIR_SHARED_LIB=	../shared-framework/lib/web

NAME_MACHINE_LEARNING=			$(DIR_LIB)/lib_machinelearning.bc

NAME_FRAMEWORK=			$(DIR_SHARED_LIB)/lib_framework.bc
NAME_BULLET_LINEARMATH=	$(DIR_SHARED_LIB)/lib_linearmath.bc
NAME_BULLET_COLLISION=	$(DIR_SHARED_LIB)/lib_collision.bc
NAME_BULLET_DYNAMICS=	$(DIR_SHARED_LIB)/lib_dynamics.bc

else ifeq ($(build_platform),web_wasm_webworker)

DIR_TARGET=	./dist/wasm/webworker

DIR_LIB=	./lib/web/webworker
DIR_SHARED_LIB=	../shared-framework/lib/web

NAME_MACHINE_LEARNING=			$(DIR_LIB)/lib_machinelearning.bc

NAME_WEB_MAIN=			$(DIR_TARGET)/index.js
NAME_WEB_WORKER=		$(DIR_TARGET)/worker.js
NAME_FRAMEWORK=			$(DIR_SHARED_LIB)/lib_framework.bc
NAME_BULLET_LINEARMATH=	$(DIR_SHARED_LIB)/lib_linearmath.bc
NAME_BULLET_COLLISION=	$(DIR_SHARED_LIB)/lib_collision.bc
NAME_BULLET_DYNAMICS=	$(DIR_SHARED_LIB)/lib_dynamics.bc

endif



#### DIRS

DIR_SRC=		src
DIR_LIB_SRC=		../shared-framework/src

#### /DIRS



ifeq ($(build_platform),native_pthread)

DIR_OBJ=			./obj/native
OBJDIR_NATIVE=		$(DIR_OBJ)
OBJDIR_MACHINE_LEARNING=	$(OBJDIR_NATIVE)/machinelearning

else ifeq ($(build_platform),web_wasm_pthread)

DIR_OBJ=			./obj/web
OBJDIR_WEB_PTHREAD=	$(DIR_OBJ)/pthread
OBJDIR_MACHINE_LEARNING=	$(OBJDIR_WEB_PTHREAD)/machinelearning

else ifeq ($(build_platform),web_wasm_webworker)

DIR_OBJ=			./obj/web/webworker
OBJDIR_WEB_MAIN=	$(DIR_OBJ)/main
OBJDIR_WEB_WORKER=	$(DIR_OBJ)/worker
OBJDIR_MACHINE_LEARNING=	$(DIR_OBJ)/machinelearning

endif

#### SRC

ifneq ($(build_platform),web_wasm_webworker)

# pthread (native + web-pthread)

SRC_NATIVE+=	$(wildcard $(DIR_SRC)/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/states/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/helpers/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/camera/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/hud/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/scene/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/helpers/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/pthread/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/logic/*.cpp)
SRC_NATIVE+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/logic/physic/*.cpp)

SRC_WEB_PTHREAD=	$(SRC_NATIVE) # web-pthread same as native

else

# web-webworker only

SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/states/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/helpers/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/camera/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/hud/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/scene/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/helpers/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/logic/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/webworker/*.cpp)
SRC_WEB_MAIN+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/webworker/producer/*.cpp)

SRC_WEB_WORKER+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/logic/*.cpp)
SRC_WEB_WORKER+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/webworker/consumer/*.cpp)

endif

SRC_MACHINE_LEARNING+=	$(wildcard $(DIR_SRC)/machineLearning/*.cpp)

#

ifeq ($(build_platform),native_pthread)

OBJ_NATIVE=	$(patsubst %.cpp, $(OBJDIR_NATIVE)/%.o, $(SRC_NATIVE))

else ifeq ($(build_platform),web_wasm_pthread)

OBJ_WEB_PTHREAD=	$(patsubst %.cpp, $(OBJDIR_WEB_PTHREAD)/%.o, $(SRC_WEB_PTHREAD))
OBJ_FRAMEWORK=	$(patsubst %.cpp, $(OBJDIR_FRAMEWORK)/%.o, $(SRC_FRAMEWORK))

else ifeq ($(build_platform),web_wasm_webworker)

OBJ_WEB_MAIN=	$(patsubst %.cpp, $(OBJDIR_WEB_MAIN)/%.o, $(SRC_WEB_MAIN))
OBJ_WEB_WORKER=	$(patsubst %.cpp, $(OBJDIR_WEB_WORKER)/%.o, $(SRC_WEB_WORKER))

endif

OBJ_MACHINE_LEARNING=	$(patsubst %.cpp, $(OBJDIR_MACHINE_LEARNING)/%.o, $(SRC_MACHINE_LEARNING))

#######


ifeq ($(build_mode),release)

	# optimisation flag
	BUILD_FLAG=		-O3

else

	# debug flag
	BUILD_FLAG=		-g3

endif


ifeq ($(build_platform),native_pthread)

CXX=g++
AR=ar

CXXFLAGS_NATIVE_FRAMEWORK += $(BUILD_FLAG)
CXXFLAGS_NATIVE_FRAMEWORK += -std=c++17
CXXFLAGS_NATIVE_FRAMEWORK += -Wall -W -Wextra -Wunused
CXXFLAGS_NATIVE_FRAMEWORK += -I$(DIR_SRC)

CXXFLAGS_NATIVE += $(CXXFLAGS_NATIVE_FRAMEWORK)
CXXFLAGS_NATIVE += -I$(DIR_LIB_SRC)

LDFLAGS_NATIVE += $(BUILD_FLAG)
LDFLAGS_NATIVE += $(NAME_FRAMEWORK)
LDFLAGS_NATIVE += $(NAME_MACHINE_LEARNING)
LDFLAGS_NATIVE += -lSDL2
LDFLAGS_NATIVE += -lGLESv2
LDFLAGS_NATIVE += -pthread
LDFLAGS_NATIVE += $(NAME_BULLET_DYNAMICS)
LDFLAGS_NATIVE += $(NAME_BULLET_COLLISION)
LDFLAGS_NATIVE += $(NAME_BULLET_LINEARMATH)

CXXFLAGS_BULLET += $(BUILD_FLAG)
CXXFLAGS_BULLET += -std=c++17
CXXFLAGS_BULLET += -I$(DIR_BULLET)
CXXFLAGS_BULLET += -DBT_NO_PROFILE
CXXFLAGS_BULLET += -DSIMD_FORCE_INLINE=inline

else ifeq ($(build_platform),web_wasm_pthread)

CXX=em++
AR=emar

CXXFLAGS_WEB_FRAMEWORK += $(BUILD_FLAG)
CXXFLAGS_WEB_FRAMEWORK += -std=c++17
CXXFLAGS_WEB_FRAMEWORK += -Wall -W -Wextra -Wunused
CXXFLAGS_WEB_FRAMEWORK += -I$(DIR_SRC)

COMMON_FLAGS += -s USE_SDL=2
COMMON_FLAGS += -s USE_PTHREADS=1

CXXFLAGS_WEB_PTHREAD += $(CXXFLAGS_WEB_FRAMEWORK)
CXXFLAGS_WEB_PTHREAD += $(COMMON_FLAGS)
CXXFLAGS_WEB_PTHREAD += -I$(DIR_LIB_SRC)

LDFLAGS_WEB_PTHREAD += $(BUILD_FLAG)
LDFLAGS_WEB_PTHREAD += $(COMMON_FLAGS)
LDFLAGS_WEB_PTHREAD += $(NAME_FRAMEWORK)
LDFLAGS_WEB_PTHREAD += $(NAME_MACHINE_LEARNING)
LDFLAGS_WEB_PTHREAD += $(NAME_BULLET_DYNAMICS)
LDFLAGS_WEB_PTHREAD += $(NAME_BULLET_COLLISION)
LDFLAGS_WEB_PTHREAD += $(NAME_BULLET_LINEARMATH)

# value must be a power of 2 (16, 32, 64. 128, etc.).
# suffixes must be one of those: KB, MB, GB, TB.
# Compile with ALLOW_MEMORY_GROWTH if you want a growable heap (slower though).
LDFLAGS_WEB_PTHREAD += -s TOTAL_MEMORY=256Mb

# assets loading
LDFLAGS_WEB_PTHREAD += --preload-file ./assets/

# hacky hack to make the multithreading works with wasm
LDFLAGS_WEB_PTHREAD += -Wl,--shared-memory,--no-check-features

LDFLAGS_WEB_PTHREAD += -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0

LDFLAGS_WEB_PTHREAD+= -s USE_PTHREADS=1
LDFLAGS_WEB_PTHREAD+= -s PTHREAD_POOL_SIZE=17

# now using wasm as faster than asm.js
LDFLAGS_WEB_PTHREAD+=	-s WASM=1 -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1

ifeq ($(build_mode),debug)

LDFLAGS_WEB_PTHREAD += -s DEMANGLE_SUPPORT=1
LDFLAGS_WEB_PTHREAD += -s GL_ASSERTIONS=1
LDFLAGS_WEB_PTHREAD += -s GL_DEBUG=1
LDFLAGS_WEB_PTHREAD += -s DISABLE_EXCEPTION_CATCHING=2

endif

else ifeq ($(build_platform),web_wasm_webworker)

CXX=em++
AR=emar

COMMON_LDFLAGS += $(BUILD_FLAG)

# value must be a power of 2 (16, 32, 64, 128, etc.).
# suffixes must be one of those: KB, MB, GB, TB.
# Compile with ALLOW_MEMORY_GROWTH if you want a growable heap (slower though).
COMMON_LDFLAGS += -s TOTAL_MEMORY=128Mb

COMMON_LDFLAGS += -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0

# now using wasm as faster than asm.js
COMMON_LDFLAGS += -s WASM=1

CXXFLAGS_COMMON += $(BUILD_FLAG)
CXXFLAGS_COMMON += -std=c++17
CXXFLAGS_COMMON += -Wall -W -Wextra -Wunused
CXXFLAGS_COMMON += -I$(DIR_SRC)
CXXFLAGS_COMMON += -I$(DIR_LIB_SRC)

FLAGS_WEB_MAIN += -s USE_SDL=2

FLAGS_WEB_WORKER += -s BUILD_AS_WORKER=1
FLAGS_WEB_WORKER += $(NAME_FRAMEWORK)
FLAGS_WEB_WORKER += $(NAME_MACHINE_LEARNING)
FLAGS_WEB_WORKER += $(NAME_BULLET_DYNAMICS)
FLAGS_WEB_WORKER += $(NAME_BULLET_COLLISION)
FLAGS_WEB_WORKER += $(NAME_BULLET_LINEARMATH)

CXXFLAGS_WEB_MAIN += $(CXXFLAGS_COMMON)
CXXFLAGS_WEB_MAIN += $(FLAGS_WEB_MAIN)

CXXFLAGS_WEB_WORKER += $(CXXFLAGS_COMMON)

LDFLAGS_WEB_MAIN += $(COMMON_LDFLAGS)
LDFLAGS_WEB_MAIN += $(FLAGS_WEB_MAIN)
LDFLAGS_WEB_MAIN += $(NAME_FRAMEWORK)
LDFLAGS_WEB_MAIN += $(NAME_MACHINE_LEARNING)

# assets loading
LDFLAGS_WEB_MAIN += --use-preload-plugins --preload-file ./assets/

LDFLAGS_WEB_WORKER += $(COMMON_LDFLAGS)
LDFLAGS_WEB_WORKER += $(FLAGS_WEB_WORKER)

ifeq ($(build_mode),debug)

LDFLAGS_WEB_MAIN += -s DEMANGLE_SUPPORT=1
LDFLAGS_WEB_MAIN += -s GL_ASSERTIONS=1
LDFLAGS_WEB_MAIN += -s GL_DEBUG=1
LDFLAGS_WEB_MAIN += -s DISABLE_EXCEPTION_CATCHING=2

LDFLAGS_WEB_WORKER += -s DEMANGLE_SUPPORT=1
LDFLAGS_WEB_WORKER += -s DISABLE_EXCEPTION_CATCHING=2

endif

endif


RM=			rm -rf


#######

#
## RULE(S)

ifeq ($(build_platform),native_pthread)

all:	native_pthread

else ifeq ($(build_platform),web_wasm_pthread)

all:	web_pthread

else ifeq ($(build_platform),web_wasm_webworker)

all:	web_main web_worker

endif

ensurefolders:
				@mkdir -p $(DIR_TARGET)
				@mkdir -p $(DIR_LIB)

ifeq ($(build_platform),native_pthread)

native_pthread:	ensurefolders $(OBJ_NATIVE)
				@mkdir -p `dirname $(NAME_NATIVE)`
				$(CXX) $(CXXFLAGS_NATIVE) $(OBJ_NATIVE) -o $(NAME_NATIVE) $(LDFLAGS_NATIVE)

machinelearning:	$(OBJ_MACHINE_LEARNING)
					$(AR) cr $(NAME_MACHINE_LEARNING) $(OBJ_MACHINE_LEARNING) $(NAME_FRAMEWORK)

else ifeq ($(build_platform),web_wasm_pthread)

web_pthread:	ensurefolders $(OBJ_WEB_PTHREAD)
				@mkdir -p `dirname $(NAME_WEB_PTHREAD)`
				$(CXX) $(CXXFLAGS_WEB_PTHREAD) $(OBJ_WEB_PTHREAD) -o $(NAME_WEB_PTHREAD) $(LDFLAGS_WEB_PTHREAD)

machinelearning:	$(OBJ_MACHINE_LEARNING)
					$(AR) cr $(NAME_MACHINE_LEARNING) $(OBJ_MACHINE_LEARNING) $(NAME_FRAMEWORK)

else ifeq ($(build_platform),web_wasm_webworker)

web_main:		ensurefolders $(OBJ_WEB_MAIN)
				@mkdir -p `dirname $(NAME_WEB_MAIN)`
				$(CXX) $(CXXFLAGS_WEB_MAIN) $(OBJ_WEB_MAIN) -o $(NAME_WEB_MAIN) $(LDFLAGS_WEB_MAIN)

web_worker:		ensurefolders $(OBJ_WEB_WORKER)
				@mkdir -p `dirname $(NAME_WEB_WORKER)`
				$(CXX) $(CXXFLAGS_WEB_WORKER) $(OBJ_WEB_WORKER) -o $(NAME_WEB_WORKER) $(LDFLAGS_WEB_WORKER)

machinelearning:	$(OBJ_MACHINE_LEARNING)
					$(AR) cr $(NAME_MACHINE_LEARNING) $(OBJ_MACHINE_LEARNING) $(NAME_FRAMEWORK)
endif

bullet:				ensurefolders bullet_linearmath bullet_collision bullet_dynamics

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

ifeq ($(build_platform),native_pthread)

$(OBJDIR_NATIVE)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_NATIVE) $< -c -o $@

$(OBJDIR_MACHINE_LEARNING)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_NATIVE) $< -c -o $@

else ifeq ($(build_platform),web_wasm_pthread)

$(OBJDIR_WEB_PTHREAD)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_WEB_PTHREAD) $< -c -o $@

$(OBJDIR_MACHINE_LEARNING)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_WEB_PTHREAD) $< -c -o $@

else ifeq ($(build_platform),web_wasm_webworker)

$(OBJDIR_WEB_MAIN)/%.o: %.cpp
	@mkdir -p `dirname $@`
	echo "FILE: " $@
	$(CXX) $(CXXFLAGS_WEB_MAIN) $< -c -o $@

$(OBJDIR_WEB_WORKER)/%.o: %.cpp
	@mkdir -p `dirname $@`
	echo "FILE: " $@
	$(CXX) $(CXXFLAGS_WEB_WORKER) $< -c -o $@

$(OBJDIR_MACHINE_LEARNING)/%.o: %.cpp
	@mkdir -p `dirname $@`
	echo "FILE: " $@
	$(CXX) $(CXXFLAGS_WEB_MAIN) $< -c -o $@

endif

#

ifeq ($(build_platform),native_pthread)

clean:
		$(RM) $(OBJDIR_NATIVE)

fclean:	clean
		$(RM) $(NAME_NATIVE)

else ifeq ($(build_platform),web_wasm_pthread)

clean:
		$(RM) $(OBJDIR_WEB_PTHREAD)

fclean:	clean
		$(RM) $(NAME_WEB_PTHREAD)

else ifeq ($(build_platform),web_wasm_webworker)

clean:
		$(RM) $(OBJDIR_WEB_MAIN)
		$(RM) $(OBJDIR_WEB_WORKER)

fclean:	clean
		$(RM) $(NAME_WEB_MAIN)
		$(RM) $(NAME_WEB_WORKER)

endif

bullet_clean:
				$(RM) $(OBJDIR_BULLET)
bullet_fclean:	bullet_clean
				$(RM) $(DIR_LIB)

re:			fclean all

.PHONY: \
			all \
			native_pthread \
			web_pthread \
			web_main \
			web_worker \
			main \
			worker \
			clean \
			fclean \
			re \
			machinelearning

## RULE(S)
#
