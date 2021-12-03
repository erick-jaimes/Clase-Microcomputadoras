#include <16f877a.h>                      //Tipo de MicroControlador a utilizar
#fuses HS,NOPROTECT,NOWDT,NOLVP, XT
#device ADC=10
#use delay(clock=20000000)                //Frec. de Osc. 20Mhz
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#include <stdio.h>

void ajustaPWM(float ciclo_trabajo);

void main()
{
   int16 valor_potenciometro;
   int16 valor_botones = 512;
   int1 modo_perilla = TRUE; // Si modo perilla es TRUE, el valor potenciometro
                             // determinará la intensidad de la lámpara
                             // Si es FALSE, la intensidad estará dada por
                             // valor botones
   
   /* Se configuran los pines B0 y B1 para recibir la entrada de los botones */
   set_tris_b(0b11);

   /* Se configura el conversor AD para poder leer los valores de un potenciometro */
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   delay_us(50);
   
   /* Se configura el módulo CCP1 como PWM para controlar la intensidad de la lámpara */
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_16, 155, 1);
   // set_pwm1_duty(312);
   
   for (;;)
   {
      if (modo_perilla)
      {
         valor_potenciometro = read_adc();
         ajustaPWM(valor_potenciometro * 100.0 / 1024.0);
         delay_ms(20);
      }
      else
      {
         if (input(PIN_B0) && valor_botones >= 8)
         {
             valor_botones -= 8;
         }
         if (input(PIN_B1) && valor_botones <= 1016)
         {

            valor_botones += 8;
         }
         ajustaPWM(valor_botones);
         delay_ms(10);
      }
   }
}

void ajustaPWM(float ciclo_trabajo)
{
   float duty = ciclo_trabajo * 624 / 100.0;
   printf("ciclo_trabajo: %.2f    Duty: %.2f %ld\n\r", ciclo_trabajo, duty, (int16)duty);
   if (ciclo_trabajo >= 0.0 && ciclo_trabajo <= 100.0)
   {
      set_pwm1_duty((int16)duty);
   }
   else
   {
      set_pwm1_duty(0);
   }
}

