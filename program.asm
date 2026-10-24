section .data

    a db "vaibhav"
    b dd 15
    c dw 300
    d db 25

section .text

    jmp calculate       ; Forward reference

start:
    mov eax, 15
    mov ebx, 25
    add eax, ebx

calculate:
    mov ecx, 10
    add eax, ecx

    jmp start           ; Backward reference
