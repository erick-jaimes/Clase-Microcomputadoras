;###############################################################################
; Rutinas para transmitir y recibir bytes de forma síncrona mediante
; comunicación serial
;
; Se requieren incluir p16f877a.inc antes de incluir este archivo

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sub Rutinas
;_____________________
; Permiten configurar la comunicación y transmitir bits individuales
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;---------------------------------------
; Configurar la comunicación serial a apriximadamente 9600 bauds y a 8 bits a
; travéz de los pines RX y TX
configurar_serial:
	bcf STATUS, RP1
	bsf STATUS, RP0 ; Banco 1
	
	; Se selecciona baja velocidad
	bcf (TXSTA & 0x7F), BRGH

	; Configurar el baudage a 9600 (aprox).
	; El valor exacto de SPBRG es (20Mhz) / (64 * baudRate) -1 ~= 31.5
	; Esté calculo se hace acorde con la selección anterior de alta/baja velocidad
  	movlw .31
	movwf (SPBRG & 0x7F)

	; Configurar comunicación asíncrona
	bcf (TXSTA & 0x7F), SYNC

	; Configurarlo para 8 bits en transmisión (valor por defecto)
	bcf (TXSTA & 0x7F), TX9

	; Habilitar la transmisión
	bsf (TXSTA & 0x7F), TXEN
	
	; Ponemos RX/RC7 como entrada y TX/RC6 como salida
	bcf (TRISC & 0x7F), 6
	bsf (TRISC & 0x7F), 7
	
	bcf STATUS, RP0 ; Banco 0

	; Encender el módulo
	bsf (RCSTA & 0x7F), SPEN

	; Configurarlo para 8 bits en transmisión (valor por defecto)
	bcf (RCSTA & 0x7F), RX9

	; Habilitar la recepción
	bsf (RCSTA & 0x7F), CREN
	return

;---------------------------------------
; Transmite el byte de información que se encuentre en W
transmitir_byte_serial:
	bcf STATUS, RP0
	bcf STATUS, RP1 ; Banco 0
	
	movwf (TXREG & 0x7F)

	bsf STATUS, RP0 ; Banco 1

espera_tx:
	btfss (TXSTA & 0x7F), TRMT
	goto espera_tx

	bcf STATUS, RP0 ; Banco 0
	return

;---------------------------------------
; Recibe un byte de información y lo guarda en W
recibir_byte_serial:
	bcf STATUS, RP0
	bcf STATUS, RP1 ; Banco 0

	; Empezamos haciendo un poleo hasta que la
	; recepción anterior termine
espera_rx:
	btfss (PIR1 & 0x7F), RCIF
	goto espera_rx
	
	movf (RCREG & 0x7F), W
	return

;---------------------------------------
; Transmite los bytes \r\n
salto_linea:
	movlw 0xD
	call transmitir_byte_serial
	movlw 0xA
	call transmitir_byte_serial
	return

;---------------------------------------
; Transmite un espacio
espacio:
	movlw 0x20
	call transmitir_byte_serial
	return