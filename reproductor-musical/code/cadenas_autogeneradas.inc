;###############################################################################
; Archivo con tablas que representan cadenas. Sirven para enviarse a la
; terminal virtual.
; La parte inicial de este archivo, la cual contiene definiciones, es fija. Por
; su parte, las tablas son autogeneradas mediante un script de python.

cadenas__zero     EQU 0x7D
cadenas__temp     EQU 0x7E
cadenas__indice   EQU 0x7F

cadenas__configurar:
	bcf STATUS, RP0
	bcf STATUS, RP1 ; Banco 0

	clrf (cadenas__zero & 0x3F)
	return

;---------------------------------------
; Macro para transmitir una tabla de datos ubicada
; en la memoria programa
cadenas__label_num = 0
cadenas__transmitir MACRO tabla
	bcf STATUS, RP1
	bcf STATUS, RP0 ; Banco 0

	clrf (cadenas__indice & 0x3F)

ciclo_transmitir_tabla#v(cadenas__label_num)
	movf (cadenas__indice & 0x3F), W
    call tabla ; Cambia el valor de W
	addlw 0xFF ; W + 255
	addlw 0x01 ; W + 256 == W, permite revizar la bandera Z sin usar un registro
	btfsc STATUS, Z
	goto terminar_transmision_tabla#v(cadenas__label_num)

	call transmitir_byte_serial ; Transmite

	incf (cadenas__indice & 0x3F), F
	goto ciclo_transmitir_tabla#v(cadenas__label_num)

terminar_transmision_tabla#v(cadenas__label_num)

cadenas__label_num++

  ENDM


str_prueba_caracteres_especiales:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw str0__table
    movwf cadenas__temp
    rlf cadenas__zero, W
    addlw HIGH(str0__table)
    movwf PCLATH
    movf cadenas__temp, W
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
    movwf cadenas__temp
    rlf cadenas__zero, W
    addlw HIGH(str1__table)
    movwf PCLATH
    movf cadenas__temp, W
    movwf PCL
str1__table
    DT "Probando la transmisi", 0XA2, "n serial", 0XD
    DT "______________________________", 0XD, 0XD
    DT "Tabla de caracteres que usa la terminal actual:", 0XD, 0XD, 0

