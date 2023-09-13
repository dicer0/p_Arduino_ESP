/*22.1.-Encoder Rotativo: Este es un dispositivo digital que físicamente se parece a un 
potenciómetro, con el cual se pueden controlar otros dispositivos electrónicos o eléctricos, 
cuenta con 3 pines para efectuar dicho control: Pin A (CLK), Pin B (DT) y Pin C (SW).
Los pines A y B entregan dos señales digitales con las cuales se puede identificar el sentido 
de giro del encoder, que consta de 30 pasos para dar una vuelta de 360°, cabe mencionar que el giro 
del encoder no tiene tope, solo se basa en su última posición para saber dónde se encuentra:
  - Giro en sentido horario (derecha o CW) del encoder: Ocurre un flanco de bajada en el pin A antes 
    que en el B.          (PinA,PinB) = (1,1)→(0,1)→(0,0)→(1,0)
  - Giro en sentido antihorario (izquierda o CCW) del encoder: Ocurre un flanco de bajada en el pin B 
    antes que en el A.    (PinA,PinB) = (1,1)→(1,0)→(0,0)→(0,1)
Y el tercer Pin C es un pulsador, que al ser presionado se conecta a GND, por lo que su valor se 
vuelve en 0 lógico.
Se elige que la interrupción se ejecute cuando la señal del PinA esté en modo LOW (0 lógico), porque 
al checar el nivel digital del PinB, si este tiene un valor de 1 (2da posición del giro CW = (0,1)), 
el giro será hacia la derecha y si tiene un valor de 0 (3era posición del giro CCW = (0,0)), el giro 
será hacia la izquierda.*/
//DECLARACIÓN DE VARIABLES Y CONSTANTES:
/*#define: Instrucción utilizada para crear con una directiva una constante global en el programa de 
Arduino siguiendo la sintaxis descrita a continuación:
  #define nombreConstante valorConstante*/
