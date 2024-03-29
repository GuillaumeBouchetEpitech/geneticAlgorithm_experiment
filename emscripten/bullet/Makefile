
build_platform=native_pthread
# build_platform=web_wasm_pthread
# build_platform=web_wasm_webworker

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native_pthread)
# $(info build_platform is valid, value=$(build_platform))
else ifeq ($(build_platform),web_wasm_pthread)
# $(info build_platform is valid, value=$(build_platform))
else ifeq ($(build_platform),web_wasm_webworker)
# $(info build_platform is valid, value=$(build_platform))
else
$(error unsupported value for "build_platform", value=$(build_platform))
endif

#

ifeq ($(build_mode),release)
# $(info build_mode is valid, value=$(build_mode))
else ifeq ($(build_mode),debug)
# $(info build_mode is valid, value=$(build_mode))
else
$(error unsupported value for "build_mode", value=$(build_mode))
endif

LOG_INFO= '[$(build_mode)] [$(build_platform)]'

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
DIR_OBJ_MACHINE_LEARNING=	./obj/native/machine_learning

else

DIR_OBJ_MACHINE_LEARNING=	./obj/web/machine_learning

ifeq ($(build_platform),web_wasm_pthread)

DIR_OBJ=									./obj/web/pthread

else ifeq ($(build_platform),web_wasm_webworker)

DIR_OBJ=									./obj/web/webworker/main
DIR_OBJ_WEB_WORKER=				./obj/web/webworker/worker

endif

