; Librería para el control de LCD, basada en LCD_4BIT.INC que
; aparece a partir de la página 196 del libro "Microcontrolador PIC16F87",
; pero adaptándola a 8 bits

	CBLOCK 0x20
	LCD_Dato
	LCD_GuardaDato
	LCD_GuardaTRISB
	LCD_Auxiliar1
	LCD_Auxiliar2
	LCD_Retardo1
	LCD_Retardo2
	LCD_Retardo3
	ENDC

LCD_CaracteresPorLinea EQU .16

#DEFINE LCD_PinRS		PORTC, 5
#DEFINE LCD_PinRW		PORTC, 6
#DEFINE LCD_PinEnable	PORTC, 7

#DEFINE LCD_BUSDatos	PORTD

;---------------------------------------------------------
; Subrutina que inicializa el LCD
LCD_Inicializa
	bcf STATUS, RP1
	bsf STATUS, RP0 		; Banco 1, donde se configuran los TRIS
	bcf LCD_PinRS
	bcf LCD_PinEnable
	bcf LCD_PinRW
	clrf LCD_BUSDatos

	bcf STATUS, RP0 		; Banco 0
	
	bcf LCD_PinRW   		; En caso de que esté conectado le indica
							; que se va a escribir en el LCD
	bcf LCD_PinEnable 		; Impide funcionamiento del LCD poniendo E=0
	bcf LCD_PinRS	 		; Activa el modo Comando poniendo RS=0
	call Retardo_20ms
	movlw b'00110000'
	call LCD_EscribeLCD		; Escribe el dato en el LCD
	call Retardo_5ms
	movlw b'00110000'
	call LCD_EscribeLCD
	call Retardo_200micros
	movlw b'00110000'
	call LCD_EscribeLCD
	
	movlw b'00110000' 		; Interface  de 8 bits
	call LCD_EscribeLCD

; Ahora configura el resto de los parámetros:
	call LCD_2Lineas8Bits5x7 	; LCD de 2 líneas y caracteres de 5x7 puntos
	call LCD_Borra				; Pantalla encendida y limpia. Cursor al principio
	call LCD_CursorOFF			; de la línea 1. Cursor apagado.
	call LCD_CursorIncr			; Cursor en modo incrementar.
	return

;---------------------------------------------------------
; Subrutina que envía W al bus dedatos del LCD
LCD_EscribeLCD
	bcf STATUS, RP1
	bcf STATUS, RP0 ; Banco 0

	movwf LCD_BUSDatos			; Recupera el dato a enviar
	bsf LCD_PinEnable			; Permite funcionamiento del LCD mediante un pequeño
	bcf LCD_PinEnable			; pulso y termina impidiendo el funcionamiento del LCD
	return

;---------------------------------------------------------
; Subrutinas variadas para el control del módulo LCD
LCD_CursorIncr					; Cursor en modo incrementar
	movlw b'00000110'
	goto LCD_EnviaComando
LCD_Linea1						; Cursor al principio de la Línea 1
	movlw b'10000000'			; Dirección 00h de la DDRAM
	goto LCD_EnviaComando
LCD_Linea2
	movlw b'11000000'			; Dirección 40h de la DDRAM
	goto LCD_EnviaComando
LCD_PosicionLinea1				; Cursor a posición de la Línea 1, a partir de la
	iorlw b'10000000'			; dirección 00h de la DDRAM más el valor del
	goto LCD_EnviaComando		; registro W
LCD_PosicionLinea2				; Cursor a posición de la Línea 2, a partir de la
	iorlw b'11000000'			; dirección 40h de la DDRAM más el valor del
	goto LCD_EnviaComando		; registro W
LCD_OFF							; Pantalla apagada
	movlw b'00001000'
	goto LCD_EnviaComando
LCD_CursorON					; Pantalla encendida y cursor encendido
	movlw b'00001110'
	goto LCD_EnviaComando
LCD_CursorOFF					; Pantalla encendida, memoria DDRAM y pone el
	movlw b'00001100'			; cursor a principio de la línea 1.
	goto LCD_EnviaComando
LCD_Borra						; Borra toda la pantalla,, memoria DDRAM y pone el
	movlw b'00000001'			; cursor al principio de la línea 1.
	goto LCD_EnviaComando
LCD_2Lineas8Bits5x7				; Define la pantalla de líneas, con caracteres
	movlw b'00101000'			; de 5x7 y conexión al PIC mediante bus de
	goto LCD_EnviaComando		; 8 bits

;---------------------------------------------------------
; LCD_EnviaComando: Subrutina que envía W al LCD cuando este se encuentra en
; modo comando
; LCD_Caracter: Subrutina que envía W al LCD cuando se trabaja en modo
; dato
LCD_EnviaComando
	bcf LCD_PinRS				; Activa el Modo Comando, poniendo RS=0
	goto LCD_Envia
LCD_Caracter					; Activa el "Modo Dato", poniendo RS=1
	bsf LCD_PinRS				
LCD_Envia
	movwf LCD_GuardaDato		; Guarda el dato enviar
	call LCD_EscribeLCD
	btfss LCD_PinRS				; Debe garantizar una correcta escritura manteniendo
	call Retardo_2ms			; 2 ms en modo comando y 50 us en modo caracter.
	call Retardo_50micros
	return

;---------------------------------------------------------
; Subrutinas de retardo usadas
Retardo_20ms
    movlw 0xdc
    movwf LCD_Retardo1
    movlw 0x82
    movwf LCD_Retardo2
Retardo_20ms_0
    decfsz LCD_Retardo1, 1
    goto Retardo_20ms_0
    decfsz LCD_Retardo2, 1
    goto Retardo_20ms_0
    nop
    return

Retardo_5ms
    movlw 0x75
    movwf LCD_Retardo1
    movlw 0x21
    movwf LCD_Retardo2
Retardo_5ms_0
    decfsz LCD_Retardo1, 1
    goto Retardo_5ms_0
    decfsz LCD_Retardo2, 1
    goto Retardo_5ms_0
    return

Retardo_200micros
    movlw 0x49
    movwf LCD_Retardo1
    movlw 0x02
    movwf LCD_Retardo2
Retardo_200us_0
    decfsz LCD_Retardo1, 1
    goto Retardo_200us_0
    decfsz LCD_Retardo2, 1
    goto Retardo_200us_0
    nop
    nop
    return

Retardo_2ms
    movlw 0xfa
    movwf LCD_Retardo1
    movlw 0x0d
    movwf LCD_Retardo2
Retardo_2ms_0
    decfsz LCD_Retardo1, 1
    goto Retardo_2ms_0
    decfsz LCD_Retardo2, 1
    goto Retardo_2ms_0
    nop
    return

Retardo_50micros
    movlw 0x51
    movwf LCD_Retardo1
Retardo_50micros_0
    decfsz LCD_Retardo1, 1
    goto Retardo_50micros_0
    nop
    nop
    return