#define PinA 2 //El PinA se conectan a un pin que active interrupciones externas en el Arduino.
#define PinB 4 //El PinB se puede conectar a un pin digital cualquiera del Arduino.
#define PinC 7 //Pin C conectado al pin digital 3 del Arduino.
/*volatile: Modificador de acceso de alcance global, cuya función es indicar que una variable puede 
cambiar su valor en cualquier momento, incluso si no se modifica explícitamente en el código. Se utiliza 
en programas de sistemas embebidos, donde las variables pueden ser modificadas por hardware externo como 
interrupciones o procesos en paralelo.*/
int ANTERIOR = 50; //En esta variable se almacena el último valor leído de la variable posición.
volatile int POSICION = 50; //Cuando se ejecute la interrupción, se guardará la posición actual.
int POSICION_MAXIMA = 100;  //El encoder podrá girar de 0 a 100, empezando en el valor 50.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL:
void setup() {
  /*pinMode(): Método que indica cuales pines del Arduino son entradas y cuales son salidas:
      - primer parámetro: Indica el pin de Arduino que será asignado como salida o entrada.
      - segundo parámetro: Usa la instrucción OUTPUT para indicar que el pin es una salida o 
        INPUT para indicar que el pin es una entrada.
  El número del pin que recibe este método como primer parámetro se puede declarar directamente
  como un número o se puede declarar al inicio del programa como una variable.*/
  pinMode(PinA, INPUT); //La constante pinA asignada al pin de interrupción 2 es una entrada.
  pinMode(PinB, INPUT); //La constante pinB asignada al pin digital 4 es una entrada.
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
  Serial.begin(9600);   //Comunicación serial de 9600 baudios, para recibir los pasos del encoder.
  /*Serial.println(): Método que escribe un mensaje en la consola serial de Arduino, a la cual se puede
  acceder en la esquina superior derecha donde se encuentra una lupa.*/
  Serial.println("Control Encoder");
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(20);
  /*attachInterrupt(): Método que se define en el setup del código y sirve para indicar el índice
  o pin a donde está conectada la interrupción, la función que se ejecutará cuando la interrupción
  sea detectada y el momento del pulso que acciona la interrupción (flanco de subida, bajada, etc.):
      - primer parámetro: Indica el índice de la interrupción, que no es lo mismo al pin de Arduino 
        que será asignado para recibir la interrupción, ya que tienen números distintos, donde por 
        ejemplo el pin 2 del Arduino corresponde al índice 0 de la interrupción y el pin 3 al 1, para
        ello se utiliza el siguiente método:
            - digitalPinToInterrupt(pin): Este método no importando el tipo de placa que se esté 
              usando transforma el número del pin digital en su equivalente índice de interrupción.
      - segundo parámetro: Indica el nombre de la función ISR (Interrupt Service Routine) que se manda 
        a llamar cuando la interrupción sea detectada, indicando la acción que se ejecuta.
      - tercer parámetro: Indica el momento del pulso que activa la interrupción, ya sea:
          - LOW: La interrupción es activada en el nivel bajo del pulso (antes o después de los flancos).
          - RISING: La interrupción es activada en el flanco de subida del pulso.
          - CHANGE: La interrupción es activada cuando ocurra un flanco de bajada o subida en el pulso,
            osea cuando haya un cambio de nivel.
          - FALLING: La interrupción es activada en el flanco de bajada del pulso.
          - HIGH: La interrupción es activada en el nivel alto del pulso (después del flanco de subida).
  Se elige que la interrupción se ejecute cuando la señal del PinA esté en modo LOW (0 lógico), porque 
  al checar el nivel digital del PinB, si este tiene un valor de 1 lógico (Giro CW = (PinA,PinB) = (0,1)), 
  el giro será hacia la derecha y si tiene un valor de 0 lógico (Giro CCW = (PinA,PinB) = (0,0)), el giro 
  será hacia la izquierda.
  Si se usara el PinB para ejecutar la interrupción en vez del PinA, se haría lo mismo, utilizando el modo 
  de interrupción LOW, pero detectando el giro CW cuando en PinA haya un 1 y CCW cuando haya un 0.*/
  attachInterrupt(digitalPinToInterrupt(2), POS_ENCODER, LOW);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Interrupciones que identifican el sentido de giro 
//del encoder y si su pulsador ha sido presionado.
void loop() {
  /*Serial.println(): Método que escribe un mensaje en la consola serial de Arduino, a la cual se puede
  acceder en la esquina superior derecha donde se encuentra una lupa. Si queremos concatenar esto con 
  un valor se debe usar el método String() para convertir su contenido en una cadena de caracteres.*/
  if(POSICION != ANTERIOR){
    Serial.println("Posición = " + String(POSICION));
    ANTERIOR = POSICION;
  }
}

//FUNCIÓN ISR QUE SE EJECUTA CUANDO SE ACTIVA UNA INTERRUPCIÓN:
/*Se elige que la interrupción se ejecute cuando la señal del PinA esté en modo LOW, porque en ese caso 
se checará el nivel digital del PinB y si este tiene un valor de 1, el giro será hacia la derecha (CW), 
por lo que se aumentará su posición y si tiene un valor de 0, el giro será hacia la izquierda (CCW), por 
lo que se reducirá su posición. 
Además, se añadirá un código de antirrebote, ya que como los pasos se realizan de forma mecánica, 
conectando los pines A y B con la base dentada de la placa C, se debe dejar pasar cierto tiempo entre 
muestreos para no captar lecturas falsas.*/
void POS_ENCODER(){
  //ANTIRREBOTES CON DELAY DE 5 MILISEGUNDOS: Para ello se debe medir el tiempo entre interrupciones.
  /*static: Modificador de acceso de alcance local y duración de almacenamiento estática, cuya función es 
  que una variable conserve su valor incluso después de que la función donde fue declarada haya terminado 
  su ejecución, esto se usa en interrupciones porque cuando este tipo de funciones dejan de correr, se 
  borran los valores de todas sus variables.
  unsigned long: Variable sin signo de 32 bits, puede abarcar números muy grandes de 0 a 4,294,967,295.*/
  static unsigned long ultimaInterrupcion = 0;
  /*millis(): Método que devuelve el tiempo transcurrido en milisegundos, empezando a contar desde que se 
  enciende la placa Arduino y no deteniéndose hasta que esta se apague o llegue a su límite, que es el 
  mismo dado por el tipo de dato unsigned long: De 0 a 4,294,967,295 milisegundos = 1,193.0464 horas
  49.7102 días = 49 días y 17 horas.
  La función se puede utilizar para detener el programa, sustituyendo al método delay(), la razón de ello 
  es que el método delay() detiene completamente la ejecución del programa, mientras que el método millis() 
  permite que otros procesos se ejecuten mientras solo una parte del código se pausa.*/
  unsigned long tiempoInterrupcion = millis();

  if(tiempoInterrupcion - ultimaInterrupcion > 5){
    //LECTURA DE LOS PINES DE SEÑAL A Y B:
    /*digitalRead(Pin): Lo que hace este método es leer una entrada digital en un pin en específico que 
    se indica como su primer parámetro, se puede ver si este valor está en nivel alto con la constante 
    HIGH o si está en su nivel bajo con la constante LOW.*/
    if(digitalRead(PinB) == HIGH){ //Giro CW
      POSICION++;
    }
    else{                         //Giro CCW
      POSICION--;
    }
    /*Actualización del tiempo guardado en la variable ultimaInterrupcion para así ejecutar de nuevo el 
    código antirrebotes la próxima vez que sea activada la interrupción.*/
    ultimaInterrupcion = tiempoInterrupcion;
    /*min(num1, num2): Método que compara dos valores num1 y num2, devolviendo el más pequeño (mínimo).*/
    /*max(num1, num2): Método que compara dos valores num1 y num2, devolviendo el más grande (máximo).*/
    /*Como el encoder rotativo no tiene un límite mecánico por ninguno de sus dos lados, por medio del 
    código se fija dicho límite usando los métodos min() y max():
    min(limMax, pos): Se utiliza para indicar la posición máxima que puede alcanzar el encoder.
    max(limMin, pos): Se utiliza para indicar la posición mínima que puede alcanzar el encoder.*/
    POSICION = min(POSICION_MAXIMA, max(0, POSICION)); //limMin = max(limMin, pos) = 0; limMax = min(limMax, pos) = 100.
  }
}
