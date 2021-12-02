;###############################################################################
; Escribir y leer bytes de la EEPROM integrada
;
; Se requieren incluir p16f877a.inc antes de incluir este archivo
; Si se cambia de pic, hay que ajustar el valor de EEPROM_SIZE para que
; se corresponda con el tamaño de EEPROM de dicho dispositivo, el cual debe ser
; menor o igual a 256 bytes

EEPROM__SIZE EQU .256

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Valores por defecto de la cabecera
;_____________________
; Especifican que pines y que dirección se usará para los registros que se usan
; en los algoritmos. Estos valores se pueden sobreescribir mediante #define o
; EQU (según sea el caso) antes de incluir este archivo.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Las variables EEPROM_dir y EEPROM__lena tienen direcciones a partir del valor
; especificado mediante un EQU, o a partir de 0x130 si dicha
; instrucción no se usa antes de este punto
	IFNDEF EEPROM__DIR_VARS
EEPROM__DIR_VARS EQU 0x130
	ENDIF

	CBLOCK EEPROM__DIR_VARS
		EEPROM__dir			  ; Dirección para realizar la lectura/escritura actual
		EEPROM__data          ; Dato a operar en la lectura/escritura

		EEPROM__top			  ; Apuntador para escribir en la rutina EEPROM_write_byte_top
		EEPROM__llena		  ; Se usa un solo bit, para indicar si se llenó la EEPROM
		EEPROM__indice_tabla  ; Se usa al transmitir cadenas guardadas en tablas

		EEPROM__SAVE_INTCON	  ; Para guardar INTCON y reponerlo luego
		EEPROM__SAVE_PCLATH
	ENDC

EEPROM__ir_banco_auxiliares MACRO
	IF (EEPROM__DIR_VARS & 0x100)
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

	IF (EEPROM__DIR_VARS & 0x080)
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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sub Rutinas
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;---------------------------------------
; Inicializa a 0 los registros a usar
EEPROM__configurar:
	EEPROM__ir_banco_auxiliares
	clrf (EEPROM__top & 0x7F)
	clrf (EEPROM__llena & 0x7F)
	return

;---------------------------------------
; Escribe un byte guardado en W a la dirección de memoria
; guardada en EEPROM_dir
EEPROM__write_byte:
	; Esperando en el banco 3 a que podamos escribir
	bsf STATUS, RP0
	bsf STATUS, RP1 ; B3
EEPROM__wait_prev_write:
	btfsc (EECON1 & 0x7F), WR
	goto EEPROM__wait_prev_write

	; Colocando la dirección y el dato a escribir en
	; los registros correspondientes
	bsf STATUS, RP1
	bcf STATUS, RP0; B2

	movwf (EEDATA & 0x7F)

	EEPROM__ir_banco_auxiliares
	movf (EEPROM__dir & 0x7F), W

	bsf STATUS, RP1
	bcf STATUS, RP0; B2

	movwf (EEADR & 0x7F)

	; Apuntar a memoria dato y habilitar escritura
	bsf STATUS, RP0 ; B3
	bcf (EECON1 & 0x7F), EEPGD ; Apunta a memoria dato
	bsf (EECON1 & 0x7F), WREN  ; Habilita escritura

	; Deshabilita interrupciones temporalmente
	swapf INTCON, W
	
	EEPROM__ir_banco_auxiliares
	movwf EEPROM__SAVE_INTCON
	

	bsf STATUS, RP1
	bsf STATUS, RP0 ; B3

	bcf INTCON, GIE

	; Escribiendo dos valores (aparentemente mágicos) a EECON2
	; que hacen que esto funcione
	movlw 0x55
	movwf (EECON2 & 0x7F)
	movlw 0xAA
	movwf (EECON2 & 0x7F)

	; Comenzar operación de escritura
	bsf (EECON1 & 0x7F), WR

	; Volver a habilitar interrupciones si es que lo estaban previamente
	EEPROM__ir_banco_auxiliares
	swapf EEPROM__SAVE_INTCON, W

	bsf STATUS, RP1
	bsf STATUS, RP0 ; B3

	movwf INTCON

	; Deshabilitar escritura
	bcf (EECON1 & 0x7F), WREN

	; Volver al banco 0
	bcf STATUS, RP1
	bcf STATUS, RP0
	return

