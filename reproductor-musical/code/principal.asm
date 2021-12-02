;###############################################################################
;Programa para el pic16f877a que permite reproducir tonadas en un
;speaker
;###############################################################################
  processor pic16f877a
  include <p16f877a.inc>

  org 0
    goto inicio
  org 4
	goto interrupcion
  org 5

;;;;;;;;;;;; Bibliotecas incluidas ;;;;;;;;;;;;
NOTASMUSIC__DIR_VARS EQU 0x20
  #DEFINE NOTASMUSIC__SALIDA_SPEAKER PORTC, 0
  include "notas_musicales.inc"

  include "comunicacion_serial.inc"

CADENAS__DIR_VARS EQU 0x28
  include "cadenas_autogeneradas.inc"

EEPROM__DIR_VARS EQU 0x30
  include "memoria_eeprom.inc"

TONADA__DIR_VARS EQU 0x38
  include "tonadas_autogeneradas.inc"

; Se configura el timer para que genere una
; interrupción aproximadamente cada 1 milisegundo
TIMER0__PREESCALADOR EQU B'100' ; Divide frecuencia entre 32
TIMER0__TMR          EQU .100
  include "timer0.inc"

;;;;;;;;;;;; Variables y definiciones ;;;;;;;;;;;;
	CBLOCK 0x40
		opcion				; Para guardar la opción de menú elegida
		banderas			; Banderas (
		indice_menu			; Contador para desplegar opciones de menú
		contador			; Para cuando se requiere un segundo contador. Este es más genérico, multi uso
		longitud_titulo		; Número de bytes del título de una tonada
		longitud_tonada		; Número de bytes en que se almacena la propia tonada

		nota				; Si es do, re, mi, fa, etc
		retardo_high		; Parte alta de un retardo
		retardo_low			; Entre ambas partes forman un valor de 13 bits que representa el número de
							; milisegundos (aproximado) que perduarará el sonido de una nota
	ENDC

BANDERA_RITMO EQU 0
BANDERA_TERMINO_TONADA EQU 1

;;;;;;;;;;;; Programa ;;;;;;;;;;;;

;-- Inicio del programa --
inicio:
	bcf STATUS, RP1
	bcf STATUS, RP0 ; Banco 0
	clrf PORTB
	clrf PORTC
	bcf banderas, BANDERA_RITMO

	bsf STATUS, RP0 ; Banco 1

	; El puerto C se configura como salida salvo por RX
	movlw B'10000000'
	movwf (TRISC & 0x7F)

	; El puerto B se configura como entrada
	movlw 0xFF
	movwf (TRISB & 0x7F)

	; El resto de cosas son configuradas por las subrutinas correspondientes
	call SERIAL__configurar
	call CADENAS__configurar
	call EEPROM__configurar
    call TIMER0__configurar
	call TONADA__configurar

	; Guardamos la música en la EEPROM
	EEPROM__guardar_cadena martinillo

	EEPROM__guardar_cadena short_mario_bros
	EEPROM__guardar_cadena short_merry_christmas
	movlw 0
	call EEPROM__write_byte_top

	; Banner
	CADENAS__transmitir str_banner

;	clrf contador
;	movlw 2
;	movwf indice_menu
;	clrf EEPROM__dir
;	movlw 2
;	movwf opcion
;	goto reencontrar_tonada

;-- Ciclo principal del programa --
main:

	CADENAS__transmitir str_menu_principal

	; Recibimos un byte por la terminal serial y lo guardamos
	call SERIAL__recibir_byte
	movwf opcion

	; Convertimos la opción a mayúscula para que las opciones de
	; menú sean case insensitive
	bcf opcion, 5

	; Comprobamos la opción de menú elegida
	; a) Tocar la música manualmente
	movlw 'A'
	subwf opcion, W
	btfss STATUS, Z
	goto probar_b

	;pagesel tocar_musica
	goto tocar_musica

probar_b
	; b) Escuchar alguna canción de la lista
	movlw 'B'
	subwf opcion, W
	btfss STATUS, Z
	goto opcion_incrrecta

	;pagesel escuchar_musica
	goto escuchar_musica

opcion_incrrecta
	CADENAS__transmitir str_opcion_incorrecta
	call SERIAL__recibir_byte

    goto main

;-- El usuario toca la música --
tocar_musica:
	CADENAS__transmitir str_modo_tocar_musica

ciclo_musical:

probar_do:
	btfss PORTB, 0
	goto probar_re

	;pagesel cambiar_bit_ritmo
	call cambiar_bit_ritmo
