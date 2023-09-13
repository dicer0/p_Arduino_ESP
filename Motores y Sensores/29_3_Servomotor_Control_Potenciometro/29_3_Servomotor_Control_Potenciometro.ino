/*29.3.-Control de movimiento de un servomotor por medio de un potenciómetro, editando además el 
duty cycle mínimo y máximo que se alcanza en los motores.
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
int PINPWMSERVO = 2;     //Pin 2 = Cable de señal PWM conectada al PIN2 digital del Arduino.
/*Cuando el valor alto del duty cycle en la señal PWM dura 1000 µs = 1 ms, la posición del motor 
está en 0°. Esta es la teoría, pero si el motor no da el giro completo o suena un clic cuando lo 
hace, se puede mover este valor para encontrar el óptimo, además al editar esto se puede cambiar 
aunque sea un poco el alcance de giro de los motores.*/
int DUTYCYCLEMIN = 600;  //High Duty Cycle Mínimo ≈ 1000 µs = 1 ms
/*Cuando el valor alto del duty cycle en la señal PWM dura 2000 µs = 2 ms, la posición del motor 
está en 180°. Esta es la teoría, pero si el motor no da el giro completo o suena un clic cuando lo 
hace, se puede mover este valor para encontrar el óptimo, además al editar esto se puede cambiar 
aunque sea un poco el alcance de giro de los motores.*/
int DUTYCYCLEMAX = 2500; //High Duty Cycle Máximo ≈ 2000 µs = 2 ms
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
int PINPOT = A0;         //Pin A0 = Cable de la terminal central del pot (patita de en medio).

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.
    - El segundo parámetro indica la duración mínima en µs de la señal PWM y el tercero la máxima.*/
  servomotor.attach(PINPWMSERVO, DUTYCYCLEMIN, DUTYCYCLEMAX);
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
  valores de ángulos de rotación de 0 a 180° que comprenda el servomotor.*/
  int ANGULO = map(LECTURAPOT, 0, 1023, 0, 180);
  /*Servo.write(0): Método usado para controlar la posición del eje perteneciente a un servomotor. 
  Recibe como argumento un valor numérico que representa el ángulo de giro deseado.*/
  servomotor.write(ANGULO);
}
