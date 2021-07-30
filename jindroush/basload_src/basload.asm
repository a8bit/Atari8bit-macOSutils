;BASLOAD 1.00
;(c)1998 Jindrich Kubec <kubecj@asw.cz>
;homepage: http://www.asw.cz/~kubecj

;Based on SIDLOAD utility by Sidney Cadot

;loads basic programs in same way as SIDLOAD does with EXEs

DOSVEC		= $0A
MATCH		= $FB
FIRST		= $FC
LAST		= $FE

COLOR2		= $2C6
RUNAD		= $2E0
INITAD		= $2E2
CRSINH		= $2F0

CIOV		= $E456
COLDSV		= $E477

;;; **********************
;;; ** BOOT DISK HEADER **
;;; **********************

		*= $480
		
BOOT:	        .BYTE 0
		.BYTE (ENDBOOT-BOOT+127)/128
		.WORD *-2
		.WORD DOSVEC_SET

;;; *************************
;;; ** BOOT INITIALISATION **
;;; *************************
	
		CLC
RTS_INSTR:	RTS

;;; ********************
;;; ** USER INTERFACE **
;;; ********************
	
USER_INTF:
#ifndef SIDSHORT
		LDA COLOR2	;  save COLOR2
		PHA
		LDA #0
		STA COLOR2	; black screen

	;; print title

		LDX #0
		LDA #11
		STA $342,X
		LDA #<TITLE
		STA $344,X
		LDA #>TITLE
		STA $345,X
		LDA #<TITLE_LEN
		STA $348,X
		LDA #>TITLE_LEN
		STA $349,X
		JSR CIOV
		
	;; print "filename" prompt
	
REDO:		LDX #0
		LDA #11
		STA $342,X
		LDA #<PROMPT
		STA $344,X
		LDA #>PROMPT
		STA $345,X
		LDA #<PROMPT_LEN
		STA $348,X
		LDA #>PROMPT_LEN
		STA $349,X
		JSR CIOV

	;; read filename from keyboard

		LDX #0
		LDA #5
		STA $342,X
		LDA #<FNAME
		STA $344,X
		LDA #>FNAME
		STA $345,X
		LDA #$FF
		STA $348,X
		STA $349,X
		JSR CIOV
