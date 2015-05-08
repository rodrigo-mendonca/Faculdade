[BITS 16]
[ORG 0x7C00]

MOV DL, 0x80 ;drive 0 = HD 80
MOV DH, 0x0 ;head (0=base)
MOV CH, 0x0 ;track/cylinder
MOV CL, 0x02 ;sector (1=bootloader, apparently sectors starts counting at 1 instead of 0)
MOV BX, 0x1000 ;place in RAM for kernel - I suppose randomly chosen on examples
MOV ES, BX ;place BX in pointer ES
MOV BX, 0x0 ;back to zero - also has something to do with RAM position

ReadHD:
MOV AH, 0x02
MOV AL, 0x18
INT 0x13
JC ReadHD ;if it went wrong, try again

;pointers to RAM position (0x1000)
MOV AX, 0x1000
MOV DS, AX
MOV ES, AX
MOV FS, AX
MOV GS, AX
MOV SS, AX

JMP 0x1000:0x0

;assuming we get never back here again, so no further coding needed (kernel handles everything now)

TIMES 510 - ($ - $$) db 0 ;fill resting bytes with zero
DW 0xAA55 ;end of bootloader (2 bytes)