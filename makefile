#=============================================================================================================================================================================

VR_FLAGS = -D _DEBUG -g -ggdb3 -std=c++17 -O0

ASSAN = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DOP = -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE

DIRECTORIES = -IFRONTEND -IBACKEND -IEXTRA -IEXAMPLES -IDSL

VR_FLAGS += $(ASSAN)

VR_FLAGS += $(DIRECTORIES)

VR_FLAGS += $(DOP)

#=============================================================================================================================================================================

VR_COMPILER = g++

#=============================================================================================================================================================================

DO: BUILD_FRONTEND BUILD_BACKEND #DO_COMPILE

#=============================================================================================================================================================================

VR_SRC_F = FRONTEND

VR_OBJ_F = $(VR_SRC_F)/OBJECTS

VR_SRC_B = BACKEND

VR_OBJ_B = $(VR_SRC_B)/OBJECTS

#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BUILD_FRONTEND: FOLDERS $(VR_OBJ_F)/front.o $(VR_OBJ_F)/main_front.o
	$(VR_COMPILER) $(VR_OBJ_F)/front.o $(VR_OBJ_F)/main_front.o -o front $(VR_FLAGS)

BUILD_BACKEND: FOLDERS $(VR_OBJ_B)/back.o $(VR_OBJ_B)/main_back.o
	$(VR_COMPILER) $(VR_OBJ_B)/back.o $(VR_OBJ_B)/main_back.o -o back $(VR_FLAGS)

#=============================================================================================================================================================================

$(VR_OBJ_F)/front.o: $(VR_SRC_F)/front.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_F)/front.o $(VR_SRC_F)/front.cpp $(VR_FLAGS)

$(VR_OBJ_F)/main_front.o: $(VR_SRC_F)/main_front.cpp
	$(VR_COMPILER) -c -o $(VR_OBJ_F)/main_front.o $(VR_SRC_F)/main_front.cpp $(VR_FLAGS)

#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

$(VR_OBJ_B)/back.o: $(VR_SRC_B)/back.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_B)/back.o $(VR_SRC_B)/back.cpp $(VR_FLAGS)

$(VR_OBJ_B)/main_back.o: $(VR_SRC_B)/main_back.cpp
	$(VR_COMPILER) -c -o $(VR_OBJ_B)/main_back.o $(VR_SRC_B)/main_back.cpp $(VR_FLAGS)

#=============================================================================================================================================================================

FOLDERS:
	mkdir -p $(VR_SRC_F)/OBJECTS; mkdir -p $(VR_SRC_B)/OBJECTS; mkdir -p $(VR_SRC_F)/GRAPH_VIZ

# vg:
# 	colour-valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./front

#=============================================================================================================================================================================
