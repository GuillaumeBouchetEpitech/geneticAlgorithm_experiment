
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

ifeq ($(build_mode),release)
$(info build_mode is valid, value=$(build_mode))
else ifeq ($(build_mode),debug)
$(info build_mode is valid, value=$(build_mode))
else
$(error unsupported value for "build_mode", value=$(build_mode))
endif

#

ifeq ($(build_platform),native_pthread)

DIR_TARGET=					./bin
NAME_APPLICATION=		$(DIR_TARGET)/exec

else ifeq ($(build_platform),web_wasm_pthread)

DIR_TARGET=					./dist/wasm/pthread
NAME_APPLICATION=		$(DIR_TARGET)/index.js

else ifeq ($(build_platform),web_wasm_webworker)

DIR_TARGET=					./dist/wasm/webworker
NAME_APPLICATION=		$(DIR_TARGET)/index.js
NAME_WEB_WORKER=		$(DIR_TARGET)/worker.js

endif

#

DIR_SHARED_LIB=		../shared-framework

ifeq ($(build_platform),native_pthread)

DIR_LIB=									./lib/native
NAME_MACHINE_LEARNING=		$(DIR_LIB)/lib_machine_learning.a

DIR_SHARED_LIB_ARCHIVES=			$(DIR_SHARED_LIB)/lib/native
NAME_LIB_FRAMEWORK_SYSTEM=		$(DIR_SHARED_LIB_ARCHIVES)/lib_framework_system.a
NAME_LIB_FRAMEWORK_GRAPHIC=		$(DIR_SHARED_LIB_ARCHIVES)/lib_framework_graphic.a
NAME_LIB_FRAMEWORK_PHYSIC=		$(DIR_SHARED_LIB_ARCHIVES)/lib_framework_physic.a
NAME_LIB_BULLET_LINEARMATH=		$(DIR_SHARED_LIB_ARCHIVES)/lib_bullet_linearmath.a
NAME_LIB_BULLET_COLLISION=		$(DIR_SHARED_LIB_ARCHIVES)/lib_bullet_collision.a
NAME_LIB_BULLET_DYNAMICS=			$(DIR_SHARED_LIB_ARCHIVES)/lib_bullet_dynamics.a

else

DIR_LIB=									./lib/web
NAME_MACHINE_LEARNING=		$(DIR_LIB)/lib_machine_learning.bc

DIR_SHARED_LIB_ARCHIVES=			$(DIR_SHARED_LIB)/lib/web
NAME_LIB_FRAMEWORK_SYSTEM=		$(DIR_SHARED_LIB_ARCHIVES)/lib_framework_system.bc
NAME_LIB_FRAMEWORK_GRAPHIC=		$(DIR_SHARED_LIB_ARCHIVES)/lib_framework_graphic.bc
NAME_LIB_FRAMEWORK_PHYSIC=		$(DIR_SHARED_LIB_ARCHIVES)/lib_framework_physic.bc
NAME_LIB_BULLET_LINEARMATH=		$(DIR_SHARED_LIB_ARCHIVES)/lib_bullet_linearmath.bc
NAME_LIB_BULLET_COLLISION=		$(DIR_SHARED_LIB_ARCHIVES)/lib_bullet_collision.bc
NAME_LIB_BULLET_DYNAMICS=			$(DIR_SHARED_LIB_ARCHIVES)/lib_bullet_dynamics.bc

endif



#### DIRS

DIR_SRC=							src
DIR_SHARED_LIB_SRC=		$(DIR_SHARED_LIB)/src

#### /DIRS



ifeq ($(build_platform),native_pthread)

DIR_OBJ=									./obj/native
OBJDIR=										$(DIR_OBJ)

else

DIR_OBJ=									./obj/web

ifeq ($(build_platform),web_wasm_pthread)

OBJDIR=										$(DIR_OBJ)/pthread

else ifeq ($(build_platform),web_wasm_webworker)