sonar_do:
	;pagesel NOTASMUSIC__sonarDO4
	call NOTASMUSIC__sonarDO4
	btfsc PORTB, 0
	goto sonar_do
	goto probar_salir

probar_re:
	btfss PORTB, 1
	goto probar_mi

	;pagesel cambiar_bit_ritmo
	call cambiar_bit_ritmo
sonar_re:
	;pagesel NOTASMUSIC__sonarRE4
	call NOTASMUSIC__sonarRE4
	btfsc PORTB, 1
	goto sonar_re
	goto probar_salir

probar_mi:
	btfss PORTB, 2
	goto probar_fa

	;pagesel cambiar_bit_ritmo
	call cambiar_bit_ritmo
sonar_mi:
	;pagesel NOTASMUSIC__sonarMI4
	call NOTASMUSIC__sonarMI4
	btfsc PORTB, 2
	goto sonar_mi
	goto probar_salir

probar_fa:
	btfss PORTB, 3
	goto probar_sol

	;pagesel cambiar_bit_ritmo
	call cambiar_bit_ritmo
sonar_fa:
	;pagesel NOTASMUSIC__sonarFA4
	call NOTASMUSIC__sonarFA4
	btfsc PORTB, 3
	goto sonar_fa
	goto probar_salir

probar_sol:
	btfss PORTB, 4
	goto probar_la

	;pagesel cambiar_bit_ritmo
	call cambiar_bit_ritmo
sonar_sol:
	;pagesel NOTASMUSIC__sonarSOL4
	call NOTASMUSIC__sonarSOL4
	btfsc PORTB, 4
	goto sonar_sol
	goto probar_salir

probar_la:
	btfss PORTB, 5
	goto probar_si

	;pagesel cambiar_bit_ritmo
	call cambiar_bit_ritmo
sonar_la:
	;pagesel NOTASMUSIC__sonarLA4
	call NOTASMUSIC__sonarLA4
	btfsc PORTB, 5
	goto sonar_la
	goto probar_salir

probar_si:
	btfss PORTB, 6
	goto probar_salir

	;pagesel cambiar_bit_ritmo
	call cambiar_bit_ritmo
sonar_si:
	;pagesel NOTASMUSIC__sonarSI4
	call NOTASMUSIC__sonarSI4
	btfsc PORTB, 6
	goto sonar_si

probar_salir
	btfss PORTB, 7
	goto ciclo_musical

	;pagesel main
	goto main

;-- La música guardada en la EEPROM es interpretada por el PIC
escuchar_musica:
	CADENAS__transmitir str_modo_escuchar_musica

	clrf EEPROM__dir
	clrf opcion
	clrf indice_menu

imprimir_menu:
	; El primer byte que se lee de la EEPROM es la longitud
	; del título
	;pagesel EEPROM__read_byte
	call EEPROM__read_byte

	; Si es 0, dejamos de imprimir el menú (ya no hay opción que imprimir)
	btfss STATUS, Z
	goto continuar_menu

	;pagesel solicitar_opcion
	goto solicitar_opcion

continuar_menu:
	; Guardamos la longitud para que controle el ciclo de impresión
	movwf longitud_titulo

	movlw 'a'
	addwf indice_menu, W
	;pagesel SERIAL__transmitir_byte
	call SERIAL__transmitir_byte

	movlw ')'
	;pagesel SERIAL__transmitir_byte
	call SERIAL__transmitir_byte
	movlw ' '
	;pagesel SERIAL__transmitir_byte
	call SERIAL__transmitir_byte

	clrf contador

imprimir_titulo:
	movf longitud_titulo, W
	subwf contador, W
	btfsc STATUS, Z
	goto final_titulo

	incf EEPROM__dir, F
	;pagesel EEPROM__read_byte
	call EEPROM__read_byte
	;pagesel SERIAL__transmitir_byte
	call SERIAL__transmitir_byte

	incf contador, F
	goto imprimir_titulo

final_titulo:
	;pagesel SERIAL__salto_linea
	call SERIAL__salto_linea

	; Tras imprimir un título, vamos a la siguiente canción
	incf indice_menu, F

	; El siguiente byte leido es la longitud de los datos
	; de la canción. Como solo vamos a imprimir el menú,
	; nos saltaremos esta los bytes indicados
	incf EEPROM__dir, F
	;pagesel EEPROM__read_byte
	call EEPROM__read_byte

	addwf EEPROM__dir, F
	incf EEPROM__dir, F

	goto imprimir_menu

