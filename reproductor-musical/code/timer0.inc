;###############################################################################
; Rutinas para configurar el timer 0 para que genere una interrupción de forma
; periódica.
; También contiene código para guardar el estado y recuperarlo tras una
; interrupción, aunque no define como tratar dicha interrupción.
;
; Se requieren incluir p16f877a.inc antes de incluir este archivo

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Valores por defecto de la cabecera
;_____________________
; Especifican que pines y que dirección se usará para los registros que se usan
; en los algoritmos. Estos valores se pueden sobreescribir mediante #define o
; EQU (según sea el caso) antes de incluir este archivo, siempre y cuando sea
; una dirección compartida en todos los bancos (0x70 a 0x7F) dado que estos
; valores deben ser usados para guardar el estado en una interrupción.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


	IFNDEF TIMER0__DIR_VARS
TIMER0__DIR_VARS EQU 0x70
	ENDIF

	CBLOCK TIMER0__DIR_VARS
		TIMER0__W_TEMP
		TIMER0__STATUS_TEMP
	ENDC

	; Si no se define el preescalador antes de incluir este archivo, por defecto
	; PS<2:0> valen 111 (preescala 256)
	IFNDEF TIMER0__PREESCALADOR
TIMER0__PREESCALADOR EQU B'111'
	ENDIF

	; Si no se define T0CS, por defecto vale 0 (reloj interno)
	IFNDEF TIMER0_TOCS
TIMER0_TOCS EQU B'0'
	ENDIF

	; Si no se define el valor para poner en TMR0, por defecto vale 0
	IFNDEF TIMER0__TMR
TIMER0__TMR EQU B'0'
	ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sub Rutinas
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;---------------------------------------
; Habilita la interrupción por timer 0 y establece los valores que controlan
; el tiempo que tardará en lanzar su primer interrupción
TIMER0__configurar:
	bcf STATUS, RP1
	bsf STATUS, RP0 ; Banco 1

	; Configuración del temporizador
	; 5   T0CS  - Requiere un 0 para usar el relog interno
	; 3   PSA   - 0 para asignar el preescalador al timer0
	; 2:0 PS2:0 - 7 Valor del preescalado 256
	movlw (TIMER0__PREESCALADOR | (TIMER0_TOCS << 5))
	movwf (OPTION_REG & 0x7F)

	; Configurar las interrupciones. INTCON está en todos los bancos
	bsf INTCON, TMR0IE ; Interrupción del timer 0
	;bsf INTCON, GIE    ; Habilitador global de interrupción

	; Establecemos el valor del timer0/contador
	bcf STATUS, RP0 ; Banco 0
	movlw TIMER0__TMR
	movwf TMR0
	return

;---------------------------------------
; Rutina para usarse dentro de la rutina de atención a interrupción del
; timer 0. Guarda el estado del programa para que se recupere luego
TIMER0__pre_interrupt:
	movwf TIMER0__W_TEMP			; Guarda W
	swapf STATUS, W					; Para copiar, pero sin alterar banderas
	clrf STATUS						; Banco 0
	movwf TIMER0__STATUS_TEMP	; Guarda STATUS (swapped)
	return

;---------------------------------------
; Rutina para usarse dentro de la rutina de atención a interrupción del
; timer 0. Devuelve el valor configurado a TMR0 para que las próximas
; interrupciones pasen tras el mismo tiempo y también recupera el estado
; del programa que guardamos en el preámbulo.
TIMER0__post_interrupt:
	bcf STATUS, RP1
	bcf STATUS, RP0 ; Banco 0

	movlw TIMER0__TMR
	movwf TMR0

	swapf TIMER0__STATUS_TEMP, W
	movwf STATUS

	swapf TIMER0__W_TEMP, F
	swapf TIMER0__W_TEMP, W
	return
