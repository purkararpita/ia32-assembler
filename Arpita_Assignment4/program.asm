section .data

    a db "Arpita"
    b dd 12
    c dw 100
    d db 35

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
