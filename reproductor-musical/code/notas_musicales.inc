;###############################################################################
; Rutinas y definiciones para hacer sonar las notas musicales
;
; Se requieren incluir p16f877a.inc antes de incluir este archivo

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Valores por defecto de la cabecera
;_____________________
; Especifican que pines y que registros se usarán en caso de no especificarse
; otro valor antes de incluir este archivo
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; El sonido sale por PORTB<0>
	IFNDEF NOTAS_MUSICALES_SALIDA_SPEAKER
		#DEFINE NOTAS_MUSICALES_SALIDA_SPEAKER PORTB, 0
	ENDIF

; Los contadores notas_musicales_d1 y notas_musicales_d2 se declaran a partir de 0x189. Si se define otra
; posición, hay que asegurarse de que haya dos espacios libres consecutivos
; a partir de la dirección elegida
	IFNDEF NOTAS_MUSICALES_ADDR_REG
NOTAS_MUSICALES_ADDR_REG EQU 0x190
	ENDIF

	CBLOCK NOTAS_MUSICALES_ADDR_REG
		notas_musicales_d1
		notas_musicales_d2
		nota_musical_actual
	ENDC

ir_banco_auxiliares MACRO
	IF (NOTAS_MUSICALES_ADDR_REG & 0x100)
		bsf STATUS, RP1
	ELSE
		bcf STATUS, RP1
	ENDIF

	IF (NOTAS_MUSICALES_ADDR_REG & 0x080)
		bsf STATUS, RP0
	ELSE
		bcf STATUS, RP0
	ENDIF
  ENDM

ir_banco_0 MACRO
	bcf STATUS, RP1
	bcf STATUS, RP0
  ENDM

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sub Rutinas
;_____________________
; Permiten producir el sonido por un corto tiempo
; otro valor antes de incluir este archivo
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;---------------------------------------
; Dependiendo del valor de W previo a llamar esta sub-rutina, sonará una
; de las otras notas musicales.
;  W = 0  ====> Do
;  W = 1  ====> Re
;  W = 2  ====> Mi
;  W = 3  ====> Fa
;  W = 4  ====> Sol
;  W = 5  ====> La
;  W = 6  ====> Si
;  W = 7  ====> Ninguna nota
sonar_nota_musical
	; Primero se manda un valor alto
	ir_banco_0
	bsf NOTAS_MUSICALES_SALIDA_SPEAKER

	; El retardo se elige mediante un salto indexado.
	; Limitamos los valores de W para el caso en que se
	; reciba algo mayor a 7
	ir_banco_auxiliares
	andlw B'111'
	movwf (nota_musical_actual & 0x7F) ; Guardamos el valor de W
	; Multiplicamos el valor de W por 2, ya que cada
	; opción del salto indexado está compuesta por dos
	; instrucciones. Guardamos el resultado para en F para
	; usarlo luego, aunque también lo requerimos en W
	addwf (nota_musical_actual & 0x7F), F
	movf (nota_musical_actual & 0x7F), W

	addwf PCL, F
	call retardo_do4
	goto segunda_mitad_nota
	call retardo_re4
	goto segunda_mitad_nota
	call retardo_mi4
	goto segunda_mitad_nota
	call retardo_fa4
	goto segunda_mitad_nota
	call retardo_sol4
	goto segunda_mitad_nota
	call retardo_la4
	goto segunda_mitad_nota
	call retardo_si4
	goto segunda_mitad_nota
	nop
	nop

segunda_mitad_nota

	; Ahora se manda el valor bajo
	ir_banco_0
	bcf NOTAS_MUSICALES_SALIDA_SPEAKER

	; Segundo retardo
	ir_banco_auxiliares
	movf (nota_musical_actual & 0x7F), W

	addwf PCL, F
	call retardo_do4
	goto salir_rutina_notas
	call retardo_re4
	goto salir_rutina_notas
	call retardo_mi4
	goto salir_rutina_notas
	call retardo_fa4
	goto salir_rutina_notas
	call retardo_sol4
	goto salir_rutina_notas
	call retardo_la4
	goto salir_rutina_notas
	call retardo_si4
	nop
	nop

salir_rutina_notas
	ir_banco_0
	return

