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

	; Se configuran los pines necesarios como salida
	bcf LCD_PinRS
	bcf LCD_PinEnable
	bcf LCD_PinRW
	clrf LCD_BUSDatos

	bcf STATUS, RP0 		; Banco 0
	
	bcf LCD_PinRW   		; En caso de que esté conectado le indica
							; que se va a escribir en el LCD
	bcf LCD_PinEnable 		; Impide funcionamiento del LCD poniendo E=0
	bcf LCD_PinRS	 		; Activa el modo Comando poniendo RS=0

	call Retardo_2ms
	movlw 0x38				; 8 bits
	call LCD_EscribeLCD
	movlw 0x01				; Clear screen
	call LCD_EscribeLCD
	movlw 0x06				; Curosr incremental
	call LCD_EscribeLCD
	movlw 0x0E				; Display and cursor on
	call LCD_EscribeLCD
	return

;---------------------------------------------------------
; Subrutina que envía W al bus dedatos del LCD
LCD_EscribeLCD
	bcf STATUS, RP1
	bcf STATUS, RP0 ; Banco 0

	movwf LCD_BUSDatos			; Recupera el dato a enviar
	bsf LCD_PinEnable			; Permite funcionamiento del LCD mediante un pequeño
	bcf LCD_PinEnable			; pulso y termina impidiendo el funcionamiento del LCD
	call Retardo_50micros
	return

;---------------------------------------------------------
; Subrutinas variadas para el control del módulo LCD
LCD_Linea1						; Cursor al principio de la Línea 1
	movlw b'10000000'			; Dirección 00h de la DDRAM
	goto LCD_EnviaComando
LCD_Linea2
	movlw b'11000000'			; Dirección 40h de la DDRAM
	goto LCD_EnviaComando

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