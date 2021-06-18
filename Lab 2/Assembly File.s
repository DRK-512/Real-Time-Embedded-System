
	AREA subroutines, CODE
	EXPORT findHighBitASM

findHighBitASM
	; Initialize, check if you really need to initalize
	PUSH {r4-r6, lr} ; store link register
	MOV r3,r0 ; input array
	MOV r4,r1 ; nelements
	MOVS r0,#0x00 ; 32bit number we are looking at (using UXTH we will make this 32bit no worries)
	MOVS r1,#0x00 ; which bit we are looking at
	MOVS r2,#0x00 ; temp array value
	MOVS r5,#0x00 ; which number we are looking at
	; NOTE: r6 will act as the other half of the 32bit for r0
	; UXTH will help us do that

loop
	LDR r2,[r3,r1,LSL #2] ; create temp array value
	CBZ r2,endloop        ; psedo code for if temp != 0 then branch to end of loop
	MOVS r5,#0x00		; restore number we are looking at
	B nestedloop2 ; we want to skip the increment if we are at the last element of the array
	
nestedloop1
	ADDS r6,r5,#1 ; increment the number we are looking at
	UXTH r5,r6		; make sure to store it as 32bit not 16bits
	LSRS r2,r2,#1   ; right shift temp 
	
nestedloop2
	CMP r2,#0x01	; increment pos2
	BHI nestedloop1 	; if the for loop is not done, go back
	ADD r6,r5,r1,LSL #5	; pos1 = j*32 + pos2;
	UXTH r0,r6 		; r6=other half of r0 to make 32bit number

endloop
	ADDS r6,r1,#1 	; increment r6 
	UXTH r1,r6	; r6 is the other half of r1
	CMP r1,r4	; compare i of for loop to nelements
	BCC loop ; branch back to beginning
	
	; return position
	POP {r4-r6,pc}
	END
				
	
