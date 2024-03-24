CC=arm-none-eabi-g++
OPT=-O0
INCDIRS=. ./inc/ ./inc/Utility/
SRCDIRS=. ./src/ ./src/Utility/
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
 $(info CPPFILES = $(CPPFILES))
 $(info OBJECTS = $(OBJECTS))


all: $(BINARY)

compile: $(OBJECTS)

link: $(BINARY)


$(BINARY): $(OBJECTS)
	@mkdir -p bin
	$(CC) $(LSCRIPT) $(LFLAGS) $^ -o $@


VPATH := $(dir $(CPPFILES))

$(OUTDIRS)/%.o: %.cpp
	@mkdir -p $(OUTDIRS)
	$(CC) $(CFLAGS) $(OPT) -c $^ -o $@
	

clean:
	rm -rf out
	rm -rf *.bin 
	rm -rf *.elf
	rm -rf bin 


dump: $(BINARY)
	arm-none-eabi-objdump -xS -C arm $^

openocd:
	openocd -f /usr/share/openocd/scripts/board/st_nucleo_f4.cfg 

gdb:
	arm-none-eabi-gdb $(BINARY)

minicom:
	minicom -D /dev/ttyACM0

.PHONY: clean openocd gdb minicom dump