/*33.2.-Control de velocidad de un motor brushless (BLDC) por medio de código, pero calibrando el
porcentaje del duty cycle de la señal PWM para adaptarla al motor sin escobillas: Para la conexión 
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
#include <Servo.h> //Librería de control de servomotores y motores brushless.

//DECLARACIÓN DE VARIABLES Y/O CONSTANTES
/*Objeto de la clase Servo, este no recibe ningún parámetro, solamente sirve para poder usar los 
métodos de la librería.*/
Servo motorBLDC; 
int PINPWM_BLDC = 3;      //Pin 3 = Cable de señal PWM conectada al PIN3~ digital del Arduino.
/*Cuando el valor bajo del duty cycle en la señal PWM dura 1000 µs = 1 ms, la velocidad del BLDC
está en 0 [rpm]. Esta es la teoría, pero si el motor no gira, este valor se debe calibrar para 
encontrar el óptimo, además al editar esto se puede cambiar el alcance de la velocidad de giro 
del motor.*/
int DUTYCYCLEMIN = 0;     //Calibración BLDC: High Duty Cycle Mínimo ≈ 0 µs = 0 s
/*Cuando el valor alto del duty cycle en la señal PWM dura 2000 µs = 2 ms, la velocidad del BLDC
está en KV * V_ESC [rpm]. Esta es la teoría, pero si el motor deja de girar, este valor se debe 
calibrar para encontrar el óptimo, además al editar esto se puede cambiar el alcance de la 
velocidad de giro del motor.*/
int DUTYCYCLEMAX = 3000;  //Calibración BLDC: High Duty Cycle Máximo ≈ 3000 µs = 3 ms
int velocidadMotor = 900; //Este delay de 900 ms solamente hace que la velocidad varíe.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.
    - El segundo parámetro indica la duración mínima en µs de la señal PWM y el tercero la máxima.*/
  motorBLDC.attach(PINPWM_BLDC, DUTYCYCLEMIN, DUTYCYCLEMAX);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*servomotor.write(0): Método usado para controlar la velocidad de giro del eje perteneciente a 
  un motor sin escobillas. Recibe como argumento un número que representa la velocidad deseada.*/
  motorBLDC.write(20);         //Velocidad mínima = 20; Velocidad máxima = 180.
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(velocidadMotor);
  motorBLDC.write(180);
  delay(velocidadMotor);
}
