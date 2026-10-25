section .data
    a db "Ajay"
    b dd 15
    c dw 300
    d db 25

section .text
    jmp calculate       ; Forward reference

start:
    mov eax, 25
    mov ebx, 10
    add eax, ebx

calculate:
    mov ecx, 10
    add eax, ecx
    jmp start           ; Backward reference
