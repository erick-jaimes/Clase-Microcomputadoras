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


	IFNDEF LIBTIMER0__DIR_VARS
LIBTIMER0__DIR_VARS EQU 0x70
	ENDIF

	CBLOCK LIBTIMER0__DIR_VARS
		LIBTIMER0__W_TEMP
		LIBTIMER0__STATUS_TEMP
		LIBTIMER0__TMR0
	ENDC

	; Si no se define el preescalador antes de incluir este archivo, por defecto
	; PS<2:0> valen 111 (preescala 256)
	IFNDEF LIBTIMER0__PREESCALADOR
LIBTIMER0__PREESCALADOR B'111'
	ENDIF

	; Si no se define T0CS, por defecto vale 0 (reloj interno)
	IFNDEF LIBTIMER0_TOCS
LIBTIMER0_TOCS EQU B'0'
	ENDIF

	; Si no se define el valor para poner en TMR0, por defecto vale 0
	IFNDEF LIBTIMER0_TMR
LIBTIMER0_TMR EQU B'0'
	ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sub Rutinas
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;---------------------------------------
; Habilita la interrupci�n por timer 0 y establece los valores que controlan
; el tiempo que tardar� en lanzar su primer interrupci�n
LIBTIMER0__configurar:
	bcf STATUS, RP1
	bsf STATUS, RP0 ; Banco 1

	; Configuraci�n del temporizador
	; 5   T0CS  - Requiere un 0 para usar el relog interno
	; 3   PSA   - 0 para asignar el preescalador al timer0
	; 2:0 PS2:0 - 7 Valor del preescalado 256
	movlw (LIBTIMER0__PREESCALADOR | (LIBTIMER0_TOCS << 5))
	movwf (OPTION_REG & 0x7F)

	; Configurar las interrupciones. INTCON est� en todos los bancos
	bsf INTCON, TMR0IE ; Interrupci�n del timer 0
	bsf INTCON, GIE    ; Habilitador global de interrupci�n

	; Establecemos el valor del timer0/contador
	bcf STATUS, RP0 ; Banco 0
	movlw LIBTIMER0_TMR
	movwf TMR0
	return

;---------------------------------------
; Rutina para usarse dentro de la rutina de atenci�n a interrupci�n del
; timer 0. Guarda el estado del programa para que se recupere luego
LIBTIMER0__pre_interrupt:
	movwf LIBTIMER0__W_TEMP			; Guarda W
	swapf STATUS, W					; Para copiar, pero sin alterar banderas
	clrf STATUS						; Banco 0
	movwf LIBTIMER0__STATUS_TEMP	; Guarda STATUS (swapped)
	return

;---------------------------------------
; Rutina para usarse dentro de la rutina de atenci�n a interrupci�n del
; timer 0. Devuelve el valor configurado a TMR0 para que las pr�ximas
; interrupciones pasen tras el mismo tiempo y tambi�n recupera el estado
; del programa que guardamos en el pre�mbulo.
LIBTIMER0__post_interrupt:
	bcf STATUS, RP1
	bcf STATUS, RP0 ; Banco 0

	movlw LIBTIMER0_TMR
	movwf TMR0

	swapf LIBTIMER0__STATUS_TEMP, W
	movwf STATUS

	swapf LIBTIMER0__W_TEMP, F
	swapf LIBTIMER0__W_TEMP, W
	return