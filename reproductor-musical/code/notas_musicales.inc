;###############################################################################
; Rutinas y definiciones para hacer sonar las notas musicales
;
; Se requieren incluir p16f877a.inc antes de incluir este archivo

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Valores por defecto de la cabecera
;_____________________
; Especifican que pines y que dirección se usará para los registros que se usan
; en los algoritmos. Estos valores se pueden sobreescribir mediante #define o
; EQU (según sea el caso) antes de incluir este archivo.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; El sonido sale por PORTB<0>
	IFNDEF NOTASMUSIC__SALIDA_SPEAKER
		#DEFINE NOTASMUSIC__SALIDA_SPEAKER PORTB, 0
	ENDIF

; Los contadores NOTASMUSIC__d1 y NOTASMUSIC__d2 se declaran a partir de 0x190. Si se define otra
; posición, hay que asegurarse de que haya dos espacios libres consecutivos
; a partir de la dirección elegida
	IFNDEF NOTASMUSIC__DIR_VARS
NOTASMUSIC__DIR_VARS EQU 0x190
	ENDIF

	CBLOCK NOTASMUSIC__DIR_VARS
		NOTASMUSIC__d1
		NOTASMUSIC__d2
		NOTASMUSIC__nota
	ENDC

NOTASMUSIC__ir_banco_auxiliares MACRO
	IF (NOTASMUSIC__DIR_VARS & 0x100)
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

	IF (NOTASMUSIC__DIR_VARS & 0x080)
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
NOTASMUSIC__sonarW
	bcf STATUS, RP1
	bcf STATUS, RP0 ; Banco 0

	; Primero se manda un valor alto
	bsf NOTASMUSIC__SALIDA_SPEAKER

	; El retardo se elige mediante un salto indexado.
	; Limitamos los valores de W para el caso en que se
	; reciba algo mayor a 7
	NOTASMUSIC__ir_banco_auxiliares
	andlw B'111'
	movwf (NOTASMUSIC__nota & 0x7F) ; Guardamos el valor de W
	; Multiplicamos el valor de W por 2, ya que cada
	; opción del salto indexado está compuesta por dos
	; instrucciones. Guardamos el resultado para en F para
	; usarlo luego, aunque también lo requerimos en W
	addwf (NOTASMUSIC__nota & 0x7F), F
	movf (NOTASMUSIC__nota & 0x7F), W

	addwf PCL, F
	call NOTASMUSIC__retardo_do4
	goto NOTASMUSIC__segunda_mitad
	call NOTASMUSIC__retardo_re4
	goto NOTASMUSIC__segunda_mitad
	call NOTASMUSIC__retardo_mi4
	goto NOTASMUSIC__segunda_mitad
	call NOTASMUSIC__retardo_fa4
	goto NOTASMUSIC__segunda_mitad
	call NOTASMUSIC__retardo_sol4
	goto NOTASMUSIC__segunda_mitad
	call NOTASMUSIC__retardo_la4
	goto NOTASMUSIC__segunda_mitad
	call NOTASMUSIC__retardo_si4
	goto NOTASMUSIC__segunda_mitad
	nop
	nop

NOTASMUSIC__segunda_mitad

	; Ahora se manda el valor bajo
    bcf STATUS, RP1
    bcf STATUS, RP0
	bcf NOTASMUSIC__SALIDA_SPEAKER

	; Segundo retardo
	NOTASMUSIC__ir_banco_auxiliares
	movf (NOTASMUSIC__nota & 0x7F), W

	addwf PCL, F
	call NOTASMUSIC__retardo_do4
	goto NOTASMUSIC__salir
	call NOTASMUSIC__retardo_re4
	goto NOTASMUSIC__salir
	call NOTASMUSIC__retardo_mi4
	goto NOTASMUSIC__salir
	call NOTASMUSIC__retardo_fa4
	goto NOTASMUSIC__salir
	call NOTASMUSIC__retardo_sol4
	goto NOTASMUSIC__salir
	call NOTASMUSIC__retardo_la4
	goto NOTASMUSIC__salir
	call NOTASMUSIC__retardo_si4
	nop
	nop

