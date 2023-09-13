/*29.1.-Control de movimiento de un servomotor con solamente código.
Servomotor SG90 que se alimenta con 4.8 a 6 V, cuenta con un pequeño torque de 1.2 [kg.cm] 
cuando se alimenta con 4.8 V y 1.8 [kg.cm] cuando se alimenta con 6 V, su posición es controlada 
por una señal PWM (Pulse Width Module) con una frecuencia de 50 Hz (20 [ms]).
------------------------------------------------------------------------------------------------
Servomotor MG995 que se alimenta con 4.8 a 7.5 V, cuenta con un torque medio de 8.5 [kg.cm] cuando 
se alimenta con 4.8 V y 10 [kg.cm] cuando se alimenta con 7.5 V, su posición es controlada por una 
señal PWM (Pulse Width Module) con una frecuencia de 50 Hz (20 [ms]).*/
//IMPORTACIÓN DE LIBRERÍAS
#include <Servo.h> //Librería de control de servomotores.

//DECLARACIÓN DE VARIABLES Y/O CONSTANTES
/*Objeto de la clase Servo, este no recibe ningún parámetro, solamente sirve para poder usar los 
métodos de la librería.*/
Servo servomotor; 
int PINPWMSERVO = 2; //Pin 2 = Cable de señal PWM conectada al PIN2 digital del Arduino.
/*El motor utiliza corriente cuando se mueve, aumenta mucho la corriente que consume cuando se 
mueve rápido y consume muy poca cuando mantiene su posición, hay que tomar mucho en cuenta esto 
cuando se prueba el motor, ya que, si se usa un valor bajo en el delay, la corriente que demanda el 
servomotor puede dañar el puerto serial de la computadora. 
--------------------------------------------------------------------------------------------------
Servomotor SG90 (El chiquito azul): Si el delay de la velocidad baja de ser 350 milisegundos, el 
servomotor no es que demande más corriente y el puerto serial comienza a fallar, sino que no le da 
tiempo de llegar al otro punto de la secuencia, más que nada si queremos que vaya de 0 a 180°, si 
es un gir que vaya a una posición menor, tal vez si se podría, pero al transcurrir mucho tiempo 
este se comenzará a calentar.
--------------------------------------------------------------------------------------------------
Servomotor MG995 (El grande negro): Si el delay de la velocidad baja de ser 860 milisegundos, el 
servomotor demanda mucha corriente si se conecta directamente y el puerto serial comienza a fallar.*/
//VALORES ÓPTIMOS DE DUTY CYCLE:
//Servomotor SG90 (El chiquito azul); velocidadMotorMax = 350 ms; f = 2.8 Hz.
//Servomotor MG995 (El grande negro); velocidadMotorMax = 860 ms; f = 1.16 Hz.
int velocidadMotor = 900;

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.*/
  servomotor.attach(PINPWMSERVO);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*servomotor.write(0): Método usado para controlar la posición del eje perteneciente a un servomotor. 
  Recibe como argumento un valor numérico que representa el ángulo de giro deseado.*/
  servomotor.write(0);
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos, 
  pero cuando se usa después del método Servo.write(), lo que hace es dictar el tiempo que se debe 
  tardar de llegar de un punto a otro en una secuencia.*/
  delay(velocidadMotor);
  servomotor.write(180);
  delay(velocidadMotor);
}