#endif

	;; open file for input (IOCB #1)

		LDX #$10
		LDA #3
		STA $342,X
		LDA #<DEVICE
		STA $344,X
		LDA #>DEVICE
		STA $345,X
		LDA #4
		STA $34A,X
		LDA #0
		STA $34B,X
		JSR CIOV
;		BMI TRY_DIR
		BPL LOADER

		JMP TRY_DIR

;;; *******************
;;; ** GO FOR LOADER **
;;; *******************

LOADER:		LDA #$07	;this is READ
		STA $342,X

#ifndef SIDSHORT
		PLA		; restore screen color
		STA COLOR2
		LDA #1		;  Turn cursor off
		STA CRSINH
#endif
		
#ifndef SIDSHORT
	;; print "Loading..."
	
		LDX #0
		LDA #11
		STA $342,X
		LDA #<LOADING
		STA $344,X
		LDA #>LOADING
		STA $345,X
		LDA #<LOADING_LEN
		STA $348,X
		LDA #>LOADING_LEN
		STA $349,X
		JSR CIOV

		LDA #0		; Turn cursor back on
		STA CRSINH
#endif

		LDX #$10	;sets channel 1

		LDA #$80	;from address 0x80
		STA $344,X
		LDA #$0E  	;7 * 2 = 0xE
		STA $348,X
		LDA #0
		STA $345,X
		STA $349,X
		JSR CIOV
		BMI TRY_DIR	;Loads 7 main pointers

		LDX	#$0D
SETVECT:	CLC
		LDA	$80,X
		ADC	#$07	;base address is 0x0700
		STA	$80,X
		DEX
		DEX
		BPL	SETVECT

		LDX #$10

		LDA $82
		STA $344,X
		LDA $83
		STA $345,X
		CLC
		LDA $8C
		SBC $82
		STA $348,X
		PHP
		LDA $8D
		PLP
		SBC $83
		STA $349,X
		JSR CIOV
		BMI TRY_DIR	;Loads main block

		LDX #$10	;  CLOSE #1
		LDA #12
		STA $342,X
		JSR CIOV
		JMP RUNBAS

;;; ************************
;;; ** TRY DIRECTORY LIST **
;;; ************************

TRY_DIR:	LDX #$10	;  CLOSE #1
		LDA #12
		STA $342,X
		JSR CIOV

#ifndef SIDSHORT
		LDX #$00	;  Print EOL
		LDA #11
		STA $342,X
		LDA #0
		STA $348,X
		STA $349,X
		LDA #$9B
		JSR CIOV

		LDX #$10	;  OPEN #1,6,0,DEVICE
		LDA #3
		STA $342,X
		LDA #<DEVICE
		STA $344,X
		LDA #>DEVICE
		STA $345,X
		LDA #6
		STA $34A,X
		LDA #0
		STA $34B,X
		JSR CIOV

		LDA #0		;  Initially, no matches detected
		STA MATCH

DIR_NEXT:	LDX #$10	; Get directory entry
		LDA #5
		STA $342,X
		LDA #<DIR_ENTRY
		STA $344,X
		LDA #>DIR_ENTRY
		STA $345,X
		LDA #$FF
		STA $348,X
		STA $349,X
		JSR CIOV
		BMI CHK_MATCH	; Bail out on error

		LDA #1		; We have a match
		STA MATCH
	
		LDX #$00	; Print space
		LDA #11
		STA $342,X
		LDA #0
		STA $348,X
		STA $349,X
		LDA #" "
		JSR CIOV
	
		LDX #$00	;  Print match
		LDA #9
		STA $342,X
		LDA #<DIR_ENTRY
		STA $344,X
		LDA #>DIR_ENTRY
		STA $345,X
		LDA #$FF
		STA $348,X
		STA $349,X
		JSR CIOV

		JMP DIR_NEXT	;  repeat for next dir entry

CHK_MATCH:	LDA MATCH	; skip print if match...
		BNE CLOSE_DIR

#endif
		LDX #0		; print "No match" message.
		LDA #11
		STA $342,X
		LDA #<NOMATCH
		STA $344,X
		LDA #>NOMATCH
		STA $345,X
		LDA #<NOMATCH_LEN
		STA $348,X
		LDA #>NOMATCH_LEN
		STA $349,X
		JSR CIOV		

#ifndef SIDSHORT
CLOSE_DIR:	LDX #$10	;  CLOSE #1 for DIR
		LDA #12
		STA $342,X
		JSR CIOV

		LDX #$00	;  Print EOL
		LDA #11
		STA $342,X
		LDA #0
		STA $348,X
		STA $349,X
		LDA #$9B
		JSR CIOV

		JMP REDO	;  Start over with 'file name' prompt.
#else
MYSELF:		JMP MYSELF
#endif

RUNBAS:		
		LDX	#$FF
		TXS
		LDA	#$A9
		PHA
		LDA	#$79
		PHA
		LDA	#$0A
		STA	$C9
		LDA	#$00
		STA	$09
		STA	$03F8
		LDA	#$FD
		STA	$D301	;turns BASIC on
		LDA	#$B7
		PHA
		LDA	#$54
		PHA
		LDA	$6A
		BPL	STORE_MEMT
		LDA	#$A0
STORE_MEMT:	STA	$6A
		JMP	$EF94

;;; ****************
;;; ** DOSVEC-SET **
;;; ****************

DOSVEC_SET:	LDA #<USER_INTF
		STA DOSVEC
		LDA #>USER_INTF
		STA DOSVEC+1
		RTS
	
	;; FILE NAME & STUFF

#ifndef SIDSHORT
TITLE:		.BYTE " "+$80
		.BYTE "B"+$80,"A"+$80,"S"+$80,"l"+$80,"o"+$80,"a"+$80,"d"+$80
		.BYTE " "+$80
		.BYTE "1"+$80,"."+$80,"0"+$80,"0"+$80
		.BYTE " "+$80
		.BYTE $9B,$9B
TITLE_LEN = * - TITLE
#endif

#ifndef SIDSHORT
PROMPT:		.BYTE "Filename: "
PROMPT_LEN	= * - PROMPT
#endif

#ifndef SIDSHORT
NOMATCH:	.BYTE " No match.",$9B
#else
NOMATCH:	.BYTE "LOAD ERROR",$9B
#endif
NOMATCH_LEN   = * - NOMATCH

#ifndef SIDSHORT
LOADING:	.BYTE $7D,$9B,"Loading...",$9B
LOADING_LEN = *-LOADING
#endif

#ifndef SIDSHORT
DEVICE:		.BYTE "H1:"
FNAME:
#else
DEVICE:		.BYTE "H1:PRG.BAS"
#endif


ENDBOOT:
DIR_ENTRY =* + 16

		.END

