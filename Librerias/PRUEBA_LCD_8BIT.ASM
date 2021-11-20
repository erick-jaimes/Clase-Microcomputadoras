	processor pic16f877a
	include p16f877a.inc
	
	org 0
		goto inicio

	org 5


inicio
	call LCD_Inicializa
	movlw 'H'
	call LCD_Caracter
	movlw 'o'
	call LCD_Caracter
	movlw 'l'
	call LCD_Caracter
	movlw 'a'
	call LCD_Caracter
	
	sleep

  include "LCD_8BIT.INC"
  end