;---------------------------------------
; Escribe un byte guardado en W a la dirección de memoria apuntada por
; EEPROM_top y luego aumenta el valor del mismo con el objetivo de
; escribir sobre bytes consecutivos.
; En caso de que solo se haga uso de esta sub-rutina, el valor de EEPROM_top
; como un indicador del número de bytes guardados, mientras que EEPROM_llena
; servirá para comprobar si la EEPROM está llena.
EEPROM__write_byte_top:
	EEPROM__ir_banco_auxiliares
	movwf (EEPROM__data & 0x7F)

	btfsc (EEPROM__llena & 0x7F), 0
	goto EEPROM__terminar

	movf (EEPROM__top & 0x7F), W
	movwf (EEPROM__dir & 0x7F)
	movf (EEPROM__data & 0x7F), W
	call EEPROM__write_byte

	EEPROM__ir_banco_auxiliares

	incf (EEPROM__top & 0x7F), F
	btfsc STATUS, Z   ; Si se desborda el valor (vuelve a cero), se llenó la memoria
	bsf (EEPROM__llena & 0x7F), 0

EEPROM__terminar:
	bcf STATUS, RP1
	bcf STATUS, RP0
	return

;---------------------------------------
; Lee un byte desde la dirección EEPROM_dir y lo guarda en W
EEPROM__read_byte:
	; Si hay alguna lectura pendiente, hay riesgo de que afecte
	; el registro de la EEPROM que queremos leer. Mejor esperar
	; a que terminen todas las escrituras
	BSF STATUS, RP0
	BSF STATUS, RP1 ; B3
EEPROM__wait_read:
	btfsc (EECON1 & 0x7F), WR
	goto EEPROM__wait_read

	; Guardar la dirección de lectura en EEADR
	EEPROM__ir_banco_auxiliares

	movf (EEPROM__dir & 0x7F), W

	bsf STATUS, RP1
	bcf STATUS, RP0 ; Banco 2

	movwf (EEADR & 0x7F)    ; Dirección de donde leemos

	bsf STATUS, RP0 ; Banco 3

	bcf (EECON1 & 0x7F), EEPGD ; Apunta a la memoria dato
	bsf (EECON1 & 0x7F), RD    ; Inicia operación de lectura

	bcf STATUS, RP0 ; Banco 2

	movf (EEDATA & 0x7F), W

	bcf STATUS, RP1 ; Banco 0
	return

;---------------------------------------
; Macro para transmitir una tabla de datos ubicada
; en la memoria programa hacia direcciones sucesivas
; de la EEPROM
EEPROM__label_num = 0
EEPROM__guardar_cadena MACRO tabla
	EEPROM__ir_banco_auxiliares
	movf PCLATH, W
	movwf (EEPROM__SAVE_PCLATH & 0x7F)
	clrf (EEPROM__indice_tabla & 0x7F)

EEPROM__ciclo_transmitir#v(EEPROM__label_num)
	movf (EEPROM__indice_tabla & 0x7F), W

    call tabla ; Cambia el valor de W
	addlw 0xFF ; W + 255
	addlw 0x01 ; W + 256 == W, permite revisar la bandera Z sin usar un registro
	btfsc STATUS, Z
	goto EEPROM__terminar#v(EEPROM__label_num)

	call EEPROM__write_byte_top ; Guarda el byte leído de la tabla

	EEPROM__ir_banco_auxiliares
	incf (EEPROM__indice_tabla & 0x7F), F
	goto EEPROM__ciclo_transmitir#v(EEPROM__label_num)

EEPROM__terminar#v(EEPROM__label_num)
	bcf STATUS, RP1
	bcf STATUS, RP0

	movf (EEPROM__SAVE_PCLATH & 0x7F), W
	movwf PCLATH
EEPROM__label_num++

  ENDM
