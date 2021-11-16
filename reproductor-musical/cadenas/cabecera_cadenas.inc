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
