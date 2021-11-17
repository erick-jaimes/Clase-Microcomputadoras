; Prueba de notas_musicales.inc
; Mandamos al pin de salida por defecto, PORTB<0>, la señal
; que reproduce la tonada durante 256 tiempos
  processor pic16f877a
  include <p16f877a.inc>

  org 0
    goto inicio
  org 5

  #DEFINE DEPURAR_POR_SEPARADO
  include "../code/notas_musicales.inc"

contador EQU 0x20
nota     EQU 0x21

; Vamos a mandar la señal correspondiente
inicio:
	bsf STATUS, RP0 ; Banco 1

	clrf TRISB & 0x7F

	bcf STATUS, RP0 ; Banco 0
	clrf contador
	bsf PORTB, 0

ciclo:
	movf nota, W
	call NOTASMUSIC__sonarW
	decfsz contador, F
	goto ciclo
	incf nota, F ; Aumenta cada que contador se hace cero

    goto ciclo
  end