/*32.2.-Motor a Pasos Bipolar tipo NEMA 17 con controlador A4988, este cuenta con 2 bobinas 
A y B, su gran ventaja en comparación con el motor a pasos unipolar es que este aguanta mucho 
más carga, soportando hasta 4kg de peso y tiene una mayor velocidad de rotación.
En este programa por medio de los pines MS1, MS2 y MS3 se elige el tipo de paso que se quiere 
utilizar, pero si no es el paso completo, se reduce la carga que puede resistir el motor 1/2, 
1/4, 1/8 y 1/16 veces proporcionalmente.*/
/*#define: Instruccion utilizada para crear constantes globales en el programa de Arduino 
siguiendo la sintaxis descrita a continuación:
  #define nombreConstante valorConstante*/
#define MS1 7
#define MS2 6
#define MS3 5
#define STEP 4
#define DIR 3
/*El motor a pasos recibe una frecuencia mínima de 2 Hz y máxima de 500 Hz, la variable demora
indicará esa frecuencia, pero en milisegundos del periodo, que es el inverso de la frecuencia:
  T = 1/f = 1/2 = 0.5 segundos = 500 milisegundos; El movimiento más lento.
  T = 1/f = 1/500 = 0.002 = 2 milisegundos; El movimiento más rápido.*/
int demora = 1; //Cambiando esta frecuencia se cambia la velocidad de rotación del motor.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*En esta parte del código Arduino se indican los puertos de salida, de entrada y la velocidad 
  de la comunicación serial*/
  /*pinMode(): Método que indica cuales pines del Arduino son entradas y cuales son salidas:
      - primer parámetro: Indica el pin de Arduino que será asignado como salida o entrada.
      - segundo parámetro: Usa la instrucción OUTPUT para indicar que el pin es una salida o 
        INPUT para indicar que el pin es una entrada.
  El número del pin que recibe este método como primer parámetro se puede declarar directamente
  como un número o se puede declarar al inicio del programa como una variable.*/
  pinMode(STEP, OUTPUT);  //La constante STEP asignada al pin digital 4 es una salida.
  pinMode(DIR, OUTPUT);   //La constante DIR asignada al pin digital 5 es una salida.
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*Con los 3 pines MS1, MS2 Y MS3 de Micro Stepping se establece el tipo de paso del motor para lograr 
  una mayor precisión o torque, se elige una de las siguientes opciones dependiendo de para qué se esté 
  utilizando el motor:
                    MS1   MS2   MS3
    Paso Completo =  0     0     0
    Paso Medio    =  1     0     0
    1/4 de Paso   =  0     1     0
    1/8 de Paso   =  1     1     0
    1/16 de Paso  =  1     1     1
  Mientras más se reduzca el paso, mayor es la resolución de la precisión en el movimiento, pero el torque 
  se reduce proporcionalmente:
    # Pasos 1 vuelta paso completo = 200 [pasos/1 revolucion]
    # Pasos 1 vuelta = # Pasos 1 vuelta paso completo * 1/(división de paso)
    # Pasos 1 vuelta 1/4 de paso = 200 * 1/(1/4) = 200 * 4 = 800 [pasos/rev].*/
  //Paso Completo: Necesita dar 200 pasos para que el eje del motor dé 1 vuelta completa.
  digitalWrite(MS1, LOW);   //MS1 = 0.
  digitalWrite(MS2, HIGH);  //MS2 = 1.
  digitalWrite(MS3, LOW);   //MS3 = 0; 1/4 de paso.

  /*digitalWrite(Pin, State): Lo que hace este método es mandar una salida digital a un pin en 
  específico que se indica como su primer parámetro, en su segundo parámetro se puede mandar la 
  constante HIGH para mandar 5V al pin o LOW para mandar 0V, osea no mandar nada.*/
  digitalWrite(DIR, HIGH); //DIR = 1 lógico: Giro en sentido de las manecillas del reloj.
  /*El motor a pasos NEMA 17 puede constar de 200 o 400 pasos:
    - Si consta de 200 pasos, tiene una rotación de 1.8° por paso.
    - Si consta de 400 pasos, tiene una rotación de 0.9° por paso.*/
  for(int i = 0; i < 800; i++){ //Giro de 360°, avanzando a 1.8° por paso.
    digitalWrite(STEP, HIGH);
    /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
    delay(demora);
    digitalWrite(STEP, LOW);
    delay(demora);
  }
  digitalWrite(DIR, LOW); //DIR = 0 lógico: Giro en sentido contrario de las manecillas del reloj.
  /*El motor a pasos NEMA 17 puede constar de 200 o 400 pasos:
    - Si consta de 200 pasos, tiene una rotación de 1.8° por paso.
    - Si consta de 400 pasos, tiene una rotación de 0.9° por paso.*/
  for(int i = 0; i < 800; i++){ //Giro de 180°, avanzando a 1.8° por paso.
    digitalWrite(STEP, HIGH);
    /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
    delay(demora);
    digitalWrite(STEP, LOW);
    delay(demora);
  }

  /*Al finalizar de dar una vuelta de un lado y otra vuelta hacia el otro lado, el programa se detiene 
  5 segundos antes de dar otra vez las dos vueltas.*/
  delay(1000);            //Detiene el código 5 segundos antes de dar otra vuelta al motor.
}
