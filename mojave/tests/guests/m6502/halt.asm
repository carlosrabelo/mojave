; MOS 6502 halt program (BRK loop)
; Requires: vasm6502_oldstyle or xa65
;
; vasm6502_oldstyle -dotdir -Fbin -o halt.bin halt.asm
; xa65 -o halt.bin halt.asm

        .org $0600

loop:
        brk
        jmp loop
