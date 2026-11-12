
section .text
global main

main:
    mov esi, 50
    mov edi, 20

    mov eax, esi
    mov ebx, edi
    add eax, ebx
    mov ecx, eax
    mov eax, esi
    mov ebx, edi
    sub eax, ebx
    mov edx, eax

    ret
