;###############################################################################
; Rutinas para configurar el timer 0 para que genere una interrupci�n de forma
; peri�dica.
; Tambi�n contiene c�digo para guardar el estado y recuperarlo tras una
; interrupci�n, aunque no define como tratar dicha interrupci�n.
;
; Se requieren incluir p16f877a.inc antes de incluir este archivo

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Valores por defecto de la cabecera
;_____________________
; Especifican que pines y que direcci�n se usar� para los registros que se usan
; en los algoritmos. Estos valores se pueden sobreescribir mediante #define o
; EQU (seg�n sea el caso) antes de incluir este archivo, siempre y cuando sea
; una direcci�n compartida en todos los bancos (0x70 a 0x7F) dado que estos
; valores deben ser usados para guardar el estado en una interrupci�n.
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
; Habilita la interrupci�n por timer 0 y establece los valores que controlan
; el tiempo que tardar� en lanzar su primer interrupci�n
TIMER0__configurar:
	bcf STATUS, RP1
	bsf STATUS, RP0 ; Banco 1

	; Configuraci�n del temporizador
	; 5   T0CS  - Requiere un 0 para usar el relog interno
	; 3   PSA   - 0 para asignar el preescalador al timer0
	; 2:0 PS2:0 - 7 Valor del preescalado 256
	movlw (TIMER0__PREESCALADOR | (TIMER0_TOCS << 5))
	movwf (OPTION_REG & 0x7F)

	; Configurar las interrupciones. INTCON est� en todos los bancos
	bsf INTCON, TMR0IE ; Interrupci�n del timer 0
	;bsf INTCON, GIE    ; Habilitador global de interrupci�n

	; Establecemos el valor del timer0/contador
	bcf STATUS, RP0 ; Banco 0
	movlw TIMER0__TMR
	movwf TMR0
	return

;---------------------------------------
; Rutina para usarse dentro de la rutina de atenci�n a interrupci�n del
; timer 0. Guarda el estado del programa para que se recupere luego
TIMER0__pre_interrupt:
	movwf TIMER0__W_TEMP			; Guarda W
	swapf STATUS, W					; Para copiar, pero sin alterar banderas
	clrf STATUS						; Banco 0
	movwf TIMER0__STATUS_TEMP	; Guarda STATUS (swapped)
	return

;---------------------------------------
; Rutina para usarse dentro de la rutina de atenci�n a interrupci�n del
; timer 0. Devuelve el valor configurado a TMR0 para que las pr�ximas
; interrupciones pasen tras el mismo tiempo y tambi�n recupera el estado
; del programa que guardamos en el pre�mbulo.
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
