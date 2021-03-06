;###############################################################################
; Rutinas para transmitir y recibir bytes de forma s�ncrona mediante
; comunicaci�n serial
;
; Se requieren incluir p16f877a.inc antes de incluir este archivo

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sub Rutinas
;_____________________
; Permiten configurar la comunicaci�n y transmitir bits individuales
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;---------------------------------------
; Configurar la comunicaci�n serial a apriximadamente 9600 bauds y a 8 bits a
; trav�z de los pines RX y TX
SERIAL__configurar:
	bcf STATUS, RP1
	bsf STATUS, RP0 ; Banco 1

	; Se selecciona baja velocidad
	bcf (TXSTA & 0x7F), BRGH

	; Configurar el baudage a 9600 (aprox).
	; El valor exacto de SPBRG es (20Mhz) / (64 * baudRate) -1 ~= 31.5
	; Est� calculo se hace acorde con la selecci�n anterior de alta/baja velocidad
  	movlw .31
	movwf (SPBRG & 0x7F)

	; Configurar comunicaci�n as�ncrona
	bcf (TXSTA & 0x7F), SYNC

	; Configurarlo para 8 bits en transmisi�n (valor por defecto)
	bcf (TXSTA & 0x7F), TX9

	; Habilitar la transmisi�n
	bsf (TXSTA & 0x7F), TXEN

	; Ponemos RX/RC7 como entrada y TX/RC6 como salida
	bcf (TRISC & 0x7F), 6
	bsf (TRISC & 0x7F), 7

	bcf STATUS, RP0 ; Banco 0

	; Encender el m�dulo
	bsf (RCSTA & 0x7F), SPEN

	; Configurarlo para 8 bits en transmisi�n (valor por defecto)
	bcf (RCSTA & 0x7F), RX9

	; Habilitar la recepci�n
	bsf (RCSTA & 0x7F), CREN
	return

;---------------------------------------
; Transmite el byte de informaci�n que se encuentre en W
SERIAL__transmitir_byte:
	bcf STATUS, RP0
	bcf STATUS, RP1 ; Banco 0

	movwf (TXREG & 0x7F)

	bsf STATUS, RP0 ; Banco 1

SERIAL__espera_tx:
	btfss (TXSTA & 0x7F), TRMT
	goto SERIAL__espera_tx

	bcf STATUS, RP0 ; Banco 0
	return

;---------------------------------------
; Recibe un byte de informaci�n y lo guarda en W
SERIAL__recibir_byte:
	bcf STATUS, RP0
	bcf STATUS, RP1 ; Banco 0

	; Empezamos haciendo un poleo hasta que la
	; recepci�n anterior termine
SERIAL__espera_rx:
	btfss (PIR1 & 0x7F), RCIF
	goto SERIAL__espera_rx

	movf (RCREG & 0x7F), W
	return

;---------------------------------------
; Transmite los bytes \r\n
SERIAL__salto_linea:
	movlw 0xD
	call SERIAL__transmitir_byte
	movlw 0xA
	call SERIAL__transmitir_byte
	return

;---------------------------------------
; Transmite un espacio
SERIAL__espacio:
	movlw 0x20
	call SERIAL__transmitir_byte
	return

;---------------------------------------
; Transmite \f
SERIAL__nueva_pagina:
	movlw 0xC
	call SERIAL__transmitir_byte
	return

;---------------------------------------
; Transmite \b
SERIAL__retroceso:
	movlw 0x8
	call SERIAL__transmitir_byte
	return