/*43.2.-Secuencia de encendido de todos los leds de la matriz 8X8, los va recorriendo uno por uno de fila en 
fila y columna en columna, pero sin apagar el actual antes de pasar al siguiente, esto se repite hasta el 
último led de la última fila y columna.*/

//IMPORTACIÓN DE LIBRERÍAS
/*Para poder importar una librería debemos dar clic en el botón de: 
Programa -> Incluir Librería -> Administrar Bibliotecas -> Buscar: LedControl -> Instalar
Luego ya habiendo realizado eso se debe incluir alguna de las siguientes instrucciones: 
#include <NombreLibrería.h>
#include "NombreLibrería.h"*/
#include <LedControl.h> //Librería de control de Matrices led 8X8.

/*Objeto de la clase LedControl, este pide en sus 4 parámetros de su constructor lo siguiente: 
  - Primer parámetro: Indica el Pin del Arduino al que está conectado el Pin DIN (Master Out 
    Slave In) de la matriz led 8X8, que indica la dirección de circulación de datos.
  - Segundo parámetro: Indica el Pin del Arduino al que está conectado el Pin CLK, que es el 
    pulso de reloj que marca el paso con el que se mandan los datos del maestro al esclavo.
  - Tercer parámetro: Indica el Pin del Arduino al que está conectado el Pin CS (Chip Select),
    que habilita la función del circuito integrado MAX7219 que controla a la matriz led 8X8.
  - Cuarto parámetro: Indica el número de matrices a controlar, que deben estar conectadas 
  entre ellas a través del pin DOUT, la librería permite manejar hasta 8 matrices conectadas.*/
LedControl matrizLed = LedControl(11, 13, 10, 1); //DIN = Pin11, CLK = Pin13, CS = Pin10, #MatricesLed.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*LedControl.shutdown(): Método que permite encender la matriz led 8X8, se cuenta el número de 
  matrices empezando desde cero: 
    - Primer parámetro: Indica la matriz que se quiere encender, indicando su numeración desde cero.
    - Segundo parámetro: Indica que la matriz está encendida con false y que está apagada con true.*/
  matrizLed.shutdown(0, false);
  /*LedControl.setIntensity(): Método que permite indicar el nivel de luminosidad de cada matriz led 8X8, 
  se cuenta el número de matrices empezando desde cero: 
    - Primer parámetro: Indica la matriz a la que se le quiere cambiar su nivel de luminosidad, indicando 
      su numeración desde cero.
    - Segundo parámetro: Indica la luminosidad de los leds de la matriz con números de 0 a 15, usualmente 
      se usa la luminosidad 4 en espacios no tan iluminados o al aire libre.*/
  matrizLed.setIntensity(0, 8);
  /*LedControl.clearDisplay(): Método que limpia cualquier secuencia previamente cargada en la matriz 8X8: 
    - Primer parámetro: Indica la matriz a la que se le quiere borrar una secuencia pasada, indicando su 
      numeración desde el número cero.*/
  matrizLed.clearDisplay(0);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Secuencia de encendido y apagado de un solo led que se va 
//recorriendo de fila en fila.
void loop() {
  for(int fila = 0; fila < 8; fila++){
    for(int columna = 0; columna < 8; columna++){
      /*LedControl.setLed(): Método que prende o apaga un led que se encuentre en un número de fila y 
      columna en específico, para ello se le debe indicar lo siguiente:
      - Primer parámetro: Indica la matriz a la que se le quiere encender o apagar un led, exponiendo su 
        numeración desde el número cero.
      - Segundo parámetro: Indica la fila a la que pertenece el led que se quiere encender o apagar.
      - Tercer parámetro: Indica la columna a la que pertenece el led que se quiere encender o apagar.
      - Cuarto parámetro: Indica si se quiere encender con true o apagar con false un led.*/
      matrizLed.setLed(0, fila, columna, true);
      /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
      delay(50); //Detiene el código 50 milisegundos antes de encender el siguiente.
    }
  }
  /*Al terminar de encender todos los leds de la matriz 8X8, los vuelve a apagar antes de empezar la secuencia 
  de nuevo.*/
  matrizLed.clearDisplay(0);
}
