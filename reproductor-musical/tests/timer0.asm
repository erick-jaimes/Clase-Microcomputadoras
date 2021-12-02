; Prueba de las rutinas para configurar la interrupción por timer 0
  processor pic16f877a
  include p16f877a.inc

    CBLOCK 0x20
        escala1
        escala2
        contador1
        contador2
        salida
        d1
        d2
    ENDC

  org 0
    goto inicio
  org 4
    goto interrupcion
  org 5

; Se configura el timer para que genere una
; interrupción aproximadamente cada 1 milisegundo
TIMER0__PREESCALADOR EQU B'100' ; Divide frecuencia entre 32
TIMER0__TMR             EQU .100
TIMER0__T0CS            EQU B'0'
    include "../code/timer0.inc"


inicio:
    bsf STATUS, RP0 ; Banco 1

    movlw 0xFF
    movwf (TRISB & 0x7F)
    movwf (TRISC & 0x7F)

    clrf (TRISD & 0x7F)
    clrf (TRISA & 0x7F)
    clrf (TRISE & 0x7F)

    movlw 0x6
    movwf (ADCON1 & 0x7F)

    bcf STATUS, RP0 ; Banco 0

    movlw B'10101010'
    movwf salida
    movwf PORTD

    call leer_escalas

    movf escala2, W
    movwf contador2
    
    movf escala1, W
    movwf contador1

    call TIMER0__configurar

ciclo:
    call leer_escalas
    call retardo_10ms
    goto ciclo

leer_escalas:
    bcf STATUS, RP1
    bcf STATUS, RP0

    ; Sucedian errores extraños y después de muchos dolres de cabeza
    ; y depuración fue evidente que las dos operaciones para leer el valor
    ; del puerto B y C y escribirlos en escala2 y escala1 deben ser atómicas, en
    ; el sentido de que no deben ser interrumpidas por el timer hasta que ambas
    ; se completen.
    ; Debido a que no estoy seguro de cómo representar una operación atómica,
    ; símplemente voy a deshabilitar la interrupción durante este periodo y
    ; volveré a habilitarla después.
    bcf INTCON, TMR0IE
    movf PORTB, W
    movwf escala2
    movlw B'00011111'
    andwf escala2, F
    
    incf escala2, F
    movf escala2, W
    btfsc escala2, 5
    clrf escala2

    movf PORTC, W
    movwf escala1
    incf escala1, F
    ; Termina la zona crítica
    bsf INTCON, TMR0IE
    return

interrupcion:
    call TIMER0__pre_interrupt
    ; Comprobar que la fuente de interrupción sea el
    ; timer 0
    btfss INTCON, TMR0IF
    goto final_interrupcion

    bcf INTCON, TMR0IF

    ; El código de importancia dentro de la interrupción
    ; se ejecuta cada que ambos contadores valgan 0
    decfsz contador1, F
    goto final_interrupcion
    decfsz contador2, F
    goto final_interrupcion

    comf salida, F
    movf salida, W
    movwf PORTD

    movf escala1, W
    movwf contador1
    movf escala2, W
    movwf contador2

final_interrupcion:
    call TIMER0__post_interrupt
    retfie

retardo_10ms:
    movlw 0xed
    movwf d1
    movlw 0x41
    movwf d2
retardo_10ms_0
    decfsz d1, 1
    goto retardo_10ms_0
    decfsz d2, 1
    goto retardo_10ms_0
    return

  end
