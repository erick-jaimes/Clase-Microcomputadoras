/*
EQUIPO 7    TEORIA DE MICROCOMPUTADORAS
SEMESTRE 2022-1     GRUPO: 04
*/

//Versión del MicroControlador a utilizar
#include <16f877a.h>                      
#fuses HS,NOPROTECT,NOWDT,NOLVP
#device ADC=8

#use delay(clock=20000000)  //Frecuencia del Oscilador 20Mhz

void ajustaPWM(int16 cicloTrabajo);

void main(){
   
   int valorAD;
//CONFIGURACION DE LOS PINES C_00 Y C_01
   output_high(PIN_C0);
   output_low(PIN_C1); 
////////////////Lectura de Convertidor AD
   setup_adc_ports(AN0);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);
   delay_us(50);
/////////// Configuracion del PIN_C2 y la entrada del ENL1
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_16,155,1);   
   set_pwm1_duty(0);
//ciclo
  while(TRUE){
       
       valorAD = read_adc();
       ajustaPWM((int16)valorAD*100/255);
       delay_ms(10);
  
 } 
 
}
 /*
 Funcion para la modulacion del PWM en un rango del 0 al 100
 */
 void ajustaPWM(int16 cicloTrabajo)
{
   if(cicloTrabajo>=0 && cicloTrabajo<=100)
   {
      set_pwm1_duty((int16)(cicloTrabajo*624/100));
   }
   else
   {
      set_pwm1_duty(0);
   }
}
 
 

