%macro myadd 3
    mov eax, %1
    mov ebx, %2
    add eax, ebx
    mov %3, eax
%endmacro

%macro mysub 3
    mov eax, %1
    mov ebx, %2
    sub eax, ebx
    mov %3, eax
%endmacro

section .text
global main

main:
    mov esi, 50
    mov edi, 20

    myadd esi, edi, ecx
    mysub esi, edi, edx

    ret
