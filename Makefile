TARGET = clock
HEX = $(TARGET).hex

CSRCS = main.c 1.c 2.c
OBJS = $(CSRCS:%.c=$(BUILD)/%.rel)

CARGS  = -mmcs51
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

OBJARGS  = -mmcs51
OBJARGS += --iram-size 256
OBJARGS += --xram-size 0
OBJARGS += --code-size 2048
OBJARGS += --nooverlay
# OBJARGS += --nointroduction
OBJARGS += --verbose
OBJARGS += --debug
OBJARGS += -V
OBJARGS += --std-sdcc99
OBJARGS += --model-small

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

all: $(BUILD) $(OBJS)
	$(CC) $(OBJARGS) $(INCLUDES) $(OBJS) -o $(BUILD)/$(HEX)

$(OBJS):$(BUILD)/%.rel:%.c
	$(CC) $(CARGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(BUILD)

$(BUILD):
	$(MKDIR) $(BUILD)

flash: all
	$(PROG) $(PROGARGS) $(BUILD)/$(HEX)