SDCC     ?= sdcc
BUILD    := build
CFLAGS   := -mstm8 --std-c99 --opt-code-size -Isrc
LDFLAGS  := -mstm8 -Wl-bDATA=0x0008
SRC      := $(wildcard src/*.c)
REL      := $(patsubst src/%.c,$(BUILD)/%.rel,$(SRC))
HDR      := $(wildcard src/*.h)

all: $(BUILD)/clock.ihx

$(BUILD)/%.rel: src/%.c $(HDR)
	mkdir -p $(BUILD)
	$(SDCC) $(CFLAGS) -c -o $@ $<

$(BUILD)/clock.ihx: $(REL)
	$(SDCC) $(LDFLAGS) -o $@ $(REL)

flash: $(BUILD)/clock.ihx
	tools/bin/stm8flash -c stlink -p stm8l152c6 -w $<

clean:
	rm -rf $(BUILD)

.PHONY: all flash clean
