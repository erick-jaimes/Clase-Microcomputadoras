#include <16f877a.h>                      //Tipo de MicroControlador a utilizar
#fuses HS,NOPROTECT,NOWDT,NOLVP
#device ADC=8
#use delay(clock=20000000)                //Frec. de Osc. 20Mhz
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#define LCD_RS_PIN      PIN_D7
#define LCD_RW_PIN      PIN_D0 /* NO se usa */
#define LCD_ENABLE_PIN  PIN_D6

#define LCD_DATA4       PIN_D5
#define LCD_DATA5       PIN_D4
#define LCD_DATA6       PIN_D3
#define LCD_DATA7       PIN_D2
#include <lcd.c>

#include <input.c>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int cadenas_iguales(char * str1, char * str2)
{
   return strcmp(str1, str2) == 0;
}

int cadena_empieza_con(char * cadena, char * prefijo)
{
   int longitud_prefijo = strlen(prefijo);
   return strncmp(cadena, prefijo, longitud_prefijo) == 0;
}

void ajustaPWM(int16 ciclo_trabajo);

void main()
{
   // Cadena para guardar lo que escriben los usuarios
   char comando_usuario[32];
   // Cadena para guardar temporalmente el nombre de un comando (ej, motor off)
   // se usa para comparar con el texto ingresado
   char comando_sistema[32];

   // Valor de la temperatura leído por el sensor
   float temp;
   
   // Ciclo de trabajo del PWM que controla el motor
   int16 ciclo_trabajo = 50;

   // Contadores auxiliares
   int i;

   ///////////////////////////////
   lcd_init();
   ///////////////////////////////
   output_high(PIN_C0);
   output_low(PIN_C1);
   ///////////////////////////////
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   delay_us(50);
   ////////////////////
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_16,155,1);
   set_pwm1_duty(50);

   for (;;)
   {
      get_string(comando_usuario, 31);
      printf(lcd_putc, "\fEl comando es: %s\n", comando_usuario);
      for (i = 0; comando_usuario[i] != '\0'; i++)
      {
         comando_usuario[i] = tolower(comando_usuario[i]);
      }
 
      strcpy(comando_sistema, "temperatura");
      if ( cadenas_iguales(comando_usuario, comando_sistema) )
      {
         temp = 0.488758 * read_adc();
         printf("Temperatura = %2.2f [C]\n", temp);
         lcd_gotoxy(1, 2);
         printf(lcd_putc, "Temperatura = %2.2f [C]\n", temp);
         continue;
      }

      strcpy(comando_sistema, "motor on");
      if ( cadenas_iguales(comando_usuario, comando_sistema) )
      {
         printf("Se enciende el motor\n");
         lcd_gotoxy(1, 2);
         printf(lcd_putc, "Se enciende el motor\n");
         ajustaPWM(ciclo_trabajo);
         continue;
      }

      strcpy(comando_sistema, "motor off");
      if ( cadenas_iguales(comando_usuario, comando_sistema) )
      {
         printf("Se apaga el motor\n");
         lcd_gotoxy(1, 2);
         printf(lcd_putc, "Se apaga el motor\n");
         ajustaPWM(0);
         continue;
      }

      strcpy(comando_sistema, "leds on");
      if ( cadenas_iguales(comando_usuario, comando_sistema) )
      {
         printf("Se encienden los leds\n");
         lcd_gotoxy(1, 2);
         printf(lcd_putc, "Se encienden los leds\n");
         output_b(0xFF);
         continue;
      }

      strcpy(comando_sistema, "leds_off");
      if ( cadenas_iguales(comando_usuario, comando_sistema) )
      {
         printf("Se apagan los leds\n");
         lcd_gotoxy(1, 1);
         printf(lcd_putc, "Se apagan los leds\n");
         output_b(0x00);
         continue;
      }

      strcpy(comando_sistema, "pwm=");
      if ( cadena_empieza_con(comando_usuario, comando_sistema) )
      {
         ciclo_trabajo = atol(comando_usuario + 4);
         ajustaPWM(ciclo_trabajo);
         continue;
      }

      printf("Comando no reconocido\n");

      delay_ms(10);

   }

}

void ajustaPWM(int16 ciclo_trabajo)
{
   if (ciclo_trabajo >= 0 && ciclo_trabajo <= 100)
   {
      set_pwm1_duty((ciclo_trabajo * 624 / 100));
   }
   else
   {
      set_pwm1_duty(0);
   }
}

