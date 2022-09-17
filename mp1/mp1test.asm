; Created by Wu Feiyang on Sep 17th, 2022
; this scrpit is used for testing mp1


.ORIG x3000
LD      R0,LINE
AND     R1,R1,#0
JSR     PRINT_SLOT
OUT
ADD     R1,R1,#2
JSR     PRINT_SLOT
OUT
ADD     R1,R1,#1
JSR     PRINT_SLOT
OUT
ADD     R1,R1,#1
JSR     PRINT_SLOT
OUT
ADD     R1,R1,#3
JSR     PRINT_SLOT
OUT
ADD     R1,R1,#4
JSR     PRINT_SLOT
OUT
ADD     R1,R1,#2
JSR     PRINT_SLOT
OUT
ADD     R1,R1,#2
JSR     PRINT_SLOT
OUT
LEA     R1,STR1
JSR     PRINT_CENTERED
OUT
LEA     R1,STR2
JSR     PRINT_CENTERED
OUT
LEA     R1,STR3
JSR     PRINT_CENTERED
OUT
LEA     R1,STR4
JSR     PRINT_CENTERED
OUT
LEA     R1,STR5
JSR     PRINT_CENTERED
OUT
LEA     R1,STR6
JSR     PRINT_CENTERED
OUT
LEA     R1,STR7
JSR     PRINT_CENTERED
OUT
HALT
STR1        .STRINGZ "wfy"
STR2        .STRINGZ "wo"
STR3        .STRINGZ "Ultraman Tiga"  
STR4        .STRINGZ "1"
STR5        .STRINGZ "1234"
STR6        .STRINGZ "123456"
STR7        .STRINGZ "12345"
LINE        .FILL   x7c
; end of main program

PRINT_SLOT
    ; Register Table at subroutine PRINT_SLOT
    ; R0 character ascii register 
    ; R2 universal iterator(meaning changes in different loop)

    ST  R0,  PS_REG_0
    ST  R2,  PS_REG_2   
    ST  R7,  PS_REG_7 
    ADD R2,R1,#7
    LD  R0,   ZERO
    ADD R2,R2,#-10
    BRn PS_ONE_DIGIT
    ADD R2,R2,#-10  
    BRn TEEN    
    ADD R0,R0,#2        ;time number equal or greater than 20
    OUT
    ADD R0,R0,#-2
    ADD R0,R2,R0
    OUT
    BR PS_RRE_RETURN  
TEEN            ;time numbers from 10 ~ 19
    ADD R2,R2,#10   ;restore R2
    ADD R0,R0,#1
    OUT
    ADD R0,R0,#-1
    ADD R0,R2,R0
    OUT
    BR PS_RRE_RETURN 
PS_ONE_DIGIT    ;numbers with only one digit 7~9
    ADD R2,R2,#10   ;restore R2
    OUT
    ADD R0,R2,R0
    OUT
PS_RRE_RETURN ;prepare to return
    LD  R0,COL
    OUT
    LD  R0,ZERO
    OUT
    OUT
    LD  R0,SPACE
    OUT
      
BRnzp END_PRINT_SLOT
PS_REG_0  .BLKW   1
PS_REG_2  .BLKW   1
PS_REG_7  .BLKW   1
COL       .FILL   x3a
SPACE     .FILL   x20
ZERO      .FILL   x30

END_PRINT_SLOT 
    LD  R0, PS_REG_0 
    LD  R2, PS_REG_2
    LD  R7, PS_REG_7 
RET
;end of PRINT_SLOT

; start of PRINT_CENTERED
PRINT_CENTERED
    ; Register Table at subroutine PRINT_SLOT
    ; R0 number of trailing sapces and sometimes terator
    ; R1 universal iterator(meaning changes in different loop)
    ; R2 universal iterator(meaning changes in different loop)
    ; R3 number of characters to be aligned in the middle
    ; R4 number of leading sapces and sometimes iterator

    ST  R0,  PC_REG_0
    ST  R1,  PC_REG_1
    ST  R2,  PC_REG_2 
    ST  R3,  PC_REG_3
    ST  R4,  PC_REG_4
    ST  R7,  PC_REG_7 
    AND R3,R3,#0
LOOP1   
    LDR R2,R1,#0
    BRz CONTINUE        ; contine
    ADD R1,R1,#1        ; point to the next location
    ADD R3,R3,#1        ; R3: number of characters
    BR  LOOP1           ; loop
CONTINUE
    ADD R1,R3,#-6
    BRn LESS
    ;if more than or equal to 6 characters
    AND R2,R2,#0
    ADD R2,R2,#6        ;iterator of the 6 characters
    LD  R1,  PC_REG_1
    INLOOP  
        LDR R0,R1,#0
        OUT
        ADD R1,R1,#1
        ADD R2,R2,#-1
        BRp INLOOP
    BR END_PRINT_CENTERED
LESS   ; less than 6 characters
    AND R4,R4,#0
    ADD R4,R4,#-1  ; R4 set to -1      
    AND R1,R1,#0
    ADD R1,R1,#-6  ; R1 is now negative value of total number of characters
    ADD R1,R1,R3   ; R1 is now negative value of total number of spaces
    NOT R1,R1
    ADD R1,R1,#1   ; R1 is now value of total number of spaces
LOOP3              ; executing right shift here, R4 should be number of int(characters/2)
    ADD R4,R4,#1 
    NOT R2,R4
    ADD R2,R2,#1  ; R2<-(-R4)
    ADD R2,R2,R2  ; R2<-(-2*R4)
    ADD R2,R1,R2  ; R2=R1-2*R4, if finished, the remain R2 will only be 0 or 1
    ADD R2,R2,#-1 ; set R2 to -1 or 0
    BRp LOOP3
    ; now R4 store the number of leading sapces
    LD  R0,PCSAPCE
    ADD R2,R4,#0  ;store R4 in R2
    BRz NO_LEADING_GO_HERE  ; if no leading spaces than skip printing
LOOP4 ; print leading sapces
    OUT
    ADD R2,R2,#-1
    BRp LOOP4
NO_LEADING_GO_HERE
    LD  R1,PC_REG_1
    ADD R2,R3,#0
LOOP5  ;print characters
    LDR R0,R1,#0
    OUT
    ADD R1,R1,#1
    ADD R2,R2,#-1
    BRp LOOP5   
    AND R1,R1,#0    ; start calculating trailing spaces
    ADD R1,R1,#-6
    ADD R1,R1,R4
    ADD R1,R1,R3 ; R1's inverse value is the number of trailing sapces
    NOT R1,R1
    ADD R1,R1,#1 ; now R1 stores the number of trailing sapces
    LD R0,PCSAPCE
LOOP6   ; print trailing sapces
    OUT
    ADD R1,R1,#-1
    BRp LOOP6
    BR  END_PRINT_CENTERED
PC_REG_0  .BLKW   1
PC_REG_1  .BLKW   1
PC_REG_2  .BLKW   1
PC_REG_3  .BLKW   1
PC_REG_4  .BLKW   1
PC_REG_7  .BLKW   1
PCSAPCE   .FILL   x20
END_PRINT_CENTERED 
    LD  R0,  PC_REG_0
    LD  R1,  PC_REG_1
    LD  R2,  PC_REG_2
    LD  R3,  PC_REG_3
    LD  R4,  PC_REG_4
    LD  R7,  PC_REG_7 
    RET
.END