.ORIG   x3000

; Initialise the array at x4000

; mp3.asm
; in this program, it translate events with variable-length into a schedule with fixed-length fields.
; The first step is to initialize the schedule starting from x4000 to x404F. 
; The second step is translating, which is to read the event starting from x5000 and store 
; them on the table initialisied by the first step.
; The program loads an extra event and uses DFS search and a stack to determine if the extra event 
; can be filled into the time table.
; All events will be filled in the possible earliest time slot if not conflicting with other schedule.
; After loading the extra event, the program prints with day names on the top and hour names on the left.
; If the extra event is loaded with confict, the program will print only error message.
; The event list starts at address x5000 in LC-3 memory.
; The extra event starts at address starting at x6000
; The main Stack starts at address starting at x8000 and grows towards bigger addresses.
; mp3.asm uses to subroutines, PRINT_CENTERED, PRINT_SLOT, EXTRA_EVENT and FIND_DAY.


; Initialization
; REGISTER USE (registers have multiple uses in different parts of code):
;   R1 - pointer to the current address the array  
;   R2 - ascii value of NULL(x0000)
;   R3 - slot iterator
    LD      R1,ARRAY
    LD      R2,NULL
    LD      R3,SLOT_NUMBER
MAIN_LOOP1      ; clear the location beginning at x4000
    STR     R2,R1,#0
    ADD     R1,R1,#1
    ADD     R3,R3,#-1
    BRp     MAIN_LOOP1

;Translation
; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - character ascii register / iterator
;   R1 - pointer to the current address the event list starting at x5000 
;   R2 - current character ascii
;   R3 - the first adress of each event
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
    JSR EXTRA_EVENT         ; load extra event

    LD      R1,JUDGE_PRINT
    BRz     END_PROGRAM     ;if that slot is zero, end the program, if slot is non zero, contine printing

;Printing
; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - character ascii register / iterator
;   R1 - pointer to the current address the event list starting at x5000 
;   R2 - current character ascii
;   R3 - week days iterator  
;   R4 - store the number -16
;   R5 - day time iterator 

    LEA     R1,NULL     ; starts printing the timetable from here
    JSR     PRINT_CENTERED
    LD      R0,SEPARATION
    OUT     
    AND     R2,R2,#0
    ADD     R2,R2,#4    ; iterator for the 4 days of the week，last day don't have a vertical line so need another case
    LEA     R1,MON
MAIN_LOOP5
    JSR     PRINT_CENTERED
    OUT
    ADD     R1,R1,#4    ; every week day has three characters and a NULL
    ADD     R2,R2,#-1   ; decrease the number of weekdays to be printed
    BRnp    MAIN_LOOP5
    JSR     PRINT_CENTERED
    LD      R0,NL
    OUT      

    ; print rest of the lines
    LD      R2,ARRAY        ; R2 points to the current address of the array
    AND     R5,R5,#0        ; time iterator
    MAIN_LOOP4
    LD      R0,SEPARATION   ; "|"
    AND     R3,R3,#0
    ADD     R3,R3,#5        ; day iterator
    ADD     R1,R5,#0
    JSR     PRINT_SLOT
    MAIN_LOOP3
    OUT
    LDR     R1,R2,#0
    BRnp    SKIP1           ; judge if the location is full
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
    HALT        ; end of the program


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
    ADD     R2,R2,R2    ; move several times
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
    BRn     HANDEL_STORE   ; if this is the day of the event, store to the characters to the location of the pointer
    HANDEL_RETURN1
    ADD     R5,R5,#-1   ; decrement day of the week
    BRzp    HANDEL_LOOP1
    ADD     R1,R1,#1
    LDR     R2,R1,#0
    BRz     END_FILLING    ; if next location is NULL, this means that the initialization is over
    BR      RETURN1
    
    HANDEL_STORE            ; this part stores the current event into the array 

    AND     R6,R6,#0
    AND     R0,R0,#0
    ADD     R6,R6,#4        ; one line containing 4+1=5 pointers
    ; multiplication and adding here to get R0=R4*16+R5+(x4000)
    HANDEL_STORE_LOOP
    ADD     R0,R0,R4
    ADD     R6,R6,#-1
    BRzp     HANDEL_STORE_LOOP     ; go to handel store process
    ADD     R0,R0,R5
    LD      R6,ARRAY        ; laod the value into the array
    ADD     R0,R0,R6
    ; check if the content in location R0 is empty
    LDR     R6,R0,#0
    BRnp    CONFLICT_TRUE
    ; store character beginning address R3 into location R0
    STR     R3,R0,#0
    BR      HANDEL_RETURN1

    
    
