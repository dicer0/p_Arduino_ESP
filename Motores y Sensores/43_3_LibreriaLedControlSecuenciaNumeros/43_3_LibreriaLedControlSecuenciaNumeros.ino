/*43.3.-Animación de números mostrada en una matriz led 8X8.*/

//IMPORTACIÓN DE LIBRERÍAS
/*Para poder importar una librería debemos dar clic en el botón de: 
Programa -> Incluir Librería -> Administrar Bibliotecas -> Buscar: LedControl -> Instalar
Luego ya habiendo realizado eso se debe incluir alguna de las siguientes instrucciones: 
#include <NombreLibrería.h>
#include "NombreLibrería.h"*/
#include "LedControl.h" //Librería de control de Matrices led 8X8.

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

//DECLARACIÓN DE VARIABLES Y CONSTANTES: Los leds de la matriz se encienden con 1 y se apagan con 0.
/*Variable que detiene el programa antes de enseñar un número distinto en la matriz led 8X8.*/
#define demora 500 //Delay de 1000 ms.
/*Variable que representa el número cero.*/
byte cero [8] = {
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B00111000
};
/*Variable que representa el número uno.*/
byte uno [8] = {
  B00010000,
  B00110000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00111000
};
/*Variable que representa el número dos.*/
byte dos[8]= {
  B00111000,
  B01000100,
  B00000100,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01111100
};
/*Variable que representa el número tres.*/
byte tres[8]= {
  B00111000,
  B01000100,
  B00000100,
  B00011000,
  B00000100,
  B00000100,
  B01000100,
  B00111000
};
/*Variable que representa el número cuatro.*/
byte cuatro[8]= {
  B00001000,
  B00011000,
  B00101000,
  B01001000,
  B01001000,
  B01111100,
  B00001000,
  B00001000
};
/*Variable que representa el número cinco.*/
byte cinco[8]= {
  B01111100,
  B01000000,
  B01000000,
  B01111000,
  B00000100,
  B00000100,
  B01000100,
  B00111000
};
/*Variable que representa el número seis.*/
byte seis[8]= {
  B00111000,
  B01000100,
  B01000000,
  B01111000,
  B01000100,
  B01000100,
  B01000100,
  B00111000
};
/*Variable que representa el número siete.*/
byte siete[8]= {
  B01111100,
  B00000100,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B00100000,
  B00100000
};
/*Variable que representa el número ocho.*/
byte ocho[8]= {
  B00111000,
  B01000100,
  B01000100,
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B00111000
};
/*Variable que representa el número nueve.*/
byte nueve[8]= {
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B00111100,
  B00000100,
  B01000100,
  B00111000
};

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

//FUNCIONES PROPIAS UTILIZADAS PARA MOSTRAR CADA NÚMERO:
void mostrar_0() {
  for(int fila = 0; fila < 8; fila++){
    /*LedControl.setRow(): Método que prende o apaga varios leds que muestren un símbolo, número o lo que 
    sea, pero para ello antes se debe haber creado una matriz de números binarios donde se cree la figura 
    encendiendo leds mandándole a la matriz un 1 lógico y apagándolos con un 0 lógico:
    - Primer parámetro: Indica la matriz a la que se le quiere encender o apagar un led, exponiendo su 
      numeración desde el número cero.
    - Segundo parámetro: Indica la fila a la que pertenece el led que se quiere encender o apagar.
    - Tercer parámetro: Indica el array que describe la figura a mostrar, donde se enciende un led con 1
      y se apaga con 0.*/
    matrizLed.setRow(0, fila, cero[fila]);
  }
}
void mostrar_1() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, uno[fila]);
  }
}
void mostrar_2() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, dos[fila]);
  }
}
void mostrar_3() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, tres[fila]);
  }
}
void mostrar_4() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, cuatro[fila]);
  }
}
void mostrar_5() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, cinco[fila]);
  }
}
void mostrar_6() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, seis[fila]);
  }
}
void mostrar_7() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, siete[fila]);
  }
}
void mostrar_8() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, ocho[fila]);
  }
}
void mostrar_9() {
  for(int fila = 0; fila < 8; fila++){
    matrizLed.setRow(0, fila, nueve[fila]);
  }
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Secuencia de encendido y apagado de un solo led que se va 
//recorriendo de fila en fila.
void loop() {
  for(int fila = 0; fila < 8; fila++){
    for(int columna = 0; columna < 8; columna++){
      /*Uso de la función propia declarada previamente.*/
      mostrar_0();
      /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
      delay(demora); //Detiene el código antes de mostrar el siguiente número.
      mostrar_1();
      delay(demora); 
      mostrar_2();
      delay(demora); 
      mostrar_3();
      delay(demora); 
      mostrar_4();
      delay(demora); 
      mostrar_5();
      delay(demora); 
      mostrar_6();
      delay(demora); 
      mostrar_7();
      delay(demora); 
      mostrar_8();
      delay(demora); 
      mostrar_9();
      delay(demora); 
    }
  }
}
