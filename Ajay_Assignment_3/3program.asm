section .data

    a db "Ajay"
    b dd 15
    c dw 300
    d db 25

section .text

    mov eax, 15
    mov ebx, 25
    mov ecx, 10
    mov edx, 50
    add eax, ebx
    sub edx, ecx
