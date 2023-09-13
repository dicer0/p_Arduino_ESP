/*29.2.-Control de movimiento de un servomotor con solamente código, editando el duty cycle mínimo
y máximo que se alcanza en los motores.
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
int PINPWMSERVO = 11;      //Pin 11 = Cable de señal PWM conectada al PIN11~ digital del Arduino.
/*Cuando el valor alto del duty cycle en la señal PWM dura 1000 µs = 1 ms, la posición del motor 
está en 0°. Esta es la teoría, pero si el motor no da el giro completo o suena un clic cuando lo 
hace, se puede mover este valor para encontrar el óptimo, además al editar esto se puede cambiar 
aunque sea un poco el alcance de giro de los motores.*/
int DUTYCYCLEMIN = 600;  //High Duty Cycle Mínimo ≈ 1000 µs = 1 ms
/*Cuando el valor alto del duty cycle en la señal PWM dura 2000 µs = 2 ms, la posición del motor 
está en 180°. Esta es la teoría, pero si el motor no da el giro completo o suena un clic cuando lo 
hace, se puede mover este valor para encontrar el óptimo, además al editar esto se puede cambiar 
aunque sea un poco el alcance de giro de los motores.*/
int DUTYCYCLEMAX = 2500;  //High Duty Cycle Máximo ≈ 2000 µs = 2 ms
//VALORES ÓPTIMOS DE DUTY CYCLE:
//Servomotor SG90 (El chiquito azul); DUTYCYCLEMIN = 600; DUTYCYCLEMAX = 2500; Giro de 0 a 180°.
//Servomotor MG995 (El grande negro); DUTYCYCLEMIN = 650; DUTYCYCLEMAX = 2500; Giro de 0 a 160°.
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
int velocidadMotor = 900;

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.
    - El segundo parámetro indica la duración mínima en µs de la señal PWM y el tercero la máxima.*/
  servomotor.attach(PINPWMSERVO, DUTYCYCLEMIN, DUTYCYCLEMAX);
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
//26,000 = 600
//X      = 0;     X = 
