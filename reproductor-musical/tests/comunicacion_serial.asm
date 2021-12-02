; Prueba de comunicacion_serial.inc
; Se prueba el funcionamiento de la transmisión serial
; haciendo un echo hacia una terminal.
  processor pic16f877a
  include <p16f877a.inc>

  org 0
    goto inicio
  org 5

	CBLOCK 0x20
		contador
		d1
		d2
		d3
	ENDC

  #DEFINE DEPURAR_POR_SEPARADO
  include "../code/comunicacion_serial.inc"
CADENAS__DIR_VARS EQU 0x120
  include "../code/cadenas_autogeneradas.inc"

inicio:
	call SERIAL__configurar
	call CADENAS__configurar

	CADENAS__transmitir str_prueba_serial

	movlw .32
	movwf contador

set_caracteres:
	; Cada 16 caracteres mandamos un salto de línea
	movf contador, W
	call SERIAL__transmitir_byte
	call SERIAL__espacio ; Sobreescribe W

	movf contador, W
	movlw 0xF
	andwf contador, W
	sublw 0xF
	btfsc STATUS, Z
	call SERIAL__salto_linea

	incfsz contador, F
	goto set_caracteres

	call SERIAL__salto_linea

	movlw .30
	movwf contador
cargando:
	movlw '#'
	call SERIAL__transmitir_byte
	call retardo

	decfsz contador, F
	goto cargando

	movlw .30
	movwf contador
descargando:
	call SERIAL__retroceso
	call retardo

	decfsz contador, F
	goto descargando

	CADENAS__transmitir str_prueba_caracteres_especiales

	call SERIAL__recibir_byte
	call SERIAL__nueva_pagina

echo:
	call SERIAL__recibir_byte
	call SERIAL__transmitir_byte
	goto echo

retardo:

	 movlw	0x8A
	 movwf	d1
	 movlw	0xBA
	 movwf	d2
	 movlw	0x01
	 movwf	d3
retardo_0
	 decfsz	d1, f
	 goto	$+2
	 decfsz	d2, f
	 goto	$+2
	 decfsz	d3, f
	 goto	retardo_0

			;5 cycles
	 goto	$+1
	 goto	$+1
	 nop

	 return

  end
