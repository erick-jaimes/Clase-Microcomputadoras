; Prueba de memoria_eeprom.inc
; Se prueba si se puede escribir y leer datos de 
; la eeprom integrada en el pic
  processor pic16f877a
  include <p16f877a.inc>

  org 0
    goto inicio
  org 5

	CBLOCK 0x20
		contador
		tope
	ENDC

  #DEFINE DEPURAR_POR_SEPARADO
  include "../code/comunicacion_serial.inc"
EEPROM__DIR_VARS EQU 0x120
  include "../code/memoria_eeprom.inc"

tabla:
	addwf PCL, F
	DT 0xA, 0xD, "tabla", 0
	IF high($) != high(tabla)
		ERROR "Se cruzan una dirección múltiplo de 256"
	ENDIF

inicio:
	call SERIAL__configurar
	call EEPROM__configurar

	movlw 0x30
	EEPROM__ir_banco_auxiliares
	movwf (EEPROM__dir & 0x7F)
	movlw 'a'
	call EEPROM__write_byte

	movlw 0x31
	EEPROM__ir_banco_auxiliares
	movwf (EEPROM__dir & 0x7F)
	movlw 'b'
	call EEPROM__write_byte

	clrw

	movlw 0x31
	EEPROM__ir_banco_auxiliares
	movwf (EEPROM__dir & 0x7F)
	call EEPROM__read_byte
	call SERIAL__transmitir_byte
	movlw 0x30
	EEPROM__ir_banco_auxiliares
	movwf (EEPROM__dir & 0x7F)
	call EEPROM__read_byte
	call SERIAL__transmitir_byte

	EEPROM__guardar_cadena tabla

	EEPROM__ir_banco_auxiliares
	movf (EEPROM__top & 0x7F), W
	bcf STATUS, RP1
	bcf STATUS, RP0

	movwf tope

	clrf contador

leer_tabla:
	movf contador, W
	EEPROM__ir_banco_auxiliares
	movwf EEPROM__dir
	
	subwf tope, W
	btfsc STATUS, Z
	goto fuera
	
	call EEPROM__read_byte
	call SERIAL__transmitir_byte

	incf contador, F
	goto leer_tabla

fuera:

	sleep
  end