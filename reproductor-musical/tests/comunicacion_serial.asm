; Prueba de comunicacion_serial.inc
; Se prueba el funcionamiento de la transmisión serial
; haciendo un echo hacia una terminal.
  processor pic16f877a
  include <p16f877a.inc>

  org 0
    goto inicio
  org 5

contador EQU 0x20

  #DEFINE DEPURAR_POR_SEPARADO
  include "../code/comunicacion_serial.inc"
  include "../code/cadenas_autogeneradas.inc"

inicio:
	call configurar_serial
	call cadenas__configurar

	cadenas__transmitir(str_prueba_serial)

	movlw .32
	movwf contador
set_caracteres
	; Cada 16 caracteres mandamos un salto de línea
	movf contador, W
	call transmitir_byte_serial
	call espacio ; Sobreescribe W

	movf contador, W
	movlw 0xF
	andwf contador, W
	sublw 0xF
	btfsc STATUS, Z
	call salto_linea
	
	incfsz contador, F
	goto set_caracteres

	call salto_linea

	cadenas__transmitir(str_prueba_caracteres_especiales)

echo:
	call recibir_byte_serial
	call transmitir_byte_serial
	goto echo
	
  end