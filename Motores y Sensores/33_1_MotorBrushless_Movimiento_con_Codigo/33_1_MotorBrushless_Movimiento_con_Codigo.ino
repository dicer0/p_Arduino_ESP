/*33.1.-Control de velocidad de un motor brushless (BLDC) por medio de código: Para la conexión 
de este motor se usa un controlador externo llamado ESC (Electronic Speed Controller), el cual es 
alimentado por una batería LiPo, misma que alimenta al Arduino a través del cable rojo del ESC por 
medio de su pin de 5V, a la tierra del ESC igual se conectará el pin GND del Arduino y a través de 
uno de los pines digitales del Arduino se controlará su velocidad de rotación, a través de una señal 
PWM con frecuencia de 50 Hz, que varíe su duty cycle de 1 a 2ms. El control del BLDC es idéntico al 
del servomotor, pero la única diferencia es que esa señal PWM controla la posición de 0 a 180 y la 
de este tipo de motor controla la velocidad, pero su funcionamiento y librería de uso es la misma.

Motor brushless MT2204 que se alimenta con baterías LiPo de 2 a 3S, cuenta con una KV = 2300, por lo 
que el rango de velocidades que abarca como máximo, alimentándolo con una batería de 3S es de:
0 a KV * V_ESC [rpm] = 2300 * 3.7 * 3S = 25,530 [rpm].
La cual es controlada por una señal PWM (Pulse Width Module) con una frecuencia de 50 Hz (20 [ms]).
------------------------------------------------------------------------------------------------
Motor brushless A2212 que se alimenta con baterías LiPo de 2 a 3S, cuenta con una KV = 1400, por lo 
que el rango de velocidades que abarca como máximo, alimentándolo con una batería de 3S es de:
0 a KV * V_ESC [rpm] = 1400 * 3.7 * 3S = 15,540 [rpm].
La cual es controlada por una señal PWM (Pulse Width Module) con una frecuencia de 50 Hz (20 [ms]).*/
//IMPORTACIÓN DE LIBRERÍAS
#include <Servo.h> //Librería de control de servomotores.

//DECLARACIÓN DE VARIABLES Y/O CONSTANTES
/*Objeto de la clase Servo, este no recibe ningún parámetro, solamente sirve para poder usar los 
métodos de la librería.*/
Servo motorBLDC; 
int PINPWMSERVO = 3;        //Pin 2 = Cable de señal PWM conectada al PIN2 digital del Arduino.
int velocidadMotor = 5000;  //Este delay de 5,000 ms = 5s solamente hace que la velocidad varíe.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.*/
  motorBLDC.attach(PINPWMSERVO);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*servomotor.write(0): Método usado para controlar la velocidad de giro del eje perteneciente a 
  un motor sin escobillas. Recibe como argumento un número que representa la velocidad deseada.*/
  motorBLDC.write(60);     //Valor mínimo = 60; Valor máximo = 150.
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(velocidadMotor);
  motorBLDC.write(150);
  delay(velocidadMotor);
}
