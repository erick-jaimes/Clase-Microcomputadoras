	processor pic16f877a
	include p16f877a.inc
	
	org 0
		goto inicio

	org 5


inicio
	call LCD_Inicializa
	movlw 'E'
	call LCD_Caracter
	movlw 'q'
	call LCD_Caracter
	movlw 'u'
	call LCD_Caracter
	movlw 'i'
	call LCD_Caracter
	movlw 'p'
	call LCD_Caracter
	movlw 'o'
	call LCD_Caracter
	movlw ':'
	call LCD_Caracter
	movlw ' '
	call LCD_Caracter
	movlw '#'
	call LCD_Caracter
	movlw '0'
	call LCD_Caracter
	movlw '7'
	call LCD_Caracter
	call LCD_Linea2
	movlw 'S'
	call LCD_Caracter
	movlw 'a'
	call LCD_Caracter
	movlw 'u'
	call LCD_Caracter
	movlw 'l'
	call LCD_Caracter
	movlw ' '
	call LCD_Caracter
	movlw 'E'
	call LCD_Caracter
	movlw 'r'
	call LCD_Caracter
	movlw 'i'
	call LCD_Caracter
	movlw 'c'
	call LCD_Caracter
	movlw 'k'
	call LCD_Caracter
	sleep

  include "lcd_8bit.inc"
  end