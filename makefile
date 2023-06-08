#=============================================================================================================================================================================

DEB_FLAGS = -DVAR_OVERSEER

VR_FLAGS =  -D_DEBUG -g -ggdb3 -std=c++17 -O3

ASSAN = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DOP = -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE

DIRECTORIES = -IFRONTEND -IBACKEND -IBACKEND/MC_BACKEND -IBACKEND/ELF_BACKEND -IEXTRA_LIBS -IEXTRA_LIBS/STACK -IEXAMPLES -IDSL -IBACKEND/MC_LIBS/ASM -IBACKEND/MC_LIBS/CPU -IBACKEND/MC_LIBS/DISASM -I/usr/include/c++/11

VR_FLAGS += $(ASSAN)

VR_FLAGS += $(DIRECTORIES)

VR_FLAGS += $(DOP)

#=============================================================================================================================================================================

VR_COMPILER = g++

#=============================================================================================================================================================================

DO: FOLDERS BUILD_FRONTEND BUILD_BACKEND_MC BUILD_BACKEND_ELF DO_ASM DO_DISASM DO_PROC  #DO_COMPILE

#=============================================================================================================================================================================

VR_SRC_F = FRONTEND

VR_OBJ_F = $(VR_SRC_F)/OBJECTS

VR_SRC_B = BACKEND

VR_OBJ_B = $(VR_SRC_B)/OBJECTS


VR_SRC_MC_B = $(VR_SRC_B)/MC_BACKEND

VR_OBJ_MC_B = $(VR_SRC_MC_B)/OBJECTS

VR_SRC_ELF_B = $(VR_SRC_B)/ELF_BACKEND

VR_OBJ_ELF_B = $(VR_SRC_ELF_B)/OBJECTS


#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BUILD_FRONTEND:  $(VR_OBJ_F)/front.o $(VR_OBJ_F)/main_front.o $(VR_OBJ_F)/front_file_print.o $(VR_OBJ_F)/front_graph_viz.o
	$(VR_COMPILER) $(VR_OBJ_F)/front.o $(VR_OBJ_F)/main_front.o $(VR_OBJ_F)/front_file_print.o $(VR_OBJ_F)/front_graph_viz.o -o front $(VR_FLAGS)

BUILD_BACKEND_MC:  	$(VR_OBJ_MC_B)/back.o $(VR_OBJ_MC_B)/main_back.o $(VR_OBJ_B)/proc.o $(VR_OBJ_B)/disasm.o $(VR_OBJ_B)/asm.o
	$(VR_COMPILER) 	$(VR_OBJ_MC_B)/back.o $(VR_OBJ_MC_B)/main_back.o $(VR_OBJ_B)/proc.o $(VR_OBJ_B)/disasm.o $(VR_OBJ_B)/asm.o -o back_mc $(VR_FLAGS)

BUILD_BACKEND_ELF:  $(VR_OBJ_ELF_B)/elf_back.o $(VR_OBJ_ELF_B)/elf_byte_ops.o $(VR_OBJ_ELF_B)/main_elf_back.o $(VR_OBJ_B)/proc.o $(VR_OBJ_B)/disasm.o $(VR_OBJ_B)/asm.o $(VR_OBJ_MC_B)/back.o
	$(VR_COMPILER) 	$(VR_OBJ_ELF_B)/elf_back.o $(VR_OBJ_ELF_B)/elf_byte_ops.o $(VR_OBJ_ELF_B)/main_elf_back.o $(VR_OBJ_B)/proc.o $(VR_OBJ_B)/disasm.o $(VR_OBJ_B)/asm.o $(VR_OBJ_MC_B)/back.o -o back_elf $(VR_FLAGS)


# EXTRA_LIBS/STACK/OBJECTS/stack.o: EXTRA_LIBS/STACK/stack.cpp
# 	$(VR_COMPILER) -c -o EXTRA_LIBS/STACK/OBJECTS/stack.o EXTRA_LIBS/STACK/stack.cpp $(VR_FLAGS)