JUDGE_INVALID_SLOT  ; determine if the schedule is valid
    ADD     R4,R4,#-15
    BRp     JUDGE_INVALID_SLOT_TRUE     ; time greater than 15, invalid
    ADD     R4,R4,#15
    BRn     JUDGE_INVALID_SLOT_TRUE     ; time smaller than 15, invalid
    BR  JUDGE_INVALID_SLOT_RETURN
    JUDGE_INVALID_SLOT_TRUE
    ADD     R0,R3,#0
    PUTS
    LEA      R0,INVALID_SLOT    ; print error message
    PUTS
    PUTS
    BR      END_PROGRAM
    
CONFLICT_TRUE       ; determine case of conflicts
    ADD     R0,R3,#0
    PUTS
    LEA     R0,CONFLICT    ; print error message conflict
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

    ST  R0,  PS_REG_0   ;store registers
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
    LD  R0,ZERO     ;prepare to print zero
    OUT
    OUT
    LD  R0,SPACE    ;prepare to print sapces
    OUT
      
BRnzp END_PRINT_SLOT        ;register restore
PS_REG_0  .BLKW   1
PS_REG_2  .BLKW   1
PS_REG_7  .BLKW   1
COL       .FILL   x3a       ;colons
SPACE     .FILL   x20       ;spaces
ZERO      .FILL   x30       ;ascii zero

END_PRINT_SLOT 
    LD  R0, PS_REG_0 
    LD  R2, PS_REG_2
    LD  R7, PS_REG_7 
RET
;end of PRINT_SLOT

JUDGE_PRINT     .FILL   xFFFF    ;location turns 0, not print the schedule

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

; EXTRA_EVENT -- Read the extra event and determine if the events can be printed
; INPUT: Event List starting at x6000
; OUTPUT: data stored starting at x4000
; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - temporary / output value
;   R1 - the pointer pointing to the eatra event list
;   R2 - temporary
;   R3 - the pointer pointing to the stack used to clear illegal events
;   R4 - event list iterator / temporary
;   R5 - bitmask / temporary
;   R6 - temporary
;   R7 - day information storage


; EVENT STRUCTURE USED ON THE STACK
; The structure uses 4 addresses
; First Address: Pointer to the event, same as in the event list
; Second Address: time to be taken, in the form of bits, the first 1 from the right implies 
; the next time to be taken
; the digit on the 0 position(the rightmost digit) represent 7:00 and so forth, 
; resembling the event list
; Third Address: Weekdays, same as in the event list
; Fourth Address: The current day taken, in the form of positive integers ranging from 0~15

EXTRA_EVENT
    ; store registers
    ST  R0,  EE_REG_0
    ST  R1,  EE_REG_1
    ST  R2,  EE_REG_2 
    ST  R3,  EE_REG_3
    ST  R4,  EE_REG_4
    ST  R5,  EE_REG_5
    ST  R6,  EE_REG_6
    ST  R7,  EE_REG_7 
    ; R1 contains the starting address of the origin
    LD  R1,EE_ORIG
    LD  R3,EE_STACK

EE_NEXT_EVENT 
    ; small stack initialise
    AND R4,R4,#0
    LEA R2,EE_CLEAR_ARRAY_STACK     ; claer the small stack
    STR R4,R2,#0
    LEA R2,EE_STACK_POINTER
    STR R4,R2,#0
    AND R4,R4,#0
    ADD R4,R4,#3    
    LDR R2,R1,#0    ; R2 contains the content in the current location in the extra event list
    BRz EE_PREP_RET ; if R2 contains NULL, it means we are finished

    AND R6,R6,#0    ; R6 is the skip event indicator

EE_LOOP1            ; copy the event into the stack---do not change R1,R3
    LDR R2,R1,#0    ; R2 contains the content in the current location in the extra event list
    BRz EE_SKIP_EVENT    ; if R2 contains NULL, it means we should skip the event
    EE_SKIP_EVENT_RET
    STR R2,R3,#0
    ADD R1,R1,#1
    ADD R3,R3,#1
    ADD R4,R4,#-1   ; copy three times 
    BRnp EE_LOOP1
    AND R5,R5,#0    ; the fourth location of the stack structure
    ADD R5,R5,#-1
    STR R5,R3,#0    ; initialise current time as -1, if current time selected this is non-negative
    ADD R3,R3,#1    ; next location on the stack

    ADD R6,R6,#0    ;set CC
    BRn EE_SKIP_EVENT_TRUE

SELECT_TIME_SLOT
; selct time slot

    AND R0,R0,#0    ; R0 is the 15 kinds of time iterator
    ADD R0,R0,#-1   ; starting at -1, so after the first loop gives 0
    ADD R3,R3,#-2   ; R3 points to the time
    LDR R2,R3,#0
    ADD R3,R3,#2
    AND R5,R5,#0
    ADD R5,R5,#1    ; R5 is the bit mask
