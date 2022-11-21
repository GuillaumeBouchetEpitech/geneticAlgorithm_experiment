
build_platform=native_pthread
# build_platform=web_wasm_pthread

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native_pthread)

$(info build_platform is valid, value=$(build_platform))

else ifeq ($(build_platform),web_wasm_pthread)

$(info build_platform is valid, value=$(build_platform))

else

$(error unsupported value for "build_platform", value=$(build_platform))

endif

#

ifeq ($(build_platform),native_pthread)

DIR_LIB=	./lib/native
DIR_TESTS=	./bin

NAME_TESTS=			$(DIR_TESTS)/exec
NAME_FRAMEWORK=			$(DIR_LIB)/lib_framework.a
NAME_BULLET_LINEARMATH=	$(DIR_LIB)/lib_linearmath.a
NAME_BULLET_COLLISION=	$(DIR_LIB)/lib_collision.a
NAME_BULLET_DYNAMICS=	$(DIR_LIB)/lib_dynamics.a

else ifeq ($(build_platform),web_wasm_pthread)

DIR_LIB=	./lib/web

NAME_FRAMEWORK=			$(DIR_LIB)/lib_framework.bc
NAME_BULLET_LINEARMATH=	$(DIR_LIB)/lib_linearmath.bc
NAME_BULLET_COLLISION=	$(DIR_LIB)/lib_collision.bc
NAME_BULLET_DYNAMICS=	$(DIR_LIB)/lib_dynamics.bc

endif



#### DIRS

DIR_SRC=		src
DIR_THIRDPARTY=	$(DIR_SRC)/thirdparty
DIR_BULLET=		$(DIR_THIRDPARTY)/bullet3/src

#### /DIRS



ifeq ($(build_platform),native_pthread)

DIR_OBJ=			./obj/native
OBJDIR_NATIVE=		$(DIR_OBJ)
OBJDIR_TESTS=			$(OBJDIR_NATIVE)

else ifeq ($(build_platform),web_wasm_pthread)

DIR_OBJ=			./obj/web
OBJDIR_WEB_PTHREAD=	$(DIR_OBJ)/pthread

endif

OBJDIR_FRAMEWORK=	$(DIR_OBJ)/framework
OBJDIR_BULLET=	$(DIR_OBJ)/bullet
OBJDIR_BULLET_LINEARMATH=	$(OBJDIR_BULLET)/linearmath
OBJDIR_BULLET_COLLISION=	$(OBJDIR_BULLET)/collision
OBJDIR_BULLET_DYNAMICS=		$(OBJDIR_BULLET)/dynamic



#### SRC

SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/files/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/math/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/math/proceduralGeneration/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/graphic/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/graphic/camera/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/graphic/vertexBuffers/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/graphic/loaders/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/physic/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/physic/body/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/physic/shape/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/physic/vehicle/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/physic/queries/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/sdl/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/string/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/multithreading/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/multithreading/internals/*.cpp)
SRC_FRAMEWORK+=	$(wildcard $(DIR_SRC)/framework/messaging/*.cpp)

SRC_BULLET_LINEARMATH+= $(wildcard $(DIR_BULLET)/LinearMath/*.cpp)

SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/BroadphaseCollision/*.cpp)
SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/CollisionShapes/*.cpp)
SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/CollisionDispatch/*.cpp)
SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/NarrowPhaseCollision/*.cpp)
SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/Gimpact/*.cpp) # TODO: useful?

SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/Character/*.cpp) # TODO: useful?
SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/Dynamics/*.cpp)
SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/Vehicle/*.cpp) # TODO: useful?
SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/ConstraintSolver/*.cpp) # TODO: useful?

SRC_TESTS+=	$(wildcard $(DIR_SRC)/tests/*.cpp)

#

ifeq ($(build_platform),native_pthread)

OBJ_FRAMEWORK=	$(patsubst %.cpp, $(OBJDIR_FRAMEWORK)/%.o, $(SRC_FRAMEWORK))
OBJ_TESTS=	$(patsubst %.cpp, $(OBJDIR_TESTS)/%.o, $(SRC_TESTS))

else ifeq ($(build_platform),web_wasm_pthread)

OBJ_FRAMEWORK=	$(patsubst %.cpp, $(OBJDIR_FRAMEWORK)/%.o, $(SRC_FRAMEWORK))

endif

OBJ_BULLET_LINEARMATH=	$(patsubst %.cpp, $(OBJDIR_BULLET_LINEARMATH)/%.o, $(SRC_BULLET_LINEARMATH))
OBJ_BULLET_COLLISION=	$(patsubst %.cpp, $(OBJDIR_BULLET_COLLISION)/%.o, $(SRC_BULLET_COLLISION))
OBJ_BULLET_DYNAMICS=	$(patsubst %.cpp, $(OBJDIR_BULLET_DYNAMICS)/%.o, $(SRC_BULLET_DYNAMICS))

#

#

#### /BULLET_SRC



#######


ifeq ($(build_mode),release)

BUILD_FLAG= -O3

else

BUILD_FLAG= -g3

endif

CXXFLAGS_BULLET += $(BUILD_FLAG)
CXXFLAGS_BULLET += -std=c++17
CXXFLAGS_BULLET += -I$(DIR_BULLET)
CXXFLAGS_BULLET += -DBT_NO_PROFILE
CXXFLAGS_BULLET += -DSIMD_FORCE_INLINE=inline

COMMON_FLAGS += $(CXXFLAGS_BULLET)
COMMON_FLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
COMMON_FLAGS += -I$(DIR_SRC)

ifeq ($(build_platform),native_pthread)

CXX=g++
AR=ar

CXXFLAGS_FRAMEWORK_NATIVE += $(COMMON_FLAGS)

CXXFLAGS_TESTS += $(BUILD_FLAG)
CXXFLAGS_TESTS += -std=c++17
CXXFLAGS_TESTS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS_TESTS += -I$(DIR_SRC)

LDFLAGS_TESTS += $(BUILD_FLAG)
LDFLAGS_TESTS += $(NAME_FRAMEWORK)
# LDFLAGS_TESTS += $(NAME_BULLET_DYNAMICS)
# LDFLAGS_TESTS += $(NAME_BULLET_COLLISION)
# LDFLAGS_TESTS += $(NAME_BULLET_LINEARMATH)

else ifeq ($(build_platform),web_wasm_pthread)

CXX=em++
AR=emar

CXXFLAGS_FRAMEWORK_WEB += $(COMMON_FLAGS)
CXXFLAGS_FRAMEWORK_WEB += -s USE_SDL=2
CXXFLAGS_FRAMEWORK_WEB += -s USE_PTHREADS=0

endif


RM=			rm -rf


#######

#
## RULE(S)

all:	bullet framework

ifeq ($(build_platform),native_pthread)

ensurefolders:
	@mkdir -p $(DIR_LIB)
	@mkdir -p $(DIR_TESTS)

else ifeq ($(build_platform),web_wasm_pthread)

ensurefolders:
	@mkdir -p $(DIR_LIB)

endif

ifeq ($(build_platform),native_pthread)

framework:	ensurefolders $(OBJ_FRAMEWORK)
	$(AR) cr $(NAME_FRAMEWORK) $(OBJ_FRAMEWORK)

build-tests: ensurefolders $(OBJ_TESTS)
	$(CXX) $(CXXFLAGS_TESTS) $(OBJ_TESTS) -o $(NAME_TESTS) $(LDFLAGS_TESTS)

else ifeq ($(build_platform),web_wasm_pthread)

framework:	$(OBJ_FRAMEWORK)
	$(AR) cr $(NAME_FRAMEWORK) $(OBJ_FRAMEWORK)

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

ifeq ($(build_platform),native_pthread)

$(OBJDIR_FRAMEWORK)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_FRAMEWORK_NATIVE) $< -c -o $@

$(OBJDIR_TESTS)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_TESTS) $< -c -o $@

else ifeq ($(build_platform),web_wasm_pthread)

$(OBJDIR_FRAMEWORK)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS_FRAMEWORK_WEB) $< -c -o $@

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

clean:
		$(RM) $(OBJDIR_FRAMEWORK)

clean_bullet:
		$(RM) $(OBJDIR_BULLET)

fclean:	clean
		$(RM) $(NAME_FRAMEWORK)

fclean_bullet:	clean
		$(RM) $(NAME_BULLET_LINEARMATH)
		$(RM) $(NAME_BULLET_COLLISION)
		$(RM) $(NAME_BULLET_DYNAMICS)

re:			fclean all

.PHONY:	\
		all \
		framework \
		clean \
		fclean \
		bullet \
		bullet_linearmath \
		bullet_collision \
		bullet_dynamics \
		clean_bullet \
		fclean_bullet \
		re

## RULE(S)
#
