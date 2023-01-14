

DIR_SRC=						./src
DIR_APPLICATION=		./bin
DIR_FRAMEWORK=			..
DIR_FRAMEWORK_SRC=	$(DIR_FRAMEWORK)/src
DIR_FRAMEWORK_DEP=	$(DIR_FRAMEWORK)/lib/native

NAME_APPLICATION=				$(DIR_APPLICATION)/exec
NAME_FRAMEWORK_SYSTEM=	$(DIR_FRAMEWORK_DEP)/lib_framework_system.a
NAME_FRAMEWORK_GRAPHIC=	$(DIR_FRAMEWORK_DEP)/lib_framework_graphic.a
NAME_FRAMEWORK_PHYSIC=	$(DIR_FRAMEWORK_DEP)/lib_framework_physic.a
NAME_FRAMEWORK_AUDIO=		$(DIR_FRAMEWORK_DEP)/lib_framework_audio.a
NAME_BULLET_LINEARMATH=	$(DIR_FRAMEWORK_DEP)/lib_bullet_linearmath.a
NAME_BULLET_COLLISION=	$(DIR_FRAMEWORK_DEP)/lib_bullet_collision.a
NAME_BULLET_DYNAMICS=		$(DIR_FRAMEWORK_DEP)/lib_bullet_dynamics.a


SRC+= \
	$(wildcard \
		$(DIR_SRC)/*.cpp \
		$(DIR_SRC)/basic_double_linked_list/*.cpp \
		$(DIR_SRC)/deterministic_rng/*.cpp \
		$(DIR_SRC)/compression/*.cpp \
		$(DIR_SRC)/frustum_culling/*.cpp \
		$(DIR_SRC)/generic_array_container/*.cpp \
		$(DIR_SRC)/dynamic_heap_array/*.cpp \
		$(DIR_SRC)/generic_array_container_commons/*.cpp \
		$(DIR_SRC)/physic_wrapper/*.cpp \
		$(DIR_SRC)/weak_ref_data_pool/*.cpp \
		$(DIR_SRC)/static_heap_grid_array/*.cpp \
		$(DIR_SRC)/math/*.cpp \
	)

# $(DIR_SRC)/audio/*.cpp \


DIR_OBJ=	./obj

OBJ=		$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))



BUILD_FLAG= -g3

CXXFLAGS += $(BUILD_FLAG)
CXXFLAGS += -std=c++17
CXXFLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS += -I$(DIR_SRC)
CXXFLAGS += -I$(DIR_FRAMEWORK_SRC)
CXXFLAGS += -no-pie

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_FRAMEWORK_AUDIO)
LDFLAGS += $(NAME_FRAMEWORK_PHYSIC)
LDFLAGS += $(NAME_FRAMEWORK_GRAPHIC)
LDFLAGS += $(NAME_FRAMEWORK_SYSTEM)
LDFLAGS += $(NAME_BULLET_DYNAMICS)
LDFLAGS += $(NAME_BULLET_COLLISION)
LDFLAGS += $(NAME_BULLET_LINEARMATH)

LIBRARIES += sdl2
LIBRARIES += glesv2
LIBRARIES += egl
LIBRARIES += openal
LIBRARIES += gtest_main

CXXFLAGS += $(shell pkg-config $(LIBRARIES) --cflags)
LDFLAGS += $(shell pkg-config $(LIBRARIES) --libs)


RM=			rm -rf


#######

#
## RULE(S)

all:	application

ensurefolders:
	@mkdir -p `dirname $(NAME_APPLICATION)`

application:	ensurefolders $(OBJ)
	@echo ' ---> building $(LOG_INFO) "application"'
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME_APPLICATION) $(LDFLAGS)
	@echo '   --> built $(LOG_INFO) "application"'

#

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS) $< -c -o $@

#

clean:
	@echo ' -> cleaning $(LOG_INFO): application build file(s)'
	@$(RM) $(DIR_OBJ)
	@echo '   -> cleaned $(LOG_INFO): application build file(s)'

fclean:	clean
	@echo ' -> cleaned $(LOG_INFO): application file(s)'
	@$(RM) $(NAME_APPLICATION)
	@echo '   -> cleaned $(LOG_INFO): application file(s)'

re:			fclean all

.PHONY:	\
	all \
	ensurefolders \
	application \
	clean \
	fclean \
	re

## RULE(S)
#