EE_LOOP2
    ADD R0,R0,#1    
    AND R6,R5,R2
    BRnp TAKE_TIME  ; there is available time
    ADD R5,R5,R5
    BRz NO_SPARE_TIME ;if run out of bit mask(R5=0) then finish time select
    BR  EE_LOOP2
    TAKE_TIME
    ; if this time exist then take this time
    ADD R3,R3,#-2
    NOT R6,R6
    ADD R6,R6,#1
    ADD R6,R2,R6    ; we get R2-R6 so that time will disappear 
    STR R6,R3,#0    ; mask the time slot, if this time is not compatible it will never search this time
    ADD R3,R3,#2
    ST  R0,EE_SELECT_TIME
    ST  R0,TIME_INPUT       ; the time selected is stored in an address of the subroutine

; weekdays
    AND R0,R0,#0    ; R0 is the 5 days of time iterator
    ADD R0,R0,#-1
    AND R5,R5,#0
    ADD R5,R5,#1    ; R5 is the bit mask
EE_LOOP3
    ADD R3,R3,#-3   ; R3 points to the day
    LDR R7,R3,#0    ; R7 stores day information
    ADD R3,R3,#3
    ADD R0,R0,#1
    AND R6,R5,R7        ; R6 can be used for other purpose from now on
    ; R0,R1,R3,R5 can't be changed
    BRnp EE_SEARCHDAY   ; if this day has the event, look if there's conflicts; not conflicts, fill it in
    EE_SEARCHDAY_RET
    ADD R5,R5,R5
    BRnp    EE_LOOP3
    ; weekday exhausted
    LD  R0,EE_SELECT_TIME
    ADD R3,R3,#-1   ;points to the current day 
    STR R0,R3,#0    ; store the time
    ADD R3,R3,#1
    BR  EE_NEXT_EVENT

EE_SEARCHDAY        ; check if schedule is available
    ST  R0,DAY_INPUT
    JSR FIND_DAY
    LD  R4,FD_OUTPUT    ; we have found the location, ready to fill in numbers
    LDR R7,R4,#0
    BRnp    EE_INVALID_SLOT       ; this space is already filled!
    
    ; if this space is not filled, we can fill it
    ADD R2,R3,#0    ; get the pointer address of the main stack, the pointer is on the weekdays now
    ADD R2,R2,#-4   ; R2 is on the event address now
    LDR R6,R2,#0    ; first address of the event is put to R6, R6 point to the first of event string
    STR R6,R4,#0    ; fill the pointer in the array

    ; now we initailise a new stack to remeber which location in the array we have filled, so it will be easy to clean it.
    ; R4 store this array address
    LEA  R7,EE_CLEAR_ARRAY_STACK
    LD   R2,EE_STACK_POINTER
    ADD  R7,R2,R7   ; R7 is the initial postion of the stack
    STR  R4,R7,#0   ; Array address store in the stack
    ADD  R2,R2,#1   ; stack pointer increment by 1
    ST   R2,EE_STACK_POINTER    ; stack pointer store back to the adress
    BR EE_SEARCHDAY_RET
EE_SKIP_EVENT
    ADD R6,R6,#-1
    BR  EE_SKIP_EVENT_RET

EE_SKIP_EVENT_TRUE
    ADD R3,R3,#-4
    BR  EE_NEXT_EVENT
EE_INVALID_SLOT
    LEA  R6,EE_CLEAR_ARRAY_STACK
    LD   R2,EE_STACK_POINTER
    BRz  EE_SKIP_4      ; if stack pointer is 0 this stack is already empty and no need to clean it
    ADD  R6,R2,R6       ; R6 is now the stack pointer pointing to the top of the stack
    AND  R7,R7,#0
EE_LOOP5
    STR  R7,R6,#0       ; empty the stack
    ADD  R6,R6,#-1
    LDR  R4,R6,#0
    STR  R7,R4,#0       ; clear the location in the array 
    ADD  R2,R2,#-1      ; stack pointer decrement by 1
    BRp EE_LOOP5
    EE_SKIP_4
    ST   R2,EE_STACK_POINTER
    BR   SELECT_TIME_SLOT     ; we can just start another time search

NO_SPARE_TIME       ; The event is invalid, need to pop it out from the stack
    AND R4,R4,#0
    ADD R4,R4,#2    ; loop two times iterator
    EE_CLEAR
    ADD R3,R3,#-1   ; current day  
    LDR R0,R3,#0
    BRn EE_SKIP6    ; if R0 contains xffff, this means that no time taken for the event
    ADD R3,R3,#-1   ; current time
    ST  R0,TIME_INPUT
    AND R0,R0,#0    ;R0 is week day
    ADD R3,R3,#-1   ; R3 is now on week days
    LDR R2,R3,#0
    AND R5,R5,#0
    ADD R5,R5,#1
