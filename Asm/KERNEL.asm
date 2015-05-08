[BITS 16]

[global start]
[extern main] ; this is in the c file

start:
  call main

  cli  ; stop interrupts
  hlt ; halt the CPU

TIMES 510 - ($ - $$) db 0 ;fill resting bytes with zero
DW 0xAA55 ;end of bootloader (2 bytes)