endif

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
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/hud/helpers/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/scene/*.cpp)
SRC+=	$(wildcard $(DIR_SRC)/demo/logic/graphic/renderers/scene/helpers/*.cpp)
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

OBJ=	$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))

ifeq ($(build_platform),web_wasm_webworker)

OBJ_WEB_WORKER=	$(patsubst %.cpp, $(DIR_OBJ_WEB_WORKER)/%.o, $(SRC_WEB_WORKER))

endif

OBJ_MACHINE_LEARNING=	$(patsubst %.cpp, $(DIR_OBJ_MACHINE_LEARNING)/%.o, $(SRC_MACHINE_LEARNING))

#######


ifeq ($(build_mode),release)

BUILD_FLAG=		-O3 # optimisation flag

else

BUILD_FLAG=		-g3 # debug flag

endif

# DEPENDENCIES += sdl2 glesv2 egl openal
DEPENDENCIES_LIST += sdl2 glesv2 egl
DEPENDENCIES_CXXFLAGS += $(shell pkg-config $(DEPENDENCIES_LIST) --cflags)
DEPENDENCIES_LDFLAGS += $(shell pkg-config $(DEPENDENCIES_LIST) --libs)

CXXFLAGS += $(BUILD_FLAG)
CXXFLAGS += -std=c++17
CXXFLAGS += -Wall -W -Wextra -Wunused
CXXFLAGS += -I$(DIR_SRC)
CXXFLAGS += -I$(DIR_SHARED_LIB_SRC)

ifeq ($(build_platform),native_pthread)

CXX=clang++
AR=ar

CXXFLAGS += $(DEPENDENCIES_CXXFLAGS)

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_FRAMEWORK_PHYSIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_GRAPHIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_SYSTEM)
LDFLAGS += $(NAME_MACHINE_LEARNING)
LDFLAGS += $(NAME_LIB_BULLET_DYNAMICS)
LDFLAGS += $(NAME_LIB_BULLET_COLLISION)
LDFLAGS += $(NAME_LIB_BULLET_LINEARMATH)
LDFLAGS += $(DEPENDENCIES_LDFLAGS)
LDFLAGS += -pthread

else

CXX=em++
AR=emar

CXXFLAGS += -s USE_SDL=2

LDFLAGS_COMMON_WEB += -s USE_SDL=2
LDFLAGS_COMMON_WEB += -s USE_WEBGL2=1
LDFLAGS_COMMON_WEB += -s FULL_ES3=1
LDFLAGS_COMMON_WEB += -s TOTAL_MEMORY=128Mb # 16Kb, 256Mb, etc.
LDFLAGS_COMMON_WEB +=	-s WASM=1
LDFLAGS_COMMON_WEB +=	-s BINARYEN_IGNORE_IMPLICIT_TRAPS=1
LDFLAGS_COMMON_WEB += -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0
LDFLAGS_COMMON_WEB += -sEXPORTED_RUNTIME_METHODS=cwrap

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
LDFLAGS += --preload-file ./assets/
LDFLAGS += -Wl,--shared-memory,--no-check-features
LDFLAGS += -s PTHREAD_POOL_SIZE=17

else

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_FRAMEWORK_GRAPHIC)
LDFLAGS += $(NAME_LIB_FRAMEWORK_SYSTEM)
LDFLAGS += $(NAME_MACHINE_LEARNING)
LDFLAGS += $(LDFLAGS_COMMON_WEB)
LDFLAGS += --preload-file ./assets/

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
	@mkdir -p `dirname $(NAME_APPLICATION)`
	@mkdir -p `dirname $(NAME_MACHINE_LEARNING)`

application:	ensurefolders $(OBJ)
	@echo ' ---> building $(LOG_INFO): "application"'
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME_APPLICATION) $(LDFLAGS)
	@echo '   --> built $(LOG_INFO): "application"'

ifeq ($(build_platform),web_wasm_webworker)

web_worker:		ensurefolders $(OBJ_WEB_WORKER)
	@echo ' ---> building $(LOG_INFO): "web worker application"'
	@$(CXX) $(CXXFLAGS) $(OBJ_WEB_WORKER) -o $(NAME_WEB_WORKER) $(LDFLAGS_WEB_WORKER)
	@echo '   --> built $(LOG_INFO): "web worker application"'

endif

machine_learning:	ensurefolders $(OBJ_MACHINE_LEARNING)
	@echo ' ---> building $(LOG_INFO): "machine learning library"'
	@$(AR) cr $(NAME_MACHINE_LEARNING) $(OBJ_MACHINE_LEARNING) $(NAME_LIB_FRAMEWORK_SYSTEM)
	@echo '   --> built $(LOG_INFO): "machine learning library"'

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' ---> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS) $< -c -o $@

ifeq ($(build_platform),web_wasm_webworker)

$(DIR_OBJ_WEB_WORKER)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' ---> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS) $< -c -o $@

endif

$(DIR_OBJ_MACHINE_LEARNING)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' ---> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS) $< -c -o $@

#

ifneq ($(build_platform),web_wasm_webworker)

clean:
	@echo ' -> cleaning $(LOG_INFO): application build file(s)'
	$(RM) $(DIR_OBJ)
	@echo '   -> cleaned $(LOG_INFO): application build file(s)'

fclean:	clean
	@echo ' -> cleaning $(LOG_INFO): application file(s)'
	$(RM) $(NAME_APPLICATION)
	@echo '   -> cleaned $(LOG_INFO): application file(s)'

else ifeq ($(build_platform),web_wasm_webworker)

clean:
	@echo ' -> cleaning $(LOG_INFO): application and webworker build file(s)'
	$(RM) $(DIR_OBJ)
	$(RM) $(DIR_OBJ_WEB_WORKER)
	@echo '   -> cleaned $(LOG_INFO): application and webworker build file(s)'

fclean:	clean
	@echo ' -> cleaning $(LOG_INFO): application and webworker file(s)'
	$(RM) $(NAME_APPLICATION)
	$(RM) $(NAME_WEB_WORKER)
	@echo '   -> cleaned $(LOG_INFO): application and webworker file(s)'

endif

re:			fclean all

clean_ml:
	@echo ' -> cleaning $(LOG_INFO): machine learning build file(s)'
	$(RM) $(DIR_OBJ_MACHINE_LEARNING)
	@echo '   -> cleaned $(LOG_INFO): machine learning build file(s)'

fclean_ml:	clean_ml
	@echo ' -> cleaning $(LOG_INFO): machine learning file(s)'
	$(RM) $(NAME_MACHINE_LEARNING)
	@echo '   -> cleaned $(LOG_INFO): machine learning file(s)'

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
