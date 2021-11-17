;###############################################################################
; Archivo con tablas que representan cadenas. Sirven para enviarse a la
; terminal virtual.
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

	IFNDEF CADENAS__DIR_VARS
CADENAS__DIR_VARS EQU 0x70
	ENDIF

	CBLOCK CADENAS__DIR_VARS
		CADENAS__zero
		CADENAS__temp
		CADENAS__indice
	ENDC

CADENAS__ir_banco_auxiliares MACRO
	IF (CADENAS__DIR_VARS & 0x100)
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

	IF (CADENAS__DIR_VARS & 0x080)
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

CADENAS__configurar:
	CADENAS__ir_banco_auxiliares

	clrf (CADENAS__zero & 0x3F)
	return

;---------------------------------------
; Macro para transmitir una tabla de datos ubicada
; en la memoria programa
CADENAS__label_num = 0
CADENAS__transmitir MACRO tabla
	CADENAS__ir_banco_auxiliares
	clrf (CADENAS__indice & 0x3F)

CADENAS__ciclo_transmitir#v(CADENAS__label_num)
	movf (CADENAS__indice & 0x3F), W

    call tabla ; Cambia el valor de W
	addlw 0xFF ; W + 255
	addlw 0x01 ; W + 256 == W, permite revisar la bandera Z sin usar un registro
	btfsc STATUS, Z
	goto CADENAS__terminar#v(CADENAS__label_num)

	call SERIAL__transmitir_byte ; Transmite

	CADENAS__ir_banco_auxiliares
	incf (CADENAS__indice & 0x3F), F
	goto CADENAS__ciclo_transmitir#v(CADENAS__label_num)

CADENAS__terminar#v(CADENAS__label_num)
	bcf STATUS, RP1
	bcf STATUS, RP0
CADENAS__label_num++

  ENDM


str_prueba_caracteres_especiales:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw str0__table
    movwf (CADENAS__temp & 0x3F)
    rlf (CADENAS__zero & 0x3F), W
    addlw HIGH(str0__table)
    movwf PCLATH
    movf (CADENAS__temp & 0x3F), W
    movwf PCL
str0__table
    DT 0XD, 0XC9, 0XCD, 0XCD, 0XCD, 0XCD, 0XCD, 0XCD, 0XCD, 0XCD, 0XCD, 0XCD
    DT 0XCD, 0XCD, 0XCD, 0XBB, 0XD, 0XBA, " ", 0XA0, " ", 0X82, " ", 0XA1, " "
    DT 0XA2, " ", 0XA3, " ", 0XA4, " ", 0XBA, 0XD, 0XCC, 0XCD, 0XCD, 0XCD, 0XCB
    DT 0XCD, 0XCD, 0XCD, 0XCB, 0XCD, 0XCD, 0XCD, 0XCB, 0XCB, 0XB9, 0XD, 0XBA
    DT " ", 0XE3, " ", 0XBA, " ", 0XE0, " ", 0XBA, " ", 0XEA, " ", 0XCC, 0XCE
    DT 0XB9, 0XD, 0XC8, 0XCD, 0XCD, 0XCD, 0XCA, 0XCD, 0XCD, 0XCD, 0XCA, 0XCD
    DT 0XCD, 0XCD, 0XCA, 0XCA, 0XBC, 0XD, 0XD, "algo@example.com", 0XD, 0


str_prueba_serial:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw str1__table
    movwf (CADENAS__temp & 0x3F)
    rlf (CADENAS__zero & 0x3F), W
    addlw HIGH(str1__table)
    movwf PCLATH
    movf (CADENAS__temp & 0x3F), W
    movwf PCL
str1__table
    DT "Probando la transmisi", 0XA2, "n serial", 0XD
    DT "______________________________", 0XD, 0XD
    DT "Tabla de caracteres que usa la terminal actual:", 0XD, 0XD, 0

