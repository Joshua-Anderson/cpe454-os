global long_mode_start
extern kmain

section .text
bits 64
long_mode_start:
    ; load 0 into all data segment registers
    mov dx, 0
    mov ss, dx
    mov ds, dx
    mov es, dx
    mov fs, dx
    mov gs, dx

    ; start kernel c code
    mov edi, eax
    mov esi, ebx
    call kmain
    hlt
