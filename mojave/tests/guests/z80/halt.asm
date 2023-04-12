        .module halt
        .globl main
        .area _CODE

main:
        halt
        jp main
