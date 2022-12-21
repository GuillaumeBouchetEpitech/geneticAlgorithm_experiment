
build_platform=native
# build_platform=web_wasm

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native)

# $(info build_platform is valid, value=$(build_platform))

else ifeq ($(build_platform),web_wasm)

# $(info build_platform is valid, value=$(build_platform))

else

$(error unsupported value for "build_platform", value=$(build_platform))

endif

LOG_INFO= '[$(build_mode)] [$(build_platform)]'

#

ifeq ($(build_platform),native)

DIR_LIB=	./lib/native
DIR_TESTS=	./bin

NAME_TESTS=			$(DIR_TESTS)/exec
NAME_FRAMEWORK_SYSTEM=			$(DIR_LIB)/lib_framework_system.a
NAME_FRAMEWORK_GRAPHIC=			$(DIR_LIB)/lib_framework_graphic.a
NAME_FRAMEWORK_PHYSIC=			$(DIR_LIB)/lib_framework_physic.a
NAME_FRAMEWORK_AUDIO=			$(DIR_LIB)/lib_framework_audio.a
NAME_BULLET_LINEARMATH=	$(DIR_LIB)/lib_bullet_linearmath.a
NAME_BULLET_COLLISION=	$(DIR_LIB)/lib_bullet_collision.a
NAME_BULLET_DYNAMICS=	$(DIR_LIB)/lib_bullet_dynamics.a

else ifeq ($(build_platform),web_wasm)

DIR_LIB=	./lib/web

NAME_FRAMEWORK_SYSTEM=			$(DIR_LIB)/lib_framework_system.bc
NAME_FRAMEWORK_GRAPHIC=			$(DIR_LIB)/lib_framework_graphic.bc
NAME_FRAMEWORK_PHYSIC=			$(DIR_LIB)/lib_framework_physic.bc
NAME_FRAMEWORK_AUDIO=			$(DIR_LIB)/lib_framework_audio.bc
NAME_BULLET_LINEARMATH=	$(DIR_LIB)/lib_bullet_linearmath.bc
NAME_BULLET_COLLISION=	$(DIR_LIB)/lib_bullet_collision.bc
NAME_BULLET_DYNAMICS=	$(DIR_LIB)/lib_bullet_dynamics.bc

endif



#### DIRS

DIR_SRC=		src
DIR_THIRDPARTY=	$(DIR_SRC)/thirdparty
DIR_BULLET=		$(DIR_THIRDPARTY)/bullet3/src

#### /DIRS



ifeq ($(build_platform),native)

DIR_OBJ=			./obj/native

else ifeq ($(build_platform),web_wasm)

DIR_OBJ=			./obj/web

endif

DIR_OBJ_FRAMEWORK=	$(DIR_OBJ)/framework
DIR_OBJ_BULLET=	$(DIR_OBJ)/bullet
DIR_OBJ_BULLET_LINEARMATH=	$(DIR_OBJ_BULLET)/linearmath
DIR_OBJ_BULLET_COLLISION=	$(DIR_OBJ_BULLET)/collision
DIR_OBJ_BULLET_DYNAMICS=		$(DIR_OBJ_BULLET)/dynamic

ifeq ($(build_platform),native)

DIR_OBJ_TESTS=	$(DIR_OBJ)/tests

endif


#### SRC

SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/files/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/math/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/math/proceduralGeneration/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/string/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/multithreading/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/multithreading/internals/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/messaging/*.cpp)
SRC_FRAMEWORK_SYSTEM+=	$(wildcard $(DIR_SRC)/framework/system/parser_utils/*.cpp)

SRC_FRAMEWORK_GRAPHIC+=	$(wildcard $(DIR_SRC)/framework/graphic/*.cpp)
SRC_FRAMEWORK_GRAPHIC+=	$(wildcard $(DIR_SRC)/framework/graphic/sdl/*.cpp)
SRC_FRAMEWORK_GRAPHIC+=	$(wildcard $(DIR_SRC)/framework/graphic/camera/*.cpp)
SRC_FRAMEWORK_GRAPHIC+=	$(wildcard $(DIR_SRC)/framework/graphic/vertexBuffers/*.cpp)
SRC_FRAMEWORK_GRAPHIC+=	$(wildcard $(DIR_SRC)/framework/graphic/loaders/*.cpp)
SRC_FRAMEWORK_GRAPHIC+=	$(wildcard $(DIR_SRC)/framework/graphic/wrappers/*.cpp)

SRC_FRAMEWORK_PHYSIC+=	$(wildcard $(DIR_SRC)/framework/physic/*.cpp)
SRC_FRAMEWORK_PHYSIC+=	$(wildcard $(DIR_SRC)/framework/physic/body/*.cpp)
SRC_FRAMEWORK_PHYSIC+=	$(wildcard $(DIR_SRC)/framework/physic/shape/*.cpp)
SRC_FRAMEWORK_PHYSIC+=	$(wildcard $(DIR_SRC)/framework/physic/vehicle/*.cpp)
SRC_FRAMEWORK_PHYSIC+=	$(wildcard $(DIR_SRC)/framework/physic/queries/*.cpp)

SRC_FRAMEWORK_AUDIO+=	$(wildcard $(DIR_SRC)/framework/audio/*.cpp)
SRC_FRAMEWORK_AUDIO+=	$(wildcard $(DIR_SRC)/framework/audio/decoders/*.cpp)

SRC_BULLET_LINEARMATH+= $(wildcard $(DIR_BULLET)/LinearMath/*.cpp)

SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/BroadphaseCollision/*.cpp)
SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/CollisionShapes/*.cpp)
SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/CollisionDispatch/*.cpp)
SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/NarrowPhaseCollision/*.cpp)
# SRC_BULLET_COLLISION+= $(wildcard $(DIR_BULLET)/BulletCollision/Gimpact/*.cpp) # TODO: useful?

# SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/Character/*.cpp) # TODO: useful?
SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/Dynamics/*.cpp)
SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/Vehicle/*.cpp)
SRC_BULLET_DYNAMICS+= $(wildcard $(DIR_BULLET)/BulletDynamics/ConstraintSolver/*.cpp) # TODO: useful?

SRC_TESTS+=	$(wildcard $(DIR_SRC)/tests/*.cpp)
SRC_TESTS+=	$(wildcard $(DIR_SRC)/tests/audio/*.cpp)

#

OBJ_FRAMEWORK_SYSTEM=	$(patsubst %.cpp, $(DIR_OBJ_FRAMEWORK)/%.o, $(SRC_FRAMEWORK_SYSTEM))
OBJ_FRAMEWORK_GRAPHIC=	$(patsubst %.cpp, $(DIR_OBJ_FRAMEWORK)/%.o, $(SRC_FRAMEWORK_GRAPHIC))
OBJ_FRAMEWORK_PHYSIC=	$(patsubst %.cpp, $(DIR_OBJ_FRAMEWORK)/%.o, $(SRC_FRAMEWORK_PHYSIC))
OBJ_FRAMEWORK_AUDIO=	$(patsubst %.cpp, $(DIR_OBJ_FRAMEWORK)/%.o, $(SRC_FRAMEWORK_AUDIO))

OBJ_BULLET_LINEARMATH=	$(patsubst %.cpp, $(DIR_OBJ_BULLET_LINEARMATH)/%.o, $(SRC_BULLET_LINEARMATH))
OBJ_BULLET_COLLISION=	$(patsubst %.cpp, $(DIR_OBJ_BULLET_COLLISION)/%.o, $(SRC_BULLET_COLLISION))
OBJ_BULLET_DYNAMICS=	$(patsubst %.cpp, $(DIR_OBJ_BULLET_DYNAMICS)/%.o, $(SRC_BULLET_DYNAMICS))

ifeq ($(build_platform),native)

OBJ_TESTS=	$(patsubst %.cpp, $(DIR_OBJ_TESTS)/%.o, $(SRC_TESTS))

endif

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

ifeq ($(build_platform),native)

CXX=clang++
AR=ar

CXXFLAGS_FRAMEWORK += $(COMMON_FLAGS)
CXXFLAGS_FRAMEWORK += -D_REENTRANT -I/usr/include/SDL2

CXXFLAGS_TESTS += $(BUILD_FLAG)
CXXFLAGS_TESTS += -std=c++17
CXXFLAGS_TESTS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS_TESTS += -I$(DIR_SRC)

LDFLAGS_TESTS += $(BUILD_FLAG)
LDFLAGS_TESTS += $(NAME_FRAMEWORK_AUDIO)
LDFLAGS_TESTS += $(NAME_FRAMEWORK_GRAPHIC)
LDFLAGS_TESTS += $(NAME_FRAMEWORK_SYSTEM)
LDFLAGS_TESTS += -lopenal

else ifeq ($(build_platform),web_wasm)

CXX=em++
AR=emar

CXXFLAGS_FRAMEWORK += $(COMMON_FLAGS)
CXXFLAGS_FRAMEWORK += -s USE_SDL=2
CXXFLAGS_FRAMEWORK += -s USE_PTHREADS=0

endif


RM=			rm -rf


#######

#
## RULE(S)

all:	\
	bullet	\
	framework

ensure_folders:
	@mkdir -p $(DIR_LIB)

# framework:	ensure_folders $(OBJ_FRAMEWORK)
# 	# $(AR) cr $(NAME_FRAMEWORK) $(OBJ_FRAMEWORK)

framework:	\
	ensure_folders	\
	framework_system	\
	framework_graphic	\
	framework_physic	\
	framework_audio

framework_system:	ensure_folders $(OBJ_FRAMEWORK_SYSTEM)
	@echo ' ---> building $(LOG_INFO): "framework system library"'
	@$(AR) cr $(NAME_FRAMEWORK_SYSTEM) $(OBJ_FRAMEWORK_SYSTEM)
	@echo '   --> built $(LOG_INFO): "framework system library"'

framework_graphic:	ensure_folders $(OBJ_FRAMEWORK_GRAPHIC)
	@echo ' ---> building $(LOG_INFO): "framework graphic library"'
	@$(AR) cr $(NAME_FRAMEWORK_GRAPHIC) $(OBJ_FRAMEWORK_GRAPHIC)
	@echo '   --> built $(LOG_INFO): "framework graphic library"'

framework_physic:	ensure_folders $(OBJ_FRAMEWORK_PHYSIC)
	@echo ' ---> building $(LOG_INFO): "framework physic library"'
	@$(AR) cr $(NAME_FRAMEWORK_PHYSIC) $(OBJ_FRAMEWORK_PHYSIC)
	@echo '   --> built $(LOG_INFO): "framework physic library"'

framework_audio:	ensure_folders $(OBJ_FRAMEWORK_AUDIO)
	@echo ' ---> building $(LOG_INFO): "framework audio library"'
	@$(AR) cr $(NAME_FRAMEWORK_AUDIO) $(OBJ_FRAMEWORK_AUDIO)
	@echo '   --> built $(LOG_INFO): "framework audio library"'

ifeq ($(build_platform),native)

ensure_tests_folders: ensure_folders
	@mkdir -p $(DIR_TESTS)

build-tests: ensure_tests_folders $(OBJ_TESTS)
	@echo ' ---> building $(LOG_INFO): "tests"'
	@$(CXX) $(CXXFLAGS_TESTS) $(OBJ_TESTS) -o $(NAME_TESTS) $(LDFLAGS_TESTS)
	@echo '   --> built $(LOG_INFO): "tests"'

endif

bullet:	\
	ensure_folders	\
	bullet_linearmath	\
	bullet_collision	\
	bullet_dynamics

bullet_linearmath:	$(OBJ_BULLET_LINEARMATH)
	@echo ' ---> building $(LOG_INFO): "bullet linear math library"'
	@$(AR) cr $(NAME_BULLET_LINEARMATH) $(OBJ_BULLET_LINEARMATH)
	@echo '   --> built $(LOG_INFO): "bullet linear math library"'

bullet_collision:	$(OBJ_BULLET_COLLISION)
	@echo ' ---> building $(LOG_INFO): "bullet collision library"'
	@$(AR) cr $(NAME_BULLET_COLLISION) $(OBJ_BULLET_COLLISION)
	@echo '   --> built $(LOG_INFO): "bullet collision library"'

bullet_dynamics:	$(OBJ_BULLET_DYNAMICS)
	@echo ' ---> building $(LOG_INFO): "bullet dynamics library"'
	@$(AR) cr $(NAME_BULLET_DYNAMICS) $(OBJ_BULLET_DYNAMICS)
	@echo '   --> built $(LOG_INFO): "bullet dynamics library"'

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_OBJ_FRAMEWORK)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS_FRAMEWORK) $< -c -o $@

ifeq ($(build_platform),native)

$(DIR_OBJ_TESTS)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS_TESTS) $< -c -o $@

endif

$(DIR_OBJ_BULLET_LINEARMATH)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

$(DIR_OBJ_BULLET_COLLISION)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

$(DIR_OBJ_BULLET_DYNAMICS)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS_BULLET) $< -c -o $@

#

clean:
	@echo ' -> cleaning $(LOG_INFO): framework library build file(s)'
	@$(RM) $(DIR_OBJ_FRAMEWORK)
	@echo '   -> cleaned $(LOG_INFO): framework library build file(s)'

fclean: clean
	@echo ' -> cleaning $(LOG_INFO): framework library file(s)'
	@$(RM) $(NAME_FRAMEWORK_SYSTEM)
	@$(RM) $(NAME_FRAMEWORK_GRAPHIC)
	@$(RM) $(NAME_FRAMEWORK_PHYSIC)
	@$(RM) $(NAME_FRAMEWORK_AUDIO)
	@echo '   -> cleaned $(LOG_INFO): framework library file(s)'

ifeq ($(build_platform),native)

clean_tests:
	@echo ' -> cleaning $(LOG_INFO): tests build file(s)'
	@$(RM) $(DIR_OBJ_TESTS)
	@echo '   -> cleaned $(LOG_INFO): tests build file(s)'

fclean_tests: clean_tests
	@echo ' -> cleaning $(LOG_INFO): tests file(s)'
	@$(RM) $(NAME_TESTS)
	@echo '   -> cleaned $(LOG_INFO): tests file(s)'

endif

clean_bullet:
	@echo ' -> cleaning $(LOG_INFO): bullet physics library build file(s)'
	@$(RM) $(DIR_OBJ_BULLET)
	@echo '   -> cleaned $(LOG_INFO): bullet physics library build file(s)'

fclean_bullet: clean_bullet
	@echo ' -> cleaning $(LOG_INFO): bullet physics library file(s)'
	@$(RM) $(NAME_BULLET_LINEARMATH)
	@$(RM) $(NAME_BULLET_COLLISION)
	@$(RM) $(NAME_BULLET_DYNAMICS)
	@echo '   -> cleaned $(LOG_INFO): bullet physics library file(s)'

re:			fclean all

.PHONY:	\
		all \
		framework \
		clean \
		fclean \
		clean_tests \
		fclean_tests \
		re \
		bullet \
		bullet_linearmath \
		bullet_collision \
		bullet_dynamics \
		clean_bullet \
		fclean_bullet

## RULE(S)
#
