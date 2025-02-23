# Toolchain configuration
CC=arm-none-eabi-g++
OBJDUMP := arm-none-eabi-objdump
GDB := arm-none-eabi-gdb

#Project configuration
TARGET ?= bootloader
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

# Build type (debug/release)
BUILD_TYPE ?= debug
ifeq ($(BUILD_TYPE), debug)
	OPT := -O0 -g3
else ifeq ($(BUILD_TYPE), release)
	OPT := -O2
endif

# Directory structure
INCDIRS := inc \
		   inc/Utility \
		   inc/Drivers \
		   inc/target
SRCDIRS := src \
 	 	   src/Utility \
		   src/Drivers

# Compiler flags
CPU_FLAGS := -mcpu=cortex-m4 -mthumb
WERROR ?=
WARNING_FLAGS := -Wall -Wextra $(WERROR)
CPP_FLAGS := -fno-exceptions -fno-rtti
INCLUDES := $(addprefix -I, $(INCDIRS))
CFLAGS := $(CPU_FLAGS) \
		  $(WARNING_FLAGS) \
		  $(CPP_FLAGS) \
		  $(INCLUDES) \
		  $(OPT)

# Linker configuration
LDSCRIPT := linker.ld
LDFLAGS := -nostartfiles \
		   -specs=nano.specs \
		   -specs=nosys.specs \
		   -T$(LDSCRIPT) \
		   -Wl,-Map=$(BIN_DIR)/$(TARGET).map

# Find source files and generate object file names
CPPFILES := $(foreach D, $(SRCDIRS), $(wildcard $(D)/*.cpp))
OBJECTS=$(addprefix $(OBJ_DIR)/, $(notdir $(CPPFILES:%.cpp=%.o)))

# Output files
ELF := $(BIN_DIR)/$(TARGET).elf
BIN := $(BIN_DIR)/$(TARGET).bin
HEX := $(BIN_DIR)/$(TARGET).hex

# Default target
all: $(ELF) $(BIN) $(HEX)

# Create directories
$(BUILD_DIR):
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

# Compile
VPATH := $(SRCDIRS)
$(OBJ_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo "Compiling: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
$(ELF): $(OBJECTS)
	@echo "Linking: $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "Size of executable"
	@size $@

$(BIN): $(ELF)
	@arm-none-eabi-objcopy -O binary $< $@

$(HEX): $(ELF)
	@arm-none-eabi-objcopy -O ihex $< $@

.PHONY: clean debug release openocd minicom dump list-ports

debug:
	$(MAKE) BUILD_TYPE=debug

release:
	$(MAKE) BUILD_TYPE=release

clean:
	rm -rf $(BUILD_DIR)

dump: $(ELF)
	$(OBJDUMP) -xS -C arm $^

# Paths where OpenOCD config files might be located
OPENOCD_PATHS ?= /usr/share/openocd/scripts/board \
                 /usr/local/share/openocd/scripts/board \
                 $(HOME)/.local/share/openocd/scripts/board

# Define the target board
BOARD ?= st_nucleo_f4

# Find the board configuration file
OPENOCD_CFG := $(firstword $(foreach dir,$(OPENOCD_PATHS),$(wildcard $(dir)/$(BOARD).cfg)))

openocd:
ifeq ($(OPENOCD_CFG),)
	@echo "Error: Could not find $(BOARD).cfg in any of these locations:"
	@$(foreach dir,$(OPENOCD_PATHS),echo "  - $(dir)";)
	@exit 1
else
	@echo "Using OpenOCD config: $(OPENOCD_CFG)"
	openocd -f $(OPENOCD_CFG)
endif

gdb: $(ELF)
	$(GDB) $^


# Function to find ST-Link device using udevadm
# Filters for ST-Link V2.1 which is used on Nucleo boards
define find_stlink_port
$(shell for dev in /dev/ttyACM*; do \
	if udevadm info -q property "$$dev" 2>/dev/null | grep -q "ST-LINK"; then \
		echo "$$dev"; \
		break; \
	fi; \
done)
endef

# Try to find ST-Link specifically, fallback to any ACM device
TTY_PORT ?= $(or $(call find_stlink_port),$(firstword $(wildcard /dev/ttyACM*)))

# Baud rate can be overridden from command line
BAUD_RATE ?= 9600

minicom:
ifeq ($(TTY_PORT),)
	@echo "Error: No ST-Link or ACM devices found. Is your board connected?"
	@echo "Available serial devices:"
	@ls /dev/tty* 2>/dev/null | grep -E "USB|ACM" || echo "  None"
	@echo "USB devices found:"
	@lsusb | grep -i "ST-Link" || echo "  No ST-Link found"
	@exit 1
else
	@echo "Connecting to $(TTY_PORT) at $(BAUD_RATE) baud"
	@if ! which minicom >/dev/null; then \
		echo "Error: minicom is not installed"; \
		echo "Install it with: sudo apt-get install minicom"; \
		exit 1; \
	fi
	minicom -D $(TTY_PORT) -b $(BAUD_RATE)
endif


list-ports:
	@echo "Available serial devices:"
	@for dev in /dev/ttyACM* /dev/ttyUSB*; do \
		if test -e "$$dev"; then \
			echo -n "$$dev: "; \
			udevadm info -q property "$$dev" 2>/dev/null | grep -E "ID_MODEL=|ID_VENDOR=|ID_SERIAL=" || echo "Unknown device"; \
		fi; \
	done