# EXTRA_LIBS/STACK/OBJECTS/p_stack.o: EXTRA_LIBS/STACK/p_stack.cpp
# 	$(VR_COMPILER) -c -o EXTRA_LIBS/STACK/OBJECTS/p_stack.o EXTRA_LIBS/STACK/p_stack.cpp $(VR_FLAGS)

# BACKEND/OBJECTS/proc.o: BACKEND/CPU/proc.cpp
# 	$(VR_COMPILER) -c -o BACKEND/OBJECTS/proc.o BACKEND/CPU/proc.cpp $(VR_FLAGS)
#
# BACKEND/OBJECTS/asm.o: BACKEND/ASM/asm.cpp
# 	$(VR_COMPILER) -c -o BACKEND/OBJECTS/asm.o BACKEND/ASM/asm.cpp $(VR_FLAGS)
#
# BACKEND/OBJECTS/disasm.o: BACKEND/DISASM/disasm.cpp
# 	$(VR_COMPILER) -c -o BACKEND/OBJECTS/disasm.o BACKEND/DISASM/disasm.cpp $(VR_FLAGS)

#=============================================================================================================================================================================

$(VR_OBJ_F)/front_file_print.o: $(VR_SRC_F)/front_file_print.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_F)/front_file_print.o $(VR_SRC_F)/front_file_print.cpp $(VR_FLAGS)

$(VR_OBJ_F)/front.o: $(VR_SRC_F)/front_graph_viz.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_F)/front_graph_viz.o $(VR_SRC_F)/front_graph_viz.cpp $(VR_FLAGS)

$(VR_OBJ_F)/front.o: $(VR_SRC_F)/front.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_F)/front.o $(VR_SRC_F)/front.cpp $(VR_FLAGS)

$(VR_OBJ_F)/main_front.o: $(VR_SRC_F)/main_front.cpp
	$(VR_COMPILER) -c -o $(VR_OBJ_F)/main_front.o $(VR_SRC_F)/main_front.cpp $(VR_FLAGS)

#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

$(VR_OBJ_MC_B)/back.o: $(VR_SRC_MC_B)/back.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_MC_B)/back.o $(VR_SRC_MC_B)/back.cpp $(VR_FLAGS)

$(VR_OBJ_MC_B)/main_back.o: $(VR_SRC_MC_B)/main_back.cpp
	$(VR_COMPILER) -c -o $(VR_OBJ_MC_B)/main_back.o $(VR_SRC_MC_B)/main_back.cpp $(VR_FLAGS)


$(VR_OBJ_ELF_B)/elf_byte_ops.o: $(VR_SRC_ELF_B)/elf_byte_ops.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_ELF_B)/elf_byte_ops.o $(VR_SRC_ELF_B)/elf_byte_ops.cpp $(VR_FLAGS)

$(VR_OBJ_ELF_B)/elf_back.o: $(VR_SRC_ELF_B)/elf_back.cpp
	$(VR_COMPILER) -c -o  $(VR_OBJ_ELF_B)/elf_back.o $(VR_SRC_ELF_B)/elf_back.cpp $(VR_FLAGS)

$(VR_OBJ_ELF_B)/main_elf_back.o: $(VR_SRC_ELF_B)/main_elf_back.cpp
	$(VR_COMPILER) -c -o $(VR_OBJ_ELF_B)/main_elf_back.o $(VR_SRC_ELF_B)/main_elf_back.cpp $(VR_FLAGS)


#=============================================================================================================================================================================

DO_ASM: BACKEND/OBJECTS/m_asm.o BACKEND/OBJECTS/asm.o
	$(VR_COMPILER) BACKEND/OBJECTS/m_asm.o BACKEND/OBJECTS/asm.o -o BACKEND/BUILD/asm $(VR_FLAGS)

BACKEND/OBJECTS/m_asm.o: BACKEND/MC_LIBS/ASM/m_asm.cpp
	$(VR_COMPILER) -c -o BACKEND/OBJECTS/m_asm.o BACKEND/MC_LIBS/ASM/m_asm.cpp $(VR_FLAGS)

