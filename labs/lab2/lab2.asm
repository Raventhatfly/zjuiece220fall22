	; like most LC-3 programs, this one starts at address x3000
	.ORIG x3000

	; First, decide how you want to use registers to keep track
	; of information in your program.  Don't forget that JSR changes
	; R7, and that you must call TOKENIZE to obtain tokens from 
	; the user (in R0).

	; R0 -- token read from user
	; R1 -- 
	; R2 -- 
	; R3 -- 
	; R4 -- 
	; R5 -- 
	; R6 -- stack pointer (base is x8000)
NEXTLINE
	LD R6,STKBASE	; initialize the stack
	; Write your program here.  

LOOP
	JSR TOKENIZE
	ADD		R0,R0,#4
	BRz		RETURN
	ADD		R0,R0,#-4
	BRz		JUDGE			; this line is over
	BRn		N
	BRp		P
	RETURN
	BR LOOP

N	; a right thing
	LD		R5,STKBASE
	NOT		R5,R5
	ADD 	R5,R5,#1
	ADD		R5,R6,R5 ; determine if at the beginning of stack
	BRz		PRINT_NOOPEN	; at the beginning location of the stack, meaning that it is the leading one
	LDR		R4,R6,#-1
	ADD		R4,R4,R0
	BRnp	PRINT_MISMATCH	; not at the begining then there is mismatch
	ADD		R6,R6,#-1		; it is a match, eliminate the top element
	BR RETURN


P	; a left thing
	STR		R0,R6,#0		; store the current element in the stack
	ADD		R6,R6,#1		; point to the next location
	BR RETURN

JUDGE						
	LD 		R5,STKBASE
	NOT		R5,R5
	ADD 	R5,R5,#1
	ADD		R6,R6,R5
	BRz 	PRINT_VALID			; if at origin of the stack, valid
	BR  	PRINT_INVALID		; the stack is empty, it is not valid

PRINT_VALID
	LEA		R0,VALID
	PUTS
	BR END
PRINT_MISMATCH
	LEA 	R0,MISMAT
	PUTS	
BR END
PRINT_INVALID
	LEA 	R0,INVALID
	PUTS	
BR END
PRINT_NOOPEN
	LEA 	R0,NOOPEN
	PUTS	
BR END
END
	 BR NEXTLINE
	; Your program should actually never halt, but this
	; instruction is here in case your code allows the LC-3
	; to continue executing by accident.

	HALT

	; Add data here if you need to do so.  Message strings have been
	; provided already.

STKBASE	.FILL x8000
VALID	.STRINGZ "That line was properly balanced.\n"
INVALID .STRINGZ "That line was NOT properly balanced.\n"
MISMAT	.STRINGZ "\nMismatched closing symbol.\n"
NOOPEN	.STRINGZ "\nClosing symbol typed before opening symbol.\n"

; ----- DO NOT CHANGE THE CODE BELOW THIS LINE -----