NOTASMUSIC__salir
    bcf STATUS, RP1
    bcf STATUS, RP0
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
NOTASMUSIC__retardo_do4
	NOTASMUSIC__ir_banco_auxiliares
			;9548 cycles
	movlw	0x75
	movwf	(NOTASMUSIC__d1 & 0x7F)
	movlw	0x08
	movwf	(NOTASMUSIC__d2 & 0x7F)
NOTASMUSIC__retardo_do4_0
	decfsz	(NOTASMUSIC__d1 & 0x7F), f
	goto	$+2
	decfsz	(NOTASMUSIC__d2 & 0x7F), f
	goto	NOTASMUSIC__retardo_do4_0

			;4 cycles
	goto	$+1
	goto	$+1

			;4 cycles (including call)
	return

;---------------------------------------
NOTASMUSIC__retardo_re4
			;8508 cycles
	movlw	0xA5
	movwf	(NOTASMUSIC__d1 & 0x7F)
	movlw	0x07
	movwf	(NOTASMUSIC__d2 & 0x7F)
NOTASMUSIC__retardo_re4_0
	decfsz	(NOTASMUSIC__d1 & 0x7F), f
	goto	$+2
	decfsz	(NOTASMUSIC__d2 & 0x7F), f
	goto	NOTASMUSIC__retardo_re4_0

			;1 cycle
	nop

			;4 cycles (including call)
	return

;---------------------------------------
NOTASMUSIC__retardo_mi4
			;7578 cycles
	movlw	0xEB
	movwf	(NOTASMUSIC__d1 & 0x7F)
	movlw	0x06
	movwf	(NOTASMUSIC__d2 & 0x7F)
NOTASMUSIC__retardo_mi4_0
	decfsz	(NOTASMUSIC__d1 & 0x7F), f
	goto	$+2
	decfsz	(NOTASMUSIC__d2 & 0x7F), f
	goto	NOTASMUSIC__retardo_mi4_0

			;2 cycles
	goto	$+1

			;4 cycles (including call)
	return


;---------------------------------------
NOTASMUSIC__retardo_fa4
			;7153 cycles
	movlw	0x96
	movwf	(NOTASMUSIC__d1 & 0x7F)
	movlw	0x06
	movwf	(NOTASMUSIC__d2 & 0x7F)
NOTASMUSIC__retardo_fa4_0
	decfsz	(NOTASMUSIC__d1 & 0x7F), f
	goto	$+2
	decfsz	(NOTASMUSIC__d2 & 0x7F), f
	goto	NOTASMUSIC__retardo_fa4_0

			;2 cycles
	goto	$+1

			;4 cycles (including call)
	return

;---------------------------------------
NOTASMUSIC__retardo_sol4
			;6373 cycles
	movlw	0xFA
	movwf	(NOTASMUSIC__d1 & 0x7F)
	movlw	0x05
	movwf	(NOTASMUSIC__d2 & 0x7F)
NOTASMUSIC__retardo_sol4_0
	decfsz	(NOTASMUSIC__d1 & 0x7F), f
	goto	$+2
	decfsz	(NOTASMUSIC__d2 & 0x7F), f
	goto	NOTASMUSIC__retardo_sol4_0

			;1 cycle
	nop

			;4 cycles (including call)
	return

;---------------------------------------
NOTASMUSIC__retardo_la4
			;5678 cycles
	movlw	0x6F
	movwf	(NOTASMUSIC__d1 & 0x7F)
	movlw	0x05
	movwf	(NOTASMUSIC__d2 & 0x7F)
NOTASMUSIC__retardo_la4_0
	decfsz	(NOTASMUSIC__d1 & 0x7F), f
	goto	$+2
	decfsz	(NOTASMUSIC__d2 & 0x7F), f
	goto	NOTASMUSIC__retardo_la4_0

			;4 cycles (including call)
	return

;---------------------------------------
NOTASMUSIC__retardo_si4
			;5058 cycles
	movlw	0xF3
	movwf	(NOTASMUSIC__d1 & 0x7F)
	movlw	0x04
	movwf	(NOTASMUSIC__d2 & 0x7F)
NOTASMUSIC__retardo_si4_0
	decfsz	(NOTASMUSIC__d1 & 0x7F), f
	goto	$+2
	decfsz	(NOTASMUSIC__d2 & 0x7F), f
	goto	NOTASMUSIC__retardo_si4_0

			;4 cycles (including call)
	return
