/*30.1.1.-Motor a Pasos Unipolar modelo 28BYJ-48 con controlador ULN2003, este cuenta con 4 bobinas 
A, B, C y D y una terminal común que se conecta a 5V. En este código se le aplica el Paso Simple.*/
//1_Paso simple: Solo se activa una fase a la vez en el motor para accionar su rotación, dando 
//como resultado una precisión de 2,048 pasos y un torque mediocre que soporta 63 gramos.
int IN1 = 8;  //Pin IN1 = Bobina A del motor a pasos conectado al PIN8 digital del Arduino.
int IN2 = 9;  //Pin IN2 = Bobina B del motor a pasos conectado al PIN9 digital del Arduino.
int IN3 = 10; //Pin IN2 = Bobina C del motor a pasos conectado al PIN10 digital del Arduino.
int IN4 = 11; //Pin IN2 = Bobina D del motor a pasos conectado al PIN11 digital del Arduino.
/*El motor a pasos recibe una frecuencia mínima de 2 Hz y máxima de 500 Hz, la variable demora
indicará esa frecuencia, pero en milisegundos del periodo, que es el inverso de la frecuencia:
  T = 1/f = 1/2 = 0.5 segundos = 500 milisegundos ; El movimiento más lento.
  T = 1/f = 1/500 = 0.002 = 2 milisegundos; El movimiento más rápido.*/
int demora = 20; //Cambiando esta frecuencia se cambia la velocidad de rotación del motor.

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
  pinMode(IN1, OUTPUT); //El pin 8  es una salida digital.
  pinMode(IN2, OUTPUT); //El pin 9  es una salida digital.
  pinMode(IN3, OUTPUT); //El pin 10 es una salida digital.
  pinMode(IN4, OUTPUT); //El pin 11 es una salida digital.
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*El motor a pasos cuenta con un sistema de engranes que reducen su velocidad y aumentan su 
  torque, por lo que el motor debe dar un total de 2,048 pasos para dar una vuelta completa, 
  esto solo se aplica en el paso simple y doble debido a que: 
  - 64 vueltas completas del rotor original corresponden a 1 vuelta del eje del motor, dando un 
    giro de 5.625° por paso al eje final, pero para que el rotor original gire una vuelta se 
    necesitan 32 pasos, dando un giro de 11.25° por paso al eje original, ya que a fuerza se 
    deben completar 8 ciclos de los pasos para que el rotor original dé 1 vuelta.
  Esta situación aunada a que:
    - Un ciclo del paso simple consta de 4 pasos, da como resultado que un giro completo del rotor 
    se conforme de 32/4 = 8 ciclos.
  En conclusión, el motor necesita dar: 
    #pasosSecuencia*#Ciclos*#reduccionRotorFinal = 32*64 = 4*8*64 = 2048 pasos para dar una vuelta.
  Esto en código se transfiere a dar #Ciclos*#reduccionRotorFinal = 8*64 = 512 ciclos, porque la 
  secuencia del paso simple consta de 4 pasos, que se indican dentro del mismo bucle for.*/
  for(int i = 0; i < 512; i++){
    /*digitalWrite(Pin, State): Lo que hace este método es mandar una salida digital a un pin en 
    específico que se indica como su primer parámetro, en su segundo parámetro se puede mandar la 
    constante HIGH para mandar 5V al pin o LOW para mandar 0V, osea no mandar nada.*/
    digitalWrite(IN1, HIGH);  //Con esta línea de código se manda un 1 lógico a la bobina A.
    digitalWrite(IN2, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina B.
    digitalWrite(IN3, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina C.
    digitalWrite(IN4, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina D.
    /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
    delay(demora);            //Detiene T = 1/100 = 20 ms el código antes de volver a ejecutarse.

    /*digitalWrite(Pin, State): Lo que hace este método es mandar una salida digital a un pin en 
    específico que se indica como su primer parámetro, en su segundo parámetro se puede mandar la 
    constante HIGH para mandar 5V al pin o LOW para mandar 0V, osea no mandar nada.*/
    digitalWrite(IN1, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina A.
    digitalWrite(IN2, HIGH);  //Con esta línea de código se manda un 1 lógico a la bobina B.
    digitalWrite(IN3, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina C.
    digitalWrite(IN4, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina D.
    /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
    delay(demora);            //Detiene T = 1/100 = 20 ms el código antes de volver a ejecutarse.

    /*digitalWrite(Pin, State): Lo que hace este método es mandar una salida digital a un pin en 
    específico que se indica como su primer parámetro, en su segundo parámetro se puede mandar la 
    constante HIGH para mandar 5V al pin o LOW para mandar 0V, osea no mandar nada.*/
    digitalWrite(IN1, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina A.
    digitalWrite(IN2, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina B.
    digitalWrite(IN3, HIGH);  //Con esta línea de código se manda un 1 lógico a la bobina C.
    digitalWrite(IN4, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina D.
    /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
    delay(demora);            //Detiene T = 1/100 = 20 ms el código antes de volver a ejecutarse.

    /*digitalWrite(Pin, State): Lo que hace este método es mandar una salida digital a un pin en 
    específico que se indica como su primer parámetro, en su segundo parámetro se puede mandar la 
    constante HIGH para mandar 5V al pin o LOW para mandar 0V, osea no mandar nada.*/
    digitalWrite(IN1, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina A.
    digitalWrite(IN2, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina B.
    digitalWrite(IN3, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina C.
    digitalWrite(IN4, HIGH);  //Con esta línea de código se manda un 1 lógico a la bobina D.
    /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
    delay(demora);            //Detiene T = 1/100 = 20 ms el código antes de volver a ejecutarse.
  }

  //Al finalizar de dar una vuelta, el programa se detiene 5 segundos antes de dar otra.
  digitalWrite(IN1, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina A.
  digitalWrite(IN2, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina B.
  digitalWrite(IN3, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina C.
  digitalWrite(IN4, LOW);   //Con esta línea de código se manda un 0 lógico a la bobina D.
  delay(5000);              //Detiene el código 5 segundos antes de dar otra vuelta al motor.
}