; TOKENIZE -- read a key from the keyboard and return a token from -4 to 3
;             based on the key pressed
; INPUT: none
; OUTPUT: R0, a token from -4 to 3
;          -4 ... all keys except those below
;          -3 ... }
;          -2 ... ]
;          -1 ... )
;           0 ... CR/LF (<Enter>)
;           1 ... (
;           2 ... [
;           3 ... {
; OTHER REGISTERS: callee-saved (except for R7)
; SIDE EFFECTS: waits for user to press a key, echos the keystroke to the
;               display, and consumes the keystroke

; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - the key pressed / output value
;   R1 - loop counter for copying bits
;   R2 - bits [7:2] of key pressed (for use in lookup table indexing)
;   R3 - bit to copy from
;   R4 - temporary
;   R5 - bit to copy to

TOKENIZE
	ST R1,TOK_R1	; save callee-saved registers
	ST R2,TOK_R2	
	ST R3,TOK_R3	
	ST R4,TOK_R4
	ST R5,TOK_R5
	ST R7,TOK_R7	; need to call UNPACK

	GETC		; read the key into R0
	OUT		; echo the key to display

	; the token lookup table is packed, so we need to shift the key
	; value to the right by two bits to look up the token
	AND R2,R2,#0	; initialize R2 to 0
	ADD R1,R2,#6	; initialize loop counter to six (copy six bits)
	ADD R3,R2,#4	; first bit to copy from is 4
	ADD R5,R2,#1	; first bit to copy to is 1
TOKLOOP	AND R4,R0,R3	; check for non-zero bit
	BRz TOKZERO	; 0 bits are already in R2
	ADD R2,R2,R5	; add bit as a 1 to R2
TOKZERO	ADD R3,R3,R3	; move to next bit
	ADD R5,R5,R5
	ADD R1,R1,#-1	; decrement loop counter
	BRp TOKLOOP

	LEA R1,TOKTAB	; point R1 to lookup table
	ADD R1,R1,R2	; add six-bit index (bits [7:2] of key pressed)
	LDR R1,R1,#0	; read packed token value into R1

	; R0 still holds the key pressed
	JSR UNPACK	; unpack token into R0

	LD R1,TOK_R1	; restore callee-saved registers
	LD R2,TOK_R2	
	LD R3,TOK_R3	
	LD R4,TOK_R4
	LD R5,TOK_R5
	LD R7,TOK_R7	; restore R7 (changed by call to UNPACK)

	RET		; all done!

TOK_R1	.BLKW #1	; storage for callee-saved registers
TOK_R2	.BLKW #1
TOK_R3	.BLKW #1
TOK_R4	.BLKW #1
TOK_R5	.BLKW #1
TOK_R7	.BLKW #1


; UNPACK -- unpack a 4-bit 2's complement value from a 16-bit word
; INPUT: R0 - specifies which 4-bit value (0 = 4 MSB, 1 = next, ... 3 = 4 LSB)
;             (only bottom 2 bits of R0 are used)
;        R1 - the 16-bit word (four 4-bit 2's comp values: AAAABBBBCCCCDDDD)
; OUTPUT: R0 - the unpacked result (as 16-bit 2's complement)
; OTHER REGISTERS: callee-saved (except for R7)
; SIDE EFFECTS: none

; REGISTER USE (registers have multiple uses in different parts of code):
;   R0 - word selector / output value
;   R2 - temporary / bit value
;   R3 - bit being tested
;   R4 - pointer to sign bit lookup table
;   R5 - bit to copy to in loop

UNPACK	ST R2,UNP_R2	; save callee-saved registers
	ST R3,UNP_R3	
	ST R4,UNP_R4
	ST R5,UNP_R5

	; nothing changes R7, so we do not need to save/restore it

	AND R0,R0,#3	; discard upper bits of R0 input value

	LEA R4,SIGNBIT	; get pointer to sign bit in lookup table (index R0)
	ADD R4,R4,R0
	LDR R2,R4,#0	; copy sign bit into R2

	AND R0,R0,#0	; initialize R0 to 0 for output
	AND R2,R2,R1	; check for negative value
	BRz POSSIGN	; for positive values, just copy low three bits
	LD R0,NEGHIGH	; for negative values, set 13 MSB to 1 (sign extend)
POSSIGN

	; low bit is next sign bit x 2 (except when R3=3)
	LDR R2,R4,#1	; look up next sign bit in table (index R3 + 1)
	ADD R2,R2,R2	; double it
	BRnp HAVELOW	; low bit is ready unless R3=3 (in which case Z is set)
	ADD R2,R2,#1	; last word should be 1, not 0 (can't double it)
HAVELOW

	AND R4,R4,#0	; reuse R4 as loop counter
	ADD R5,R4,#1	; first bit to copy to is 1
	ADD R4,R4,#3	; execute loop three times
UNPLOOP	AND R3,R2,R1	; check next bit in R1
	BRz BITCLR	; if bit is 0, already correct in R0
	ADD R0,R0,R5	; add bit to R0
BITCLR	ADD R2,R2,R2	; move to next bit
	ADD R5,R5,R5
	ADD R4,R4,#-1	; decrement loop counter
	BRp UNPLOOP	; repeat for all three bits
	; R0 is now fully unpacked!

	LD R2,UNP_R2	; restore callee-saved registers
	LD R3,UNP_R3	
	LD R4,UNP_R4
	LD R5,UNP_R5

	RET		; all done!

UNP_R2	.BLKW #1	; storage for callee-saved registers
UNP_R3	.BLKW #1
UNP_R4	.BLKW #1
UNP_R5	.BLKW #1
SIGNBIT	.FILL x8000	; sign bit / low bit lookup table
	.FILL x0800
	.FILL x0080
	.FILL x0008
	.FILL x0000
NEGHIGH	.FILL xFFF8	; for sign extension from 4-bit to 16-bit


; This is the lookup table for token values from 8-bit extended ASCII.
; Since we only need 3 bits for a token, we pack four token values
; (as 4-bit 2's complement) into each memory location.

TOKTAB	.FILL xCCCC	; (x00 - x0F)
	.FILL xCCCC
	.FILL xCC0C	; x0A -> 0
	.FILL xC0CC	; x0D -> 0

	.FILL xCCCC	; (x10 - x1F)
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC

	.FILL xCCCC	; (x20 - x2F)
	.FILL xCCCC
	.FILL x1FCC	; x28 -> 1, x29 -> -1
	.FILL xCCCC

	.FILL xCCCC	; (x30 - x3F)
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC

	.FILL xCCCC	; (x40 - x4F)
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC

	.FILL xCCCC	; (x50 - x5F)
	.FILL xCCCC
	.FILL xCCC2	; x5B -> 2
	.FILL xCECC	; x5D -> -2

	.FILL xCCCC	; (x60 - x6F)
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC

	.FILL xCCCC	; (x70 - x7F)
	.FILL xCCCC
	.FILL xCCC3	; x7B -> 3
	.FILL xCDCC	; x7D -> -3

	.FILL xCCCC	; (x80 - xFF)
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC
	.FILL xCCCC

	.END
