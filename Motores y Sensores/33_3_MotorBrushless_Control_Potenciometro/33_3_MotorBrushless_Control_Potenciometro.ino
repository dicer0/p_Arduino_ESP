/*33.3.-Control de velocidad de un motor brushless (BLDC) con un potenciómetro y calibrando el
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
#include <Servo.h> //Librería de control de servomotores.

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
int PINPOT = A0;          //Pin A0 = Cable de la terminal central del pot (patita de en medio).

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.
    - El segundo parámetro indica la duración mínima en µs de la señal PWM y el tercero la máxima.*/
  motorBLDC.attach(PINPWM_BLDC, DUTYCYCLEMIN, DUTYCYCLEMAX);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*analogRead(): Método que lee un valor analógico por medio del ADC del Arduino que consta de 10
  bits, por lo que convertirá los valores de tensión de 0 a 5V que reciba a un equivalente de 0 a 
  2^10 - 1 = 1023.*/
  int LECTURAPOT = analogRead(PINPOT);
  /*map(): Método que realiza una regla de 3, convirtiendo un número de un rango a otro que sea 
  equivalente al primero, en sus parámetros recibe lo siguiente:
    - Primer parámetro: Recibe la variable de donde proviene el número a convertir.
    - Segundo parámetro: Recibe el valor mínimo del primer rango de valores.
    - Tercer parámetro: Recibe el valor máximo del primer rango de valores.
    - Cuarto parámetro: Recibe el valor mínimo del segundo rango de valores, al que queremos llegar.
    - Quinto parámetro: Recibe el valor máximo del segundo rango de valores, al que queremos llegar.
  De esta manera se convierten los valores de de 0 a 1023 entregados por el método analogRead() a 
  valores de ángulos de rotación de 20 a 180 que varíe la velocidad del BLDC de 0 a KV * V_ESC [rpm].*/
  int VELOCIDAD = map(LECTURAPOT, 0, 1023, 20, 180);  //Velocidad mínima = 20; Velocidad máxima = 180.
  /*Servo.write(0): Método usado para controlar la posición del eje perteneciente a un servomotor. 
  Recibe como argumento un valor numérico que representa el ángulo de giro deseado.*/
  motorBLDC.write(VELOCIDAD);
}
