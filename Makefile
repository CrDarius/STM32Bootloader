CC=arm-none-eabi-g++
OPT=-O0
INCDIRS=. ./inc/
SRCDIRS=. ./src/
OUTDIRS=out
CFLAGS=-mcpu=cortex-m4 -mthumb -Wall -Wextra -fno-exceptions $(foreach D, $(INCDIRS), -I$(D)) -g
LFLAGS=-nostartfiles -specs=nano.specs -specs=nosys.specs -Xlinker -Map=$(dir $(BINARY))/$(TARGET).map
LSCRIPT=-T linker.ld

BINARY=bin/$(TARGET).elf
TARGET=bootloader

# Collect source files in subdirectories
CPPFILES=$(foreach D, $(SRCDIRS), $(wildcard $(D)*.cpp))
OBJECTS=$(addprefix $(OUTDIRS)/, $(notdir $(patsubst %.cpp, %.o, $(CPPFILES))))

# Used just for debugging purposes
#$(info CPPFILES = $(CPPFILES))
#$(info OBJECTS = $(OBJECTS))

all: $(BINARY)

compile: $(OBJECTS)

link: $(BINARY)


$(BINARY): $(OBJECTS)
	@mkdir -p bin
	$(CC) $(LSCRIPT) $(LFLAGS) $^ -o $@

$(OUTDIRS)/%.o: src/%.cpp
	@mkdir -p $(OUTDIRS)
	$(CC) $(CFLAGS) $(OPT) -c $^ -o $@

	
clean:
	rm -rf $(foreach D, $(SRCDIRS), $(wildcard $(D)*.o)) *.elf *.map $(OUTDIRS) bin

dump: $(BINARY)
	arm-none-eabi-objdump -xS -C arm $^

openocd:
	openocd -f /usr/share/openocd/scripts/board/st_nucleo_f4.cfg 

gdb:
	arm-none-eabi-gdb $(BINARY)