EE_LOOP7
    AND R7,R5,R2
    BRz    EE_SKIP3     ; if no day found, no need to calculate the location
    ST  R0,DAY_INPUT
    JSR FIND_DAY        ; find location
    LD  R6,FD_OUTPUT    ;location found and taken into R6
    AND R7,R7,#0
    STR R7,R6,#0        ; clear the location with x0000
    EE_SKIP3
    ADD R0,R0,#1
    ADD R5,R5,R5        ; bit mask left shift
    BRnp    EE_LOOP7

    ADD R3,R3,#-1  
    ADD R4,R4,#-1
    BRp EE_CLEAR        ; if this event is invalid, it means the time taken prior to this event has taken the wrong time
                        ; so we need to remove the previous event as well in the stack, loop twice
    ADD R1,R1,#-3
    ADD R3,R3,#4
    LD  R0,EE_STACK ; R0 is the location of the small stack
    NOT R0,R0
    ADD R0,R0,#1    ;R0 = -R0
    ADD R0,R3,R0    ;R0 = R3-R0
    BRz EE_ERROR    ; the stack is empty, which means that can not be fullfilled and no solution for the search

    ; stack initialization: copied from above
    AND R4,R4,#0
    LEA R2,EE_CLEAR_ARRAY_STACK
    STR R4,R2,#0
    ; ADD R4,R4,#1
    LEA R2,EE_STACK_POINTER
    STR R4,R2,#0
    BR SELECT_TIME_SLOT
    ;BR   EE_NEXT_EVENT ; jump to the location to take the next thing in the list
EE_SKIP6
    ADD R3,R3,#-3
    ADD R4,R4,#-1
    BR EE_CLEAR

EE_ERROR        ; pirnt error message
    LEA R0,EE_ERROR_MESSAGE
    PUTS
    LD  R1,JUDGE_PRINT      ; store in a location to tell the printing subroutine not to print
    ADD R1,R1,#1    
    ST  R1,JUDGE_PRINT

EE_PREP_RET                 ; restore registers
    LD  R0,  EE_REG_0
    LD  R1,  EE_REG_1
    LD  R2,  EE_REG_2 
    LD  R3,  EE_REG_3
    LD  R4,  EE_REG_4
    LD  R5,  EE_REG_5
    LD  R6,  EE_REG_6
    LD  R7,  EE_REG_7 
RET
EE_ARRAY  .FILL   x4000
EE_ORIG   .FILL   x6000
EE_STACK  .FILL   x8000
EE_SELECT_TIME      .BLKW   1
EE_STACK_POINTER    .FILL   x0000       ; small stack pointer initialise to 1
EE_CLEAR_ARRAY_STACK    .BLKW   6       ; the first address is the stack pointer
EE_REG_0  .BLKW   1
EE_REG_1  .BLKW   1
EE_REG_2  .BLKW   1
EE_REG_3  .BLKW   1
EE_REG_4  .BLKW   1
EE_REG_5  .BLKW   1
EE_REG_6  .BLKW   1
EE_REG_7  .BLKW   1
EE_ERROR_MESSAGE  .STRINGZ "Could not fit all events into schedule.\n"


; FIND_DAY-- Finds the current day address in the array
; INPUT: Day number(0~4) and time number(0~15) 
; OUTPUT: Address of the event in the array starting at x4000
; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - Starting address of the array, x4000
;   R1 - time input 
;   R2 - temporary
;   R3 - result address

FIND_DAY
;   register storage
    ST  R0,  FD_REG_0
    ST  R1,  FD_REG_1
    ST  R2,  FD_REG_2 
    ST  R3,  FD_REG_3
    ST  R7,  FD_REG_7

    LD  R1,TIME_INPUT   ;(0~15)
    LD  R2,DAY_INPUT    ;(0~4)
    AND R3,R3,#0    ; result
DF_LOOP
    ADD R3,R3,#5        ; a week has 5 days
    ADD R1,R1,#-1       ; time decrement
    BRzp  DF_LOOP
    ADD R3,R3,#-5
    ADD R3,R3,R2
    LD  R0,FD_ARRAY     ; load the initial position of the array 
    ADD R3,R3,R0
    ST  R3,FD_OUTPUT
; Restore registers
    LD  R0,  FD_REG_0
    LD  R1,  FD_REG_1
    LD  R2,  FD_REG_2 
    LD  R3,  FD_REG_3
    LD  R7,  FD_REG_7
RET
TIME_INPUT  .BLKW   1
DAY_INPUT   .BLKW   1
FD_OUTPUT   .BLKW   1
FD_REG_0    .BLKW   1
FD_REG_1    .BLKW   1
FD_REG_2    .BLKW   1
FD_REG_3    .BLKW   1
FD_REG_7    .BLKW   1
FD_ARRAY    .FILL   x4000

.END