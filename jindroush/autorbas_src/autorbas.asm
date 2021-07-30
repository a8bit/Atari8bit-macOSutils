;(c)1998 Jindrich Kubec <kubecj@asw.cz>
;homepage: http://www.asw.cz/~kubecj

;this is source for autorun.sys, which should start BASIC programs.
;any commands could be inserted in name.asm, althought RUN "D:name.bas" 
;should be enough

STARTBLK = $600

HATABS = $031A

		*= STARTBLK - 6

.WORD		$FFFF
.WORD		STARTBLK
.WORD		ENDBLK - 1

RUNAD:		LDX	#$00

		;find HATABS entry for editor E:
LOOPHTBS:	LDA	HATABS,X
		CMP	#"E"
		BEQ	ENDSEARCH
		INX
		INX
		INX
		BNE	LOOPHTBS

		;store HATABS pointer
ENDSEARCH:	INX
		STX	OLDHATB

		;revector hatabs
		LDA	HATABS,X
		STA	$CD
		LDA	#<NEWENTRY
		STA	HATABS,X

		INX

		LDA	HATABS,X
		STA	$CE
		LDA	#>NEWENTRY
		STA	HATABS,X

		;copy original entry
		LDY	#$00
		LDX	#$10
CPYLOOP:	LDA	($CD),Y
		STA	NEWENTRY,Y
		INY
		DEX
		BNE	CPYLOOP

		;and patch it
		LDA	#<READCOMMANDBYTE - 1
		STA	NEW_EGETCH
		LDA	#>READCOMMANDBYTE
		STA	NEW_EGETCH + 1
		RTS

		;read one byte from string
READCOMMANDBYTE:LDY	COMMANDPTR
		LDA	BASICCOMMAND,Y
		BEQ	RESTORE_HATABS
		INC	COMMANDPTR
		LDY	#$01
		RTS

RESTORE_HATABS:	TXA
		PHA

		;restore HATABS
		LDX	OLDHATB

		LDA	$CD
		STA	HATABS,X

		INX

		LDA	$CE
		STA	HATABS,X

		PLA
		TAX

		;and return ENTER as last character
		LDA	#$9B
		LDY	#$01
		RTS

		;old HATABS pointer
OLDHATB:	.BYTE $00

		;current ptr in BASIC command
COMMANDPTR:	.BYTE $00

		;new device entry for E:
NEWENTRY:
NEW_EOPEN:	.WORD $0000
NEW_ECLOSE:	.WORD $0000
NEW_EGETCH:	.WORD $0000
NEW_EOUTCH:	.WORD $0000
NEW_RETUR1:	.WORD $0000
		.WORD $0000
NEW_JMP:	.BYTE $00, $00, $00
NEW_PAD:	.BYTE $00


		;basic command string in include file
BASICCOMMAND:	
#include name.asm
		.BYTE $00

ENDBLK:

.WORD		$02E0
.WORD		$02E1
.WORD		RUNAD

