# Guest program build system
#
# Each CPU family has its own build rule.
# If no assembler is available, binaries are generated from inline hex data.
#
# Build all:   make
# Build one:   make z80/halt.bin

.PHONY: all clean z80 m6502

all: z80/halt.bin m6502/halt.bin

# --- Z80 ---

z80/halt.bin: z80/halt.asm
	@if command -v sdasz80 >/dev/null 2>&1 && command -v objcopy >/dev/null 2>&1; then \
		sdasz80 -o $*.rel $< && \
		sdldz80 -i $*.ihx $*.rel >/dev/null 2>&1 && \
		objcopy -I ihex -O binary $*.ihx $@ && \
		rm -f $*.rel $*.ihx $*.lst $*.map $*.sym; \
	elif command -v zasm >/dev/null 2>&1; then \
		zasm -o $@ $<; \
	else \
		echo "WARNING: no Z80 assembler — using fallback for $@"; \
		python3 -c "open('$@','wb').write(bytes([0x76, 0xC3, 0x00, 0x06]))"; \
	fi

# --- MOS 6502 ---

m6502/halt.bin: m6502/halt.asm
	@if command -v xa65 >/dev/null 2>&1; then \
		xa65 -o $@ $<; \
	elif command -v vasm6502_oldstyle >/dev/null 2>&1; then \
		vasm6502_oldstyle -dotdir -Fbin -o $@ $<; \
	else \
		echo "WARNING: no 6502 assembler — using fallback for $@"; \
		python3 -c "open('$@','wb').write(bytes([0x00, 0x4C, 0x00, 0x06]))"; \
	fi

# --- Clean ---

clean:
	rm -f z80/halt.bin z80/*.rel z80/*.ihx z80/*.lst z80/*.map z80/*.sym m6502/halt.bin
