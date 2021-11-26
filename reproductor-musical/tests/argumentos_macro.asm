  processor pic16f877a
  include p16f877a.inc

  org 0
	goto inicio
  org 5

DIR EQU 0x20
VALOR EQU 0x21

	hola MACRO a, b
		bsf STATUS, a
		bsf STATUS, b
	ENDM

inicio:
	hola RP1, RP0
	movlw B'10101010'
	movwf 0x10
	goto inicio
  end