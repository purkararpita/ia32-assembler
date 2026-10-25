; Assignment 5 - Flag Register Testing
; 8-bit IA-32 arithmetic examples

MOV AL, 5
ADD AL, 3       ; Result = 8

MOV AL, 5
SUB AL, 5       ; Result = 0, ZF = 1

MOV AL, 255
ADD AL, 1       ; Result = 0, CF = 1

MOV AL, 127
ADD AL, 1       ; Signed overflow, OF = 1

MOV AL, 5
CMP AL, 5       ; Equal, ZF = 1
