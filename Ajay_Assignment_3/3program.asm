section .data

    a db "Ajay"
    b dd 15
    c dw 300
    d db 25

section .text

    jmp calculate       ; Forward reference

start:
    mov eax, 15
    mov ebx, 25
    mov ecx, 10
    mov edx, 50
    add eax, ebx
    sub edx, ecx

calculate:
    mov eax, 10
    jmp start           ; Backward reference