BACKEND/OBJECTS/asm.o: BACKEND/MC_LIBS/ASM/asm.cpp
	$(VR_COMPILER) -c -o BACKEND/OBJECTS/asm.o BACKEND/MC_LIBS/ASM/asm.cpp $(VR_FLAGS)

#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DO_DISASM: BACKEND/OBJECTS/m_disasm.o BACKEND/OBJECTS/disasm.o BACKEND/OBJECTS/asm.o
	$(VR_COMPILER) BACKEND/OBJECTS/m_disasm.o BACKEND/OBJECTS/disasm.o BACKEND/OBJECTS/asm.o -o BACKEND/BUILD/disasm $(VR_FLAGS)

BACKEND/OBJECTS/m_disasm.o: BACKEND/MC_LIBS/DISASM/m_disasm.cpp
	$(VR_COMPILER) -c -o BACKEND/OBJECTS/m_disasm.o BACKEND/MC_LIBS/DISASM/m_disasm.cpp $(VR_FLAGS)

BACKEND/OBJECTS/disasm.o: BACKEND/MC_LIBS/DISASM/disasm.cpp
	$(VR_COMPILER) -c -o BACKEND/OBJECTS/disasm.o BACKEND/MC_LIBS/DISASM/disasm.cpp $(VR_FLAGS)

#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DO_PROC: BACKEND/OBJECTS/m_proc.o BACKEND/OBJECTS/proc.o BACKEND/OBJECTS/disasm.o BACKEND/OBJECTS/asm.o
	$(VR_COMPILER) BACKEND/OBJECTS/m_proc.o BACKEND/OBJECTS/proc.o BACKEND/OBJECTS/disasm.o BACKEND/OBJECTS/asm.o  -o BACKEND/BUILD/proc $(VR_FLAGS)

BACKEND/OBJECTS/m_proc.o: BACKEND/MC_LIBS/CPU/m_proc.cpp
	$(VR_COMPILER) -c -o BACKEND/OBJECTS/m_proc.o BACKEND/MC_LIBS/CPU/m_proc.cpp $(VR_FLAGS)

BACKEND/OBJECTS/proc.o: BACKEND/MC_LIBS/CPU/proc.cpp
	$(VR_COMPILER) -c -o BACKEND/OBJECTS/proc.o BACKEND/MC_LIBS/CPU/proc.cpp $(VR_FLAGS)

#=============================================================================================================================================================================

FOLDERS:
	@mkdir -p EXTRA_LIBS/STACK/OBJECTS
	@mkdir -p $(VR_SRC_F)/OBJECTS
	@mkdir -p $(VR_SRC_B)/OBJECTS
	@mkdir -p $(VR_SRC_B)/GRAPH_VIZ
	@mkdir -p $(VR_SRC_F)/GRAPH_VIZ
	@mkdir -p $(VR_SRC_MC_B)/OBJECTS
	@mkdir -p $(VR_SRC_ELF_B)/OBJECTS

#=============================================================================================================================================================================

.PHONY: clean
clean:
	@rm -rf *.o

.PHONY: run
run:
	@echo "\e[0;35m===========================BEGIN======================================================================================================================================= \e[0m"
	@make -j
	@echo "\e[0;35m===========================END========================================================================================================================================= \e[0m"
	@echo "\e[0;32m===========================BEGIN======================================================================================================================================= \e[0m"
	@./back_elf
	@echo "\e[0;32m===========================END========================================================================================================================================= \e[0m"

#@hexdump EXAMPLES_ELF/code.elf

.PHONY: ex
ex:
	@nasm -f elf64 -g -o BACKEND/ELF_BACKEND/ELF_EX/example.o BACKEND/ELF_BACKEND/ELF_EX/example.asm
	@ld -m elf_x86_64 BACKEND/ELF_BACKEND/ELF_EX/example.o -o asm_example

#=============================================================================================================================================================================

# vg:
# 	colour-valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./front

#=============================================================================================================================================================================

# make -Bnw | compiledb
# wc -l *.cpp *.h */*.cpp */*.h */*/*.cpp */*/*.h
