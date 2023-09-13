//IMPORTACIÓN DE LIBRERÍAS
/*Para poder importar una librería debemos dar clic en el botón de: 
Programa -> Incluir Librería -> Administrar Bibliotecas -> Buscar: Stepper -> Instalar
Luego ya habiendo realizado eso se debe incluir alguna de las siguientes instrucciones: 
#include <NombreLibrería.h>
#include "NombreLibrería.h"*/
#include <Stepper.h> //Librería de control de motores a paso.

/*Objeto de la clase Stepper, este solamente permite utilizar la secuencia de paso 
doble (o conpleto) para mover el motor a pasos y recibe como parámetros: 
  - Los pasos que resibe el motor para lograr un giro completo del eje
        - El modelo 28BYJ-48 en su paso doble ejecuta 2,048 pasos.
  - Los 4 siguientes parámetros son los pines digitales en los cuales están 
  conectadas las 4 bobinas del motor a pasos en el órden: A, C, B y D.*/
Stepper motorAPasos(2048, 8, 10, 9, 11); 
//Motor a pasos con paso doble conectado a los pines 8, 9, 10 y 11.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*Stepper.setSpeed(): Método que permite establecer la velocidad del motor a pasos
  en revoluciones por minuto: 
    - El modelo 28BYJ-48 puede recibir como parámetro los valores: 
      - 1 rpm: 1 vuelta en 1 minuto implica que la frecuencia de los pasos sea de 29.3 ms.
      - 2 rpm: 2 vueltas en 1 minuto implica que la frecuencia de los pasos sea de 14.6 ms.
      - 3 rpm: 3 vueltas en 1 minuto implica que la frecuencia de los pasos sea de 9.8 ms.*/
  motorAPasos.setSpeed(3);
  /*Serial.begin(baudRate): Este método inicializa la comunicación serial entre la placa Arduino 
  y la computadora, además de que configura su velocidad de transmisión dada en unidad de baudios 
  (bit trasmitido por segundo) que recibe como su único parámetro:
      - En general, 9600 baudios es una velocidad de transmisión comúnmente utilizada y es 
        compatible con la mayoría de los dispositivos y programas. 
      - Sin embargo, si se necesita una transferencia de datos más rápida y el hardware/software 
        lo admiten, se puede optar por velocidades más altas como 115200 o 57600 baudios.
  Es importante asegurarse de que la velocidad de transmisión especificada coincida con la 
  velocidad de comunicación del otro dispositivo al que se conecta el Arduino. Si la velocidad de 
  transmisión no coincide, los datos pueden no transmitirse o recibirse correctamente.*/
  Serial.begin(9600);   //Comunicación serial de 9600 baudios, para recibir los pasos del motor.
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Secuencia de Movimiento de un Motor a Pasos.
void loop() {
  //Secuencia preprogramada en el motor a pasos:
  /*Stepper.step(): Método que permite indicar la cantidad de pasos que queremos que ejecute 
  el motor, indicando con un signo su dirección.*/
  /*El motor a pasos 28BYJ-48 cuenta con un sistema de engranes que reducen su velocidad y aumentan 
  su torque, por lo que el motor debe dar un total de 2,048 pasos para dar una vuelta completa en el 
  paso simple y doble, pero en el paso medio esto se aumenta a ser 4,096 pasos para dar una vuelta 
  completa, esto debido a que el número de las acciones de la secuencia puede variar, pero para 
  que el rotor gire, a fuerza se deben cumplir 8 ciclos, por lo que: 
  - 64 vueltas completas del rotor original corresponden a 1 vuelta del eje del motor, dando un 
    giro por de 5.625° por paso al eje final, pero para que el rotor original gire una vuelta se 
    necesitan 32 pasos (8*4), dando un giro de 11.25° por paso al eje original.
  Esta situación aunada a que:
    - Un ciclo del paso doble consta de 4 acciones, da como resultado que un giro completo del rotor 
    conste de 32/4 = 8 ciclos.
  
  En conclusión, el motor necesita dar: 
    #pasosGiro*#pasosCiclo*#ciclosGiroRotor = 64*32 = 64*4*8 = 2048 pasos para dar una vuelta.
  Esto en código se transfiere a dar #pasosGiro*#ciclosGiroRotor = 64*8 = 512 ciclos, porque la 
  secuencia del paso simple consta de 8 pasos, que se indican dentro del mismo bucle for.*/
  motorAPasos.step(512); //Pasos = (Grados*2048)/360 = (90*2048)/360 = 512
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(2000); //Detiene el código 2 segundos después de dar una vuelta completa.
  /*Stepper.step(): Método que permite indicar la cantidad de pasos que queremos que ejecute 
  el motor, indicando con un signo su dirección.*/
  motorAPasos.step(-341.3333); //Pasos = (Grados*2048)/360 = (-60*2048)/360 = 512
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(2000); //Detiene el código 2 segundos después de dar una vuelta completa.
}
