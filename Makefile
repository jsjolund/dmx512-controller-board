# System configuration
SHELL = bash
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
RM = rm -rf

# Includes
INCLUDE_DIRS = Inc \
			Src \
			Drivers/CMSIS/Include \
			Drivers/CMSIS/Device/ST/STM32F4xx/Include \
			Drivers/STM32F4xx_HAL_Driver/Inc

INCLUDE_DIRS_PARAMS = $(foreach d, $(INCLUDE_DIRS), -I$d)

# Assembler, Compiler and Linker flags and linker script settings
# -lc_s -lm -lstdc++_s -lsupc++_s 
LINK_SCRIPT = "STM32F411RE_FLASH.ld"

LINKER_FLAGS = -lm -mthumb -mhard-float -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -Wl,--gc-sections -T$(LINK_SCRIPT) -static  -Wl,--start-group -Wl,--end-group -Wl,-cref "-Wl,-Map=$(BIN_DIR)/dmx512-controller-board.map" -Wl,--defsym=malloc_getpagesize_P=0x1000

ASSEMBLER_FLAGS = -c -g -O0 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mhard-float -x assembler-with-cpp  $(INCLUDE_DIRS_PARAMS)

COMPILER_FLAGS = -c -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -O0 -Wall -ffunction-sections -fdata-sections -mthumb -mhard-float -D"STM32F411xE" $(INCLUDE_DIRS_PARAMS)

CXXCOMPILER_FLAGS = -fno-threadsafe-statics -c -g -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -O0 -Wall -ffunction-sections -fdata-sections -mthumb -mhard-float -fno-exceptions -fno-rtti -D"STM32F411xE" $(INCLUDE_DIRS_PARAMS)

# Define output directory
OBJECT_DIR = Debug
BIN_DIR = $(OBJECT_DIR)

# Define sources and objects
SRC := $(wildcard */*/*/*/*/*/*/*.c) \
	$(wildcard */*/*/*/*/*/*.c) \
	$(wildcard */*/*/*/*/*.c) \
	$(wildcard */*/*/*/*.c) \
	$(wildcard */*/*/*.c) \
	$(wildcard */*/*.c) \
	$(wildcard */*.c) \
	$(wildcard */*/*/*/*/*/*/*.cpp) \
	$(wildcard */*/*/*/*/*/*.cpp) \
	$(wildcard */*/*/*/*/*.cpp) \
	$(wildcard */*/*/*/*.cpp) \
	$(wildcard */*/*/*.cpp) \
	$(wildcard */*/*.cpp) \
	$(wildcard */*.cpp)
SRCSASM := $(wildcard */*/*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*/*.s) \
	$(wildcard */*/*/*/*/*.s) \
	$(wildcard */*/*/*/*.s) \
	$(wildcard */*/*/*.s) \
	$(wildcard */*/*.s) \
	$(wildcard */*.s)
OBJS := $(SRC:%.c=$(OBJECT_DIR)/%.o) $(SRCSASM:%.s=$(OBJECT_DIR)/%.o)
OBJS := $(OBJS:%.S=$(OBJECT_DIR)/%.o)
OBJS := $(OBJS:%.cpp=$(OBJECT_DIR)/%.o)

###############
# Build project
# Major targets
###############
all: buildelf

buildelf: $(OBJS)
	$(CXX) -o "$(BIN_DIR)/dmx512-controller-board.elf" $(OBJS) $(LINKER_FLAGS)

clean:
	$(RM) $(OBJS) "$(BIN_DIR)/dmx512-controller-board.elf" "$(BIN_DIR)/dmx512-controller-board.map"

##################
# Specific targets
##################
# 	@mkdir $(subst /,\,$(dir $@)) 2> NUL || echo off
$(OBJECT_DIR)/Src/main.o: Src/main.cpp
	$(CXX) $(CXXCOMPILER_FLAGS) Src/main.cpp -o $(OBJECT_DIR)/Src/main.o

##################
# Implicit targets
##################
$(OBJECT_DIR)/%.o: %.c
	$(CC) $(COMPILER_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o: %.cpp
	$(CXX) $(CXXCOMPILER_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o: %.s
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o: %.S
	$(CC) $(ASSEMBLER_FLAGS) $< -o $@
