TARGET = clock
HEX = $(TARGET).hex

CSRC = main.c
CARGS = -mmcs51
CARGS += --iram-size 256
CARGS += --xram-size 0
CARGS += --code-size 2048
CARGS += --nooverlay
CARGS += --nointroduction
CARGS += --verbose
CARGS += --debug
CARGS += -V
CARGS += --std-sdcc99
CARGS += --model-small
INCLUDES = -Iinclude

PROG = stcgal
PROGARGS = -P stc12a
PROGARGS += -p /dev/ttyUSB0
PROGARGS += -b 9600
PROGARGS += -l 2400
PROGARGS += -o clock_source=external

BUILD = build
CC = sdcc
CP = CP
MV = mv
RM = rm -rf
MKDIR = mkdir
MOECHO = @

.PHONY: all clean

all: $(BUILD)
	$(CC) $(CARGS) $(INCLUDES) $(CSRC) -o $(BUILD)/$(HEX)

clean:
	$(RM) $(BUILD)

$(BUILD):
	$(MKDIR) $(BUILD)

flash: all
	$(PROG) $(PROGARGS) $(BUILD)/$(HEX)