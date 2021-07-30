;Boot stub for loading BASIC programs from short ATR files.
;(c) 1998-1999 Jindrich Kubec <kubecj@asw.cz>

SIOV		= $E459

DOSVEC		= $0A
RAMTOP		= $6A
LOMEM		= $80
BOOT		= $09
BASICF		= $03F8
PORTB		= $D301

EOPEN		= $EF94

BAS_CONT	= $A97A
BAS_EXEC	= $B755

LOADER_START	= $0700
LOADER_END	= $0800

BASMEMSTART	= $0700

BASPTRSLEN	= $0E
BASPTRSSTART	= LOADER_END - BASPTRSLEN

		*= LOADER_START
		
BOOTST:	        .byt 0
		.byt (ENDBOOT-BOOTST+127)/128
		.WORD *-2
		.WORD MAIN_BEG

		CLC
RTS_INSTR:	RTS

		;used for extractor
ORI_LEN:	.WORD 0

MAIN_BEG:
		LDX	#<header
		LDY	#>header
		JSR	$C642

		LDX	#<padding
		LDY	#>padding
		JSR	$C642

		;copy pointers to correct area, add $700
		LDX	#BASPTRSLEN - 1
SETVECT:	CLC
		LDA	BASPTRSSTART,X
		ADC	#>BASMEMSTART
		STA	LOMEM,X
		DEX
		LDA	BASPTRSSTART,X
		STA	LOMEM,X
		DEX
		BPL	SETVECT

		LDA	#$31	;D
		STA	$300
		LDA	#$01	;1
		STA	$301
		LDA	#$52	;read
		STA	$302
		LDA	#$80	;$80 bytes
		STA	$308
		LDA	#$00
		STA	$309
		STA	$30B

		LDA	$82	;buffer address
		STA	$304
		LDA	$83
		STA	$305

		;starting sector
		LDA	# ( (ENDBOOT-BOOTST+127)/128 ) +1
		STA	$30A
SIO_CONT:
		LDA	#$40	;read
		STA	$303

		JSR	SIOV
		LDA	$303
		CMP	#$01
		BEQ	NOERR
		JSR	$C63E
AGAIN:		JMP	AGAIN	;dynamic halt

		;move buffer pointer
NOERR:		

		;LDA	$304
		;CLC
		;ADC	$308
		;STA	$304
		;LDA	$305
		;ADC	$309
		;STA	$305

		;this does the same as the above, but is shorter
		CLC
		LDX	#$FE
LP0:		LDA	$304 - $FE, X
		ADC	$308 - $FE, X
		STA	$304 - $FE, X
		INX
		BNE	LP0

		;increment sector number
		INC	$30A
		BNE	NX1
		INC	$30B
NX1:
		;decrement number of sectors to load
		LDA	SECTORS
		BNE	NX2
		DEC	SECTORS+1
		BMI	BAS_RUN
NX2:		DEC	SECTORS
		JMP	SIO_CONT

BAS_RUN:	LDX	#$FF
		TXS

		LDA	#$0A
		STA	$C9
		LDA	#$00
		STA	BOOT
		STA	BASICF

		;turn on basic
		LDA	#$FD
		STA	PORTB

		;set stack
		LDA	#>BAS_CONT
		PHA
		LDA	#<BAS_CONT - 1
		PHA

	    	LDA	#>BAS_EXEC
		PHA
		LDA	#<BAS_EXEC - 1
		PHA

		LDA	RAMTOP
		BPL	STORE_MEMT
		LDA	#$A0
STORE_MEMT:	STA	RAMTOP

		JMP	EOPEN

header:		.asc "BAS2BOOT (c)1999 Jindroush"
		.byte $1D
		.byte $9B
padding:	.dsb BASPTRSSTART - padding - 2 - 1, " "
		.byte $9B
SECTORS:	.WORD 0
BASPTRSSTART2:	.dsb BASPTRSLEN

ENDBOOT:

		.END

