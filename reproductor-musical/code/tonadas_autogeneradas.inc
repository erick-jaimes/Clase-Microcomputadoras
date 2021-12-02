;###############################################################################
; Archivo con tablas que representan tonadas.
; La parte inicial de este archivo, la cual contiene definiciones, es fija. Por
; su parte, las tablas son autogeneradas mediante un script de python.
;
; Antes de este archivo, se deben incluir la cabecera del procesador a usar y
; también la cabecera de transmisión serial

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Valores por defecto de la cabecera
;_____________________
; Se indica la dirección inicial en la que se ubicarán los registros (variables)
; que se usan en estas sub-rutinas.
; Estos valores se pueden sobreescribir mediante #define o EQU
; (según sea el caso) antes de incluir este archivo.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	IFNDEF TONADA__DIR_VARS
TONADA__DIR_VARS EQU 0x50
	ENDIF

	CBLOCK TONADA__DIR_VARS
		TONADA__zero
		TONADA__temp
		TONADA__indice
	ENDC

TONADA__ir_banco_auxiliares MACRO
	IF (TONADA__DIR_VARS & 0x100)
		IFDEF DEPURAR_POR_SEPARADO
			MESSG "RP1 = 1"
		ENDIF
		bsf STATUS, RP1
	ELSE
		IFDEF DEPURAR_POR_SEPARADO
			MESSG "RP1 = 0"
		ENDIF
		bcf STATUS, RP1
	ENDIF

	IF (TONADA__DIR_VARS & 0x080)
		IFDEF DEPURAR_POR_SEPARADO
			MESSG "RP0 = 1"
		ENDIF
		bsf STATUS, RP0
	ELSE
		IFDEF DEPURAR_POR_SEPARADO
			MESSG "RP0 = 0"
		ENDIF
		bcf STATUS, RP0
	ENDIF
  ENDM

TONADA__configurar:
	TONADA__ir_banco_auxiliares

	clrf (TONADA__zero & 0x3F)
	return


martinillo:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw LOW(tonada0__table)
    movwf (TONADA__temp & 0x7F)
    rlf (TONADA__zero & 0x7F), W
    addlw HIGH(tonada0__table)
    movwf PCLATH
    movf (TONADA__temp & 0x7F), W
    movwf PCL
tonada0__table
    DT 0XA, "martinillo", 0X40, 0X2, 0X2B, 0X22, 0X2B, 0X42, 0X2B, 0X2, 0X2B
    DT 0X2, 0X2B, 0X22, 0X2B, 0X42, 0X2B, 0X2, 0X2B, 0X42, 0X2B, 0X62, 0X2B
    DT 0X84, 0X57, 0X42, 0X2B, 0X62, 0X2B, 0X84, 0X57, 0X81, 0X15, 0XA1, 0X15
    DT 0X81, 0X15, 0X61, 0X15, 0X42, 0X2B, 0X2, 0X2B, 0X81, 0X15, 0XA1, 0X15
    DT 0X81, 0X15, 0X61, 0X15, 0X42, 0X2B, 0X2, 0X2B, 0X22, 0X2B, 0X82, 0X2B
    DT 0X4, 0X57, 0X22, 0X2B, 0X82, 0X2B, 0X4, 0X57, 0x0


short_mario_bros:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw LOW(tonada1__table)
    movwf (TONADA__temp & 0x7F)
    rlf (TONADA__zero & 0x7F), W
    addlw HIGH(tonada1__table)
    movwf PCLATH
    movf (TONADA__temp & 0x7F), W
    movwf PCL
tonada1__table
    DT 0X10, "short_mario_bros", 0X3C, 0X41, 0X15, 0X41, 0X15, 0XE1, 0X15, 0X41
    DT 0X15, 0XE1, 0X15, 0X1, 0X15, 0X41, 0X15, 0X82, 0X2B, 0XE2, 0X2B, 0X81
    DT 0X15, 0XE2, 0X2B, 0X3, 0X41, 0X81, 0X15, 0XE2, 0X2B, 0X43, 0X41, 0XA2
    DT 0X2B, 0XC2, 0X2B, 0XA1, 0X15, 0XA2, 0X2B, 0X81, 0XA0, 0X41, 0XA0, 0X81
    DT 0XA0, 0XA2, 0X2B, 0X61, 0X15, 0X81, 0X15, 0XE1, 0X15, 0X42, 0X2B, 0X1
    DT 0X15, 0X21, 0X15, 0XC3, 0X41, 0x0


short_merry_christmas:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw LOW(tonada2__table)
    movwf (TONADA__temp & 0x7F)
    rlf (TONADA__zero & 0x7F), W
    addlw HIGH(tonada2__table)
    movwf PCLATH
    movf (TONADA__temp & 0x7F), W
    movwf PCL
tonada2__table
    DT 0X15, "short_merry_christmas", 0X3A, 0X2, 0X2B, 0X62, 0X2B, 0X61, 0X15
    DT 0X81, 0X15, 0X61, 0X15, 0X41, 0X15, 0X22, 0X2B, 0X22, 0X2B, 0X22, 0X2B
    DT 0X82, 0X2B, 0X81, 0X15, 0XA1, 0X15, 0X81, 0X15, 0X61, 0X15, 0X42, 0X2B
    DT 0X2, 0X2B, 0X2, 0X2B, 0XA2, 0X2B, 0XA1, 0X15, 0XA1, 0X15, 0XA1, 0X15
    DT 0X81, 0X15, 0X62, 0X2B, 0X22, 0X2B, 0X1, 0X15, 0X1, 0X15, 0X22, 0X2B
    DT 0X82, 0X2B, 0X42, 0X2B, 0x0

