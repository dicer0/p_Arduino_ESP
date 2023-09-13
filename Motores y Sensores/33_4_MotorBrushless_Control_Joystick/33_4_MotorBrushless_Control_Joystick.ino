/*33.4.-Control de velocidad de un motor brushless (BLDC) con un potenciómetro y calibrando el
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
La cual es controlada por una señal PWM (Pulse Width Module) con una frecuencia de 50 Hz (20 [ms]).
Acerca del Joystick cabe mencionar que cuando este está conectado al protoboard, el sentido Y es el 
horizontal y el vertical es el X.*/
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
int DUTYCYCLEMAX = 3000;  //Calibración BLDC: High Duty Cycle Máximo ≈ 3000 µs = 3 ms.
#define VRY A0            //Pin VRY que da las coordenadas verticales Y del Joystick.
int Y;                    //Coordenada Y que adopta valores de 0 a 1023 por el ADC de 10 bits.
int CoordenadaY;          //Coordenada Y después de la transformación de 0 a 1023 -> 0 a 5V.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL
void setup() {
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.
    - El segundo parámetro indica la duración mínima en µs de la señal PWM y el tercero la máxima.*/
  motorBLDC.attach(PINPWM_BLDC, DUTYCYCLEMIN, DUTYCYCLEMAX);
  /*pinMode(): Método que indica cuales pines del Arduino son entradas y cuales son salidas:
      - primer parámetro: Indica el pin de Arduino que será asignado como salida o entrada.
      - segundo parámetro: Usa la instrucción OUTPUT para indicar que el pin es una salida o 
        INPUT para indicar que el pin es una entrada.
  El número del pin que recibe este método como primer parámetro se puede declarar directamente
  como un número o se puede declarar al inicio del programa como una variable.*/
  pinMode(VRY, INPUT);  //Constante VRX asignada al pin A1.
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
  Serial.println("Control Joystick de Motor Brushless");
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*analogRead(): Método que lee un valor analógico por medio del ADC del Arduino que consta de 10
  bits, por lo que convertirá los valores de tensión de 0 a 5V que reciba a un equivalente de 0 a 
  2^10 - 1 = 1023.*/
  Y = analogRead(VRY);
  /*En el Joystick se considera una zona de error de ±20 más o menos, por eso en vez de considerar 
  rangos de 0 a 511.5 y de 511.5 a 1023, se considera de 0 a 480 y de 520 a 1023.*/
  if(Y >= 0 && Y < 480){            //Obtención de coordenadas -Y: Velocidad de 20 a 20+(180-20)/2 = 100.
    CoordenadaY = map(Y, 0, 480, 20, 100);
  }else if(Y >= 520 && Y <= 1023){  //Obtención de coordenadas +Y: Velocidad de 20+(180-20)/2 = 100 a 180.
    CoordenadaY = map(Y, 520, 1023, 100, 180);
  }else{
    CoordenadaY = 80;
  }
  /*Serial.println(): Método que escribe un mensaje en la consola serial de Arduino, a la cual se puede
  acceder en la esquina superior derecha donde se encuentra una lupa. Si queremos concatenar esto con 
  un valor se debe usar el método String() para convertir su contenido en una cadena de caracteres.*/
  Serial.println("Velocidad Joystick = " + String(CoordenadaY) + " rpm");
  /*Servo.write(): Método usado para controlar la posición del eje perteneciente a un servomotor. 
  Recibe como argumento un valor numérico que representa el ángulo de giro deseado.*/
  motorBLDC.write(CoordenadaY);   //Velocidad mínima = 20; Velocidad máxima = 180.
}
