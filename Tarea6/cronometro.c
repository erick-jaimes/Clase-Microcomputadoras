#include <16f877a.h>
#use delay(clock=20000000)

const unsigned int SEVEN_SEG[] = {
   ~0b00111111,
   ~0b00000110,
   ~0b01011011,
   ~0b01001111,
   ~0b01100110,
   ~0b01101101,
   ~0b01111101,
   ~0b00000111,
   ~0b01111111,
   ~0b01100111
};

void SalidaCronometro(int minutos, int segundos) {
   int minutos_decenas, minutos_unidades;
   minutos_decenas = minutos / 10;
   minutos_unidades = minutos % 10;
   
   int segundos_decenas, segundos_unidades;
   segundos_decenas = segundos / 10;
   segundos_unidades = segundos % 10;
   
   output_c(0b1000);
   output_d(SEVEN_SEG[minutos_decenas]);
   delay_ms(10);

   output_c(0b0001);
   output_d(SEVEN_SEG[minutos_unidades]);
   delay_ms(10);

   output_c(0b0010);
   output_d(SEVEN_SEG[segundos_decenas]);
   delay_ms(10);

   output_c(0b0100);
   output_d(SEVEN_SEG[segundos_unidades]);
   delay_ms(10);
}

void main(void)
{
   set_tris_b(0b00000011); // Entradas
   set_tris_c(0b00000000); // Salida de control
   set_tris_d(0b00000000); // Salida del dígito
   
   int minutos = 59;
   int segundos = 30;
   
   int contador = 0;
   
   for (;;) {
    
      SalidaCronometro(minutos, segundos); // Casi 40ms
      contador = (contador + 1) % 25;
      
      if (contador == 0) { // Contador se vuelve 0 aproximadamente cada segundo
         segundos++;
         if (segundos >= 60) {
            segundos = 0;
            minutos = (minutos + 1) % 60;
         }
      }

   }
}
