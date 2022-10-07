.ORIG   x3000
; Initialise the array at x4000
    LD      R1,ARRAY
    LD      R2,NULL
    LD      R3,SLOT_NUMBER
MAIN_LOOP1      ; clear the location beginning at x4000
    STR     R2,R1,#0
    ADD     R1,R1,#1
    ADD     R3,R3,#-1
    BRp     MAIN_LOOP1

; read event list

    LD      R1,EVENT_LIST
    RETURN1
    ADD     R3,R1,#0        ; R3 points to the first character, copy the first value of R1
    MAIN_LOOP2
    ; start to process event list
    LDR     R2,R1,#0    ; store the current character to R2
    BRz     HANDEL      ; if end of character, go to find its slot pointer
    ADD     R1,R1,#1
    BR      MAIN_LOOP2

END_FILLING
    
    
    ; printing the first line

    ;to be deleted here
    ; LEA     R1,NULL
    ; JSR     PRINT_CENTERED
    ; LD      R0,SEPARATION
    ; OUT
    ; LEA     R1,MON
    ; JSR     PRINT_CENTERED
    ; OUT
    ; LEA     R1,TUE
    ; JSR     PRINT_CENTERED
    ; OUT
    ; LEA     R1,WED
    ; JSR     PRINT_CENTERED
    ; OUT
    ; LEA     R1,THU
    ; JSR     PRINT_CENTERED
    ; OUT
    ; LEA     R1,FRI
    ; JSR     PRINT_CENTERED
    ; LD      R0,NL
    ; OUT

    LEA     R1,NULL     ; starts printing the timetable from here
    JSR     PRINT_CENTERED
    LD      R0,SEPARATION
    OUT     
    AND     R2,R2,#0
    ADD     R2,R2,#5    ; iterator for the 5 days of the week
    LEA     R1,MON
MAIN_LOOP5
    JSR     PRINT_CENTERED
    OUT
    ADD     R1,R1,#1
    ADD     R2,R2,#-1
    BRnp    MAIN_LOOP5      

    ; print rest of the lines
    LD      R2,ARRAY
    AND     R5,R5,#0        ; time iterator
    MAIN_LOOP4
    LD      R0,SEPARATION
    AND     R3,R3,#0
    ADD     R3,R3,#5        ; day iterator
    ADD     R1,R5,#0
    JSR     PRINT_SLOT
    MAIN_LOOP3
    OUT
    LDR     R1,R2,#0
    BRnp    SKIP1
    LEA     R1,NULL
    SKIP1
    JSR     PRINT_CENTERED  
    ADD     R2,R2,#1
    ADD     R3,R3,#-1
    BRp     MAIN_LOOP3  ; current time not over
    ; current time over, continue next line
    LD      R0,NL
    OUT     
    ADD     R5,R5,#1
    AND     R4,R4,#0
    ADD     R4,R4,#-16
    ADD     R4,R4,R5
    BRn     MAIN_LOOP4 ; time iteration not finished
END_PROGRAM
    HALT


; lookup table starts here
ARRAY           .FILL   x4000
EVENT_LIST      .FILL   x5000
NULL            .FILL   x0000
SLOT_NUMBER     .FILL   x0050
MON             .STRINGZ    "Mon"
TUE             .STRINGZ    "Tue"
WED             .STRINGZ    "Wed"
THU             .STRINGZ    "Thu"
FRI             .STRINGZ    "Fri"
SEPARATION      .FILL       x007c   ;"|"
NL              .FILL       x000a   ;"\n"
INVALID_SLOT    .STRINGZ    " has an invalid slot number.\n"
CONFLICT        .STRINGZ    " conflicts with an earlier event.\n"

HANDEL  ; find the slot pointer of the character
    ADD     R1,R1,#1
    LDR     R2,R1,#0    ; day information store to R2
    ADD     R1,R1,#1  
    LDR     R4,R1,#0    ; time information store to R4
    BR     JUDGE_INVALID_SLOT
    JUDGE_INVALID_SLOT_RETURN
    AND     R5,R5,#0    ; iterate 5 times 
    ADD     R5,R5,#4
    ADD     R2,R2,R2    ; move the first bit to the front
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    ADD     R2,R2,R2
    HANDEL_LOOP1
    ADD     R2,R2,R2    ; Now the current day digit is on the first digit
    BRn     HANDEL_STORE          ; if this is the day of the event, store to the characters to the location of the pointer
    HANDEL_RETURN1
    ADD     R5,R5,#-1   ; decrement day of the week
    BRzp    HANDEL_LOOP1
    ADD     R1,R1,#1
    LDR     R2,R1,#0
    BRz     END_FILLING    ; if next location is NULL, this means that the initialization is over
    BR      RETURN1
    
    HANDEL_STORE
    ; TODO: to be finished, I just need to put the address of R3 in to the array
    ; R3 is the initial address of characters
    ; R4 is the current time
    ; Do not change R1, R2, R5

    AND     R6,R6,#0
    AND     R0,R0,#0
    ADD     R6,R6,#4        ; one line containing 4+1=5 pointers
    ; multiplication and adding here to get R0=R4*16+R5+(x4000)
    HANDEL_STORE_LOOP
    ADD     R0,R0,R4
    ADD     R6,R6,#-1
    BRzp     HANDEL_STORE_LOOP     
    ADD     R0,R0,R5
    LD      R6,ARRAY
    ADD     R0,R0,R6
    ; check if the content in location R0 is empty
    LDR     R6,R0,#0
    BRnp    CONFLICT_TRUE
    ; store character beginning address R3 into location R0
    STR     R3,R0,#0
    BR      HANDEL_RETURN1

    
    