OBJDIR=										$(DIR_OBJ)/webworker/main
OBJDIR_WEB_WORKER=				$(DIR_OBJ)/webworker/worker

endif

endif

OBJDIR_MACHINE_LEARNING=	$(DIR_OBJ)/machine_learning

#### SRC

# pthread (native + web-pthread)

SRC+=	$(wildcard $(DIR_SRC)/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/states/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/helpers/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/camera/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/hud/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/scene/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/helpers/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/postProcess/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/logic/*.cpp)

ifneq ($(build_platform),web_wasm_webworker)

SRC+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/pthread/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/logic/physic/*.cpp)

else

SRC+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/webworker/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/webworker/producer/*.cpp)

SRC_WEB_WORKER+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/logic/*.cpp)
SRC_WEB_WORKER+=	$(wildcard $(DIR_SRC)/demo/logic/simulation/webworker/consumer/*.cpp)

endif

SRC_MACHINE_LEARNING+=	$(wildcard $(DIR_SRC)/machineLearning/*.cpp)

#

OBJ=	$(patsubst %.cpp, $(OBJDIR)/%.o, $(SRC))

ifeq ($(build_platform),web_wasm_webworker)

OBJ_WEB_WORKER=	$(patsubst %.cpp, $(OBJDIR_WEB_WORKER)/%.o, $(SRC_WEB_WORKER))

endif

OBJ_MACHINE_LEARNING=	$(patsubst %.cpp, $(OBJDIR_MACHINE_LEARNING)/%.o, $(SRC_MACHINE_LEARNING))

#######


ifeq ($(build_mode),release)

BUILD_FLAG=		-O3 # optimisation flag

else

BUILD_FLAG=		-g3 # debug flag

endif


CXXFLAGS += $(BUILD_FLAG)
CXXFLAGS += -std=c++17
CXXFLAGS += -Wall -W -Wextra -Wunused
CXXFLAGS += -I$(DIR_SRC)
CXXFLAGS += -I$(DIR_SHARED_LIB_SRC)

ifeq ($(build_platform),native_pthread)

CXX=g++
AR=ar

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_FRAMEWORK_PHYSIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_GRAPHIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_SYSTEM)
LDFLAGS += $(NAME_MACHINE_LEARNING)
LDFLAGS += -lSDL2
LDFLAGS += -lGLESv2
LDFLAGS += -pthread
LDFLAGS += $(NAME_LIB_BULLET_DYNAMICS)
LDFLAGS += $(NAME_LIB_BULLET_COLLISION)
LDFLAGS += $(NAME_LIB_BULLET_LINEARMATH)

else

CXX=em++
AR=emar

CXXFLAGS += -s USE_SDL=2

LDFLAGS_COMMON_WEB += -s USE_SDL=2
LDFLAGS_COMMON_WEB += -s TOTAL_MEMORY=128Mb # 16Kb, 256Mb, etc.
LDFLAGS_COMMON_WEB += --preload-file ./assets/
LDFLAGS_COMMON_WEB +=	-s WASM=1
LDFLAGS_COMMON_WEB +=	-s BINARYEN_IGNORE_IMPLICIT_TRAPS=1
LDFLAGS_COMMON_WEB += -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0

ifeq ($(build_mode),debug)

LDFLAGS_COMMON_WEB += -s DEMANGLE_SUPPORT=1
LDFLAGS_COMMON_WEB += -s GL_ASSERTIONS=1
LDFLAGS_COMMON_WEB += -s GL_DEBUG=1
LDFLAGS_COMMON_WEB += -s DISABLE_EXCEPTION_CATCHING=2

endif

ifeq ($(build_platform),web_wasm_pthread)

FLAGS_WEB_MAIN += -s USE_PTHREADS=1

CXXFLAGS += $(FLAGS_WEB_MAIN)

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(FLAGS_WEB_MAIN)
LDFLAGS += $(NAME_LIB_FRAMEWORK_PHYSIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_GRAPHIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_SYSTEM)
LDFLAGS += $(NAME_MACHINE_LEARNING)
LDFLAGS += $(NAME_LIB_BULLET_DYNAMICS)
LDFLAGS += $(NAME_LIB_BULLET_COLLISION)
LDFLAGS += $(NAME_LIB_BULLET_LINEARMATH)
LDFLAGS += $(LDFLAGS_COMMON_WEB)
LDFLAGS += -Wl,--shared-memory,--no-check-features
LDFLAGS += -s PTHREAD_POOL_SIZE=17

else

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_FRAMEWORK_GRAPHIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_SYSTEM)
LDFLAGS += $(NAME_MACHINE_LEARNING)
LDFLAGS += $(LDFLAGS_COMMON_WEB)

LDFLAGS_WEB_WORKER += $(BUILD_FLAG)
LDFLAGS_WEB_WORKER += $(NAME_LIB_FRAMEWORK_PHYSIC)
LDFLAGS_WEB_WORKER += $(NAME_LIB_FRAMEWORK_SYSTEM)
LDFLAGS_WEB_WORKER += $(NAME_MACHINE_LEARNING)
LDFLAGS_WEB_WORKER += $(NAME_LIB_BULLET_DYNAMICS)
LDFLAGS_WEB_WORKER += $(NAME_LIB_BULLET_COLLISION)
LDFLAGS_WEB_WORKER += $(NAME_LIB_BULLET_LINEARMATH)
LDFLAGS_WEB_WORKER += $(LDFLAGS_COMMON_WEB)
LDFLAGS_WEB_WORKER += -s BUILD_AS_WORKER=1

endif

endif


RM=			rm -rf


#######

#
## RULE(S)

ifneq ($(build_platform),web_wasm_webworker)

all:	machine_learning application

else ifeq ($(build_platform),web_wasm_webworker)

all:	machine_learning application web_worker

endif

ensurefolders:
				@mkdir -p $(DIR_TARGET)
				@mkdir -p $(DIR_LIB)

application:	ensurefolders $(OBJ)
				@mkdir -p `dirname $(NAME_APPLICATION)`
				$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME_APPLICATION) $(LDFLAGS)

ifeq ($(build_platform),web_wasm_webworker)

web_worker:		ensurefolders $(OBJ_WEB_WORKER)
				@mkdir -p `dirname $(NAME_WEB_WORKER)`
				$(CXX) $(CXXFLAGS) $(OBJ_WEB_WORKER) -o $(NAME_WEB_WORKER) $(LDFLAGS_WEB_WORKER)

endif

machine_learning:	ensurefolders $(OBJ_MACHINE_LEARNING)
				$(AR) cr $(NAME_MACHINE_LEARNING) $(OBJ_MACHINE_LEARNING) $(NAME_LIB_FRAMEWORK_SYSTEM)

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(OBJDIR)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $< -c -o $@

ifeq ($(build_platform),web_wasm_webworker)

$(OBJDIR_WEB_WORKER)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $< -c -o $@

endif

$(OBJDIR_MACHINE_LEARNING)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $< -c -o $@

#

ifneq ($(build_platform),web_wasm_webworker)

clean:
		$(RM) $(OBJDIR)

fclean:	clean
		$(RM) $(NAME_APPLICATION)

else ifeq ($(build_platform),web_wasm_webworker)

clean:
		$(RM) $(OBJDIR)
		$(RM) $(OBJDIR_WEB_WORKER)

fclean:	clean
		$(RM) $(NAME_APPLICATION)
		$(RM) $(NAME_WEB_WORKER)

endif

re:			fclean all

clean_ml:
		$(RM) $(OBJDIR_MACHINE_LEARNING)

fclean_ml:	clean_ml
		$(RM) $(NAME_MACHINE_LEARNING)

re_ml:	fclean_ml machine_learning

.PHONY: \
			all \
			application \
			web_worker \
			clean \
			fclean \
			re \
			machine_learning \
			clean_ml \
			fclean_ml \
			re_ml

## RULE(S)
#
