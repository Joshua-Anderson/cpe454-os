%macro irq_handle_code 1
  global irq_%1
  irq_%1:
    ; Don't change the order of this without updating thread loading and saving order
    push rdi
    push rsi
    push rdx
    push rax
    push rbx
    push rcx
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov edi, %1
    mov esi, [rsp+120]
    mov rdx, rsp
    add rdx, 128
    call irq_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rcx
    pop rbx
    pop rax
    pop rdx
    pop rsi
    pop rdi
    add rsp, 8 ; Remove Error Code From Stack
    iretq
%endmacro

%macro irq_handle_wo_code 1
  global irq_%1
  irq_%1:
    ; Don't change the order of this without updating thread loading and saving order
    push rdi
    push rsi
    push rdx
    push rax
    push rbx
    push rcx
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov edi, %1
    mov esi, 0
    mov rdx, rsp
    add rdx, 120
    call irq_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rcx
    pop rbx
    pop rax
    pop rdx
    pop rsi
    pop rdi
    iretq
%endmacro

extern irq_handler

section .text
bits 64

irq_handle_wo_code 0
irq_handle_wo_code 1
irq_handle_wo_code 2
irq_handle_wo_code 3
irq_handle_wo_code 4
irq_handle_wo_code 5
irq_handle_wo_code 6
irq_handle_wo_code 7
irq_handle_code 8
irq_handle_wo_code 9
irq_handle_code 10
irq_handle_code 11
irq_handle_code 12
irq_handle_code 13
irq_handle_code 14
irq_handle_wo_code 15
irq_handle_wo_code 16
irq_handle_wo_code 17
irq_handle_wo_code 18
irq_handle_wo_code 19
irq_handle_wo_code 20
irq_handle_wo_code 21
irq_handle_wo_code 22
irq_handle_wo_code 23
irq_handle_wo_code 24
irq_handle_wo_code 25
irq_handle_wo_code 26
irq_handle_wo_code 27
irq_handle_wo_code 28
irq_handle_wo_code 29
irq_handle_wo_code 30
irq_handle_wo_code 31
irq_handle_wo_code 32
irq_handle_wo_code 33
irq_handle_wo_code 34
irq_handle_wo_code 35
irq_handle_wo_code 36
irq_handle_wo_code 37
irq_handle_wo_code 38
irq_handle_wo_code 39
irq_handle_wo_code 40
irq_handle_wo_code 41
irq_handle_wo_code 42
irq_handle_wo_code 43
irq_handle_wo_code 44
irq_handle_wo_code 45
irq_handle_wo_code 46
irq_handle_wo_code 47
irq_handle_wo_code 48
