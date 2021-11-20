; Librería para el control de LCD, basada en LCD_4BIT.INC que
; aparece a partir de la página 196 del libro "Microcontrolador PIC16F87",
; pero adaptándola a 8 bits

	CBLOCK
	LCD_Dato
	LCD_GuardaDato
	LCD_GuardaTRISB
	LCD_Auxiliar1
	LCD_Auxiliar2
	ENDC

LCD_CaracteresPorLinea EQU .16

#DEFINE LCD_PinRS		PORTC, 5
#DEFINE LCD_PinRW		PORTC, 6
#DEFINE LCD_PinEnable	PORTC, 7

#DEFINE LCD_BUSDatos	PORTD

LCD_Inicializa
	bcf STATUS, RP1
	bsf STATUS, RP0 ; Banco 1, donde se configuran los TRIS
	bcf LCD_PinRS
	bcf LCD_PinEnable
	bcf LCD_PinRW
	bcf STATUS, RP0 ; Banco 0
	bcf LCD_PinRW