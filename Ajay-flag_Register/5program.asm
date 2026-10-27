;ass5 -flag register
; 8-bit IA-32 arithmetic ex

MOV AL, 5
ADD AL, 3   ;RESULT =8

MOV AL, 5
SUB AL, 5   ; RESULT =0,ZF =1

MOV AL, 255
ADD AL, 1   ;RESULT =0, CF =1

MOV AL, 127
MOV AL, 1   ; SIGNED OVERFLOW, OF =1

MOV AL, 5
CPM AL, 5    ; EQUAL, ZF =1
~                                                                                                                                                       
~                                                                                                                                                       
~                                                                                                                                                       
~                                                                                                                                                       
~                    