solicitar_opcion:
	; Recibimos la opción solicitada
	;pagesel SERIAL__recibir_byte
	call SERIAL__recibir_byte
	movwf opcion
	bcf opcion, 5

	; Restamos 'a' para obtener un índice
	movlw 'A'
	subwf opcion, F

	; indice_menu contiene el número de opciones de menú
	; Le restamos 1 para que sea el índice de la opción mayor
	decf indice_menu, F

	; Comprobamos que la opción elegida tiene que ser menor o
	; igual al índice de la opción mayor
	movf opcion, W
	subwf indice_menu, W

	btfss STATUS, C
	goto tonada_incorrecta

	; La tonada elegida existe, pero no podemos tocarla directamente
	; Primero hay que volver a encontrar su posición
	clrf EEPROM__dir
	clrf contador

reencontrar_tonada:
	; Nos saltamos el título
	;pagesel EEPROM__read_byte
	call EEPROM__read_byte
	movwf longitud_titulo
	addwf EEPROM__dir, F
	incf EEPROM__dir, F

	; Leemos la longitud del cuerpo de la tonada
	;pagesel EEPROM__read_byte
	call EEPROM__read_byte
	movwf longitud_tonada

	; Si esta es la canción indicada (contador == opcion), terminamos
	movf contador, W
	subwf opcion, W

	btfsc STATUS, Z
	goto preparar_timer

	movf longitud_tonada, W
	addwf EEPROM__dir, F
	incf EEPROM__dir, F

	incf contador, F
	goto reencontrar_tonada

preparar_timer:

;	goto prueba_interrupcion

	; Preparamos las variables para empezar a tocar música
	bcf banderas, BANDERA_TERMINO_TONADA
	movlw 0x7
	movwf nota
	clrf contador
	movlw 0x1
	movwf retardo_high
	movwf retardo_low

	; Activamos la interrupción por TIMER0, ya que este controlará
	; la música
	bsf INTCON, TMR0IE
	bsf INTCON, GIE

	bcf STATUS, RP1
	bcf STATUS, RP0

tocar_tonada:
	movf nota, W

	; pagesel NOTASMUSIC__sonarW
	call NOTASMUSIC__sonarW

	btfss banderas, BANDERA_TERMINO_TONADA
	goto tocar_tonada

	bcf INTCON, GIE
	bcf INTCON, TMR0IE
	;pagesel main
	goto main

	; Aquí llegamos con opciones incorrectas de menú
tonada_incorrecta:
	CADENAS__transmitir str_opcion_incorrecta
	;pagesel SERIAL__recibir_byte
	call SERIAL__recibir_byte
	;pagesel main
	goto main

;-- Controla los tiempos que dura cada nota musical --
interrupcion:
    call TIMER0__pre_interrupt
    ; Comprobar que la fuente de interrupción sea el
    ; timer 0
    btfss INTCON, TMR0IF
    goto final_interrupcion

    bcf INTCON, TMR0IF

    ; El código de importancia dentro de la interrupción
    ; se ejecuta cada que ambos contadores valgan 0
    decfsz retardo_low, F
    goto final_interrupcion
    decfsz retardo_high, F
    goto final_interrupcion

	; Revisamos si ya terminamos de reproducir la tonada
	movf contador, W
	subwf longitud_tonada, W
	btfss STATUS, Z
	goto nota_siguiente

	bsf banderas, BANDERA_TERMINO_TONADA
	goto final_interrupcion

nota_siguiente:
;prueba_interrupcion:
	; El primer byte guarda 3 bits de la nota y 5 bits
	; para retardo_high
	incf contador, F
	incf EEPROM__dir, F
	call EEPROM__read_byte
	movwf retardo_high
	movwf nota

	movlw B'00011111'
	andwf retardo_high, F
	incf retardo_high, F
	btfsc retardo_high, 5
	clrf retardo_high

	movlw B'11100000'
	andwf nota, F
	swapf nota, F
	bcf STATUS, C
	rrf nota, F

	; El segundo byte guarda retardo_low

	incf contador, F
	incf EEPROM__dir, F
	call EEPROM__read_byte
	movwf retardo_low
	incf retardo_low, F

;goto prueba_interrupcion

final_interrupcion:
    call TIMER0__post_interrupt
    retfie

;-- Se usa un bit del registro ritmo para representar en el osciloscopio
; cuando se presiona un botón
cambiar_bit_ritmo:
	movlw 0x1
	xorwf banderas, F

	btfss banderas, BANDERA_RITMO
	bcf PORTC, 3
	btfsc banderas, BANDERA_RITMO
	bsf PORTC, 3
	return

  end