; Frecuencias necesarias para los sonidos de la octava central
; Las frecuencias de la octava central se obtuvieron de 
; https://microsonus.com/biblioteca-musical/microseries/fundamentos-del-sonido/fundamentos-notas-frecuencias/
;
; Su valor aproximado es:
;Do 261.626 .00382225 s
;Re 293.665 .003405241 s
;Mi 329.628 .003033723 s
;Fa 349.228 .002863459 s
;So 391.995 .002551053 s
;La 440.000 .002272727 s
;Si 493.883 .002024771 s

;---------------------------------------
; Retardo auxiliar, adecuado para hacer sonar la nota 'DO'
retardo_do4
	ir_banco_auxiliares
			;9548 cycles
	movlw	0x75
	movwf	(notas_musicales_d1 & 0x7F)
	movlw	0x08
	movwf	(notas_musicales_d2 & 0x7F)
retardo_do4_0
	decfsz	(notas_musicales_d1 & 0x7F), f
	goto	$+2
	decfsz	(notas_musicales_d2 & 0x7F), f
	goto	retardo_do4_0

			;4 cycles
	goto	$+1
	goto	$+1

			;4 cycles (including call)
	return

;---------------------------------------
retardo_re4
			;8508 cycles
	movlw	0xA5
	movwf	(notas_musicales_d1 & 0x7F)
	movlw	0x07
	movwf	(notas_musicales_d2 & 0x7F)
retardo_re4_0
	decfsz	(notas_musicales_d1 & 0x7F), f
	goto	$+2
	decfsz	(notas_musicales_d2 & 0x7F), f
	goto	retardo_re4_0

			;1 cycle
	nop

			;4 cycles (including call)
	return

;---------------------------------------
retardo_mi4
			;7578 cycles
	movlw	0xEB
	movwf	(notas_musicales_d1 & 0x7F)
	movlw	0x06
	movwf	(notas_musicales_d2 & 0x7F)
retardo_mi4_0
	decfsz	(notas_musicales_d1 & 0x7F), f
	goto	$+2
	decfsz	(notas_musicales_d2 & 0x7F), f
	goto	retardo_mi4_0

			;2 cycles
	goto	$+1

			;4 cycles (including call)
	return


;---------------------------------------
retardo_fa4
			;7153 cycles
	movlw	0x96
	movwf	(notas_musicales_d1 & 0x7F)
	movlw	0x06
	movwf	(notas_musicales_d2 & 0x7F)
retardo_fa4_0
	decfsz	(notas_musicales_d1 & 0x7F), f
	goto	$+2
	decfsz	(notas_musicales_d2 & 0x7F), f
	goto	retardo_fa4_0

			;2 cycles
	goto	$+1

			;4 cycles (including call)
	return

;---------------------------------------
retardo_sol4
			;6373 cycles
	movlw	0xFA
	movwf	(notas_musicales_d1 & 0x7F)
	movlw	0x05
	movwf	(notas_musicales_d2 & 0x7F)
retardo_sol4_0
	decfsz	(notas_musicales_d1 & 0x7F), f
	goto	$+2
	decfsz	(notas_musicales_d2 & 0x7F), f
	goto	retardo_sol4_0

			;1 cycle
	nop

			;4 cycles (including call)
	return

;---------------------------------------
retardo_la4
			;5678 cycles
	movlw	0x6F
	movwf	(notas_musicales_d1 & 0x7F)
	movlw	0x05
	movwf	(notas_musicales_d2 & 0x7F)
retardo_la4_0
	decfsz	(notas_musicales_d1 & 0x7F), f
	goto	$+2
	decfsz	(notas_musicales_d2 & 0x7F), f
	goto	retardo_la4_0

			;4 cycles (including call)
	return

;---------------------------------------
retardo_si4
			;5058 cycles
	movlw	0xF3
	movwf	(notas_musicales_d1 & 0x7F)
	movlw	0x04
	movwf	(notas_musicales_d2 & 0x7F)
retardo_si4_0
	decfsz	(notas_musicales_d1 & 0x7F), f
	goto	$+2
	decfsz	(notas_musicales_d2 & 0x7F), f
	goto	retardo_si4_0

			;4 cycles (including call)
	return