JUDGE_INVALID_SLOT
    ADD     R4,R4,#-15
    BRp     JUDGE_INVALID_SLOT_TRUE
    ADD     R4,R4,#15
    BRn     JUDGE_INVALID_SLOT_TRUE
    BR  JUDGE_INVALID_SLOT_RETURN
    JUDGE_INVALID_SLOT_TRUE
    ADD     R0,R3,#0
    PUTS
    LEA      R0,INVALID_SLOT
    PUTS
    BR      END_PROGRAM
    
CONFLICT_TRUE
    ADD     R0,R3,#0
    PUTS
    LEA     R0,CONFLICT
    PUTS
    BR      END_PROGRAM


; PRINT_SLOT -- Pass a number to R1 and print the time according to the value of R1
; INPUT: R1, a time period from 0 ~ 15 that matches 7:00 ~ 22:00
; OUTPUT: Print time string from "7:00 " to "22:00 " with a trailing space

; OTHER REGISTERS: callee-saved (except for R7)
; SIDE EFFECTS: waits for user to press a key, echos the keystroke to the
;               display, and consumes the keystroke

; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - character ascii register / iterator
;   R2 universal iterator(meaning changes in different loop)

PRINT_SLOT

    ST  R0,  PS_REG_0
    ST  R2,  PS_REG_2   
    ST  R7,  PS_REG_7 
    ADD R2,R1,#7
    LD  R0,   ZERO      ; load ascii character 0
    ADD R2,R2,#-10
    BRn PS_ONE_DIGIT
    ADD R2,R2,#-10  
    BRn TEEN    
    ADD R0,R0,#2        ;time number equal or greater than 20
    OUT
    ADD R0,R2,R0
    ADD R0,R0,#-2
    OUT
    BR PS_RRE_RETURN  
TEEN                ;time numbers from 10 ~ 19
    ADD R2,R2,#10   ;restore the value of R2 before the condition judgement
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


; PRINT_CENTERED -- Pass the location of the first character of a string to R1 
; and align the string at the middle of a proccessed string whose maximum value
; is limited to 6, and string more then 6 characters long will take only the 6 on
; the front.
; INPUT: R1, a time period from 0 ~ 15 that matches 7:00 ~ 22:00
; OUTPUT: Print time string from "7:00 " to "22:00 " with a trailing space

; OTHER REGISTERS: callee-saved (except for R7)

; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - number of trailing sapces / iterator
;   R1 - universal iterator(meaning changes in different loop)
;   R2 - universal iterator(meaning changes in different loop)
;   R3 - number of characters to be aligned in the middle
;   R4 - number of leading sapces and sometimes iterator




PRINT_CENTERED
    ; Register Table at subroutine PRINT_SLOT
    

    ST  R0,  PC_REG_0
    ST  R1,  PC_REG_1
    ST  R2,  PC_REG_2 
    ST  R3,  PC_REG_3
    ST  R4,  PC_REG_4
    ST  R7,  PC_REG_7 
    AND R3,R3,#0
    LDR R2,R1,#0
    BRz DEAL0           ;if a string contains nothing in the form of "", execute here
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
DEAL0   ;deal with case when input strings is ""
    LEA R0, ALLSPACES
    PUTS
    BR END_PRINT_CENTERED
PC_REG_0  .BLKW   1
PC_REG_1  .BLKW   1
PC_REG_2  .BLKW   1
PC_REG_3  .BLKW   1
PC_REG_4  .BLKW   1
PC_REG_7  .BLKW   1
PCSAPCE   .FILL   x20
ALLSPACES .STRINGZ "      "     ; case when input string is ""
END_PRINT_CENTERED 
    LD  R0,  PC_REG_0
    LD  R1,  PC_REG_1
    LD  R2,  PC_REG_2
    LD  R3,  PC_REG_3
    LD  R4,  PC_REG_4
    LD  R7,  PC_REG_7 
    RET


.END