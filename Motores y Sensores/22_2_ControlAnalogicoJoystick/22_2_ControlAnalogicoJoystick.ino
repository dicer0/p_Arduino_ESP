/*22.2.-Joystick: Este es un dispositivo analógico con el cual se pueden controlar otros dispositivos 
electrónicos o eléctricos, cuenta con 3 pines para efectuar dicho control: VRX, BRY y SW.
Los pines VRX y VRY entregan dos señales analógicas que cambian su valor dependiendo del punto XY donde 
se encuentre su eje, tan sencillo como eso.*/
//DECLARACIÓN DE VARIABLES Y CONSTANTES:
#define VRX A1                //Pin VRX que da las coordenadas horizontales X del Joystick.
#define VRY A0                //Pin VRY que da las coordenadas verticales Y del Joystick.
#define SW  11                //Pin SW del interruptor normalmente abierto del Arduino.

int X;                        //Coordenada X que adopta valores de 0 a 1023 por el ADC de 10 bits.
int Y;                        //Coordenada Y que adopta valores de 0 a 1023 por el ADC de 10 bits.

int CoordenadaX;              //Coordenada X después de la transformación de 0 a 1023 -> 0 a 5V.
int CoordenadaY;              //Coordenada Y después de la transformación de 0 a 1023 -> 0 a 5V.

int intervaloMuestreo = 50;   //Intervalo de muestreo dado en milisegundos.

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL:
void setup() {
  /*pinMode(): Método que indica cuales pines del Arduino son entradas y cuales son salidas:
      - primer parámetro: Indica el pin de Arduino que será asignado como salida o entrada.
      - segundo parámetro: Usa la instrucción OUTPUT para indicar que el pin es una salida o 
        INPUT para indicar que el pin es una entrada.
  El número del pin que recibe este método como primer parámetro se puede declarar directamente
  como un número o se puede declarar al inicio del programa como una variable.*/
  pinMode(VRX, INPUT);  //Constante VRX asignada al pin A1.
  pinMode(VRY, INPUT);  //Constante VRY asignada al pin A0.
  pinMode(SW, INPUT);   //Constante SW asignada al pin digital 11.
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
  Serial.println("Control Joystick");
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO:
void loop() {
  /*analogRead(): Método que lee un valor analógico por medio del ADC del Arduino que consta de 10
  bits, por lo que convertirá los valores de tensión de 0 a 5V que reciba a un equivalente de 0 a 
  2^10 - 1 = 1023.*/
  X = analogRead(VRX);
  Y = analogRead(VRY);
  /*digitalRead(Pin): Lo que hace este método es leer una entrada digital en un pin en específico que 
  se indica como su primer parámetro, se puede ver si este valor está en nivel alto con la constante 
  HIGH o si está en su nivel bajo con la constante LOW.*/
  int JoystickButton = digitalRead(SW);

  /*La zona de reposo es un cuadrado que se encuentra alrededor del punto inicial del joystick, que 
  como abarca valores de 0 a 5V, osea de 0 a 1023 después de ser procesador por el ADC de 10 bits del
  Arduino, idealmente se debería encontrar en la coordenada: 
    Posicion_inicial = (X,Y) = (1023/2,1023/2) = (511.5,511.5)
  Pero por seguridad se declara la zona de reposo que considera el error que se podría tener, abarcando
  posiciones de 480 a 520.*/
  if(X >= 0 && X < 480){      //Obtención de coordenadas -X.
    /*map(): Método que realiza una regla de 3, convirtiendo un número de un rango a otro que sea 
    equivalente al primero, en sus parámetros recibe lo siguiente:
      - Primer parámetro: Recibe la variable de donde proviene el número a convertir.
      - Segundo parámetro: Recibe el valor mínimo del primer rango de valores.
      - Tercer parámetro: Recibe el valor máximo del primer rango de valores.
      - Cuarto parámetro: Recibe el valor mínimo del segundo rango de valores, al que queremos llegar.
      - Quinto parámetro: Recibe el valor máximo del segundo rango de valores, al que queremos llegar.
    De esta manera se convierten los valores de de 0 a 480 entregados por el método analogRead() y la zona 
    de reposo de la dirección -X a valores de tensión de -5 a 0V.*/
    CoordenadaX = map(X, 0, 480, -5, 0);
  }else if(X >= 520 && X <= 1023){   //Obtención de coordenadas +X.
    CoordenadaX = map(X, 520, 1023, 0, 5);
  }else{
    CoordenadaX = 0;
  }
  if(Y >= 0 && Y < 480){      //Obtención de coordenadas -Y.
    CoordenadaY = map(Y, 0, 480, -5, 0);
  }else if(Y >= 520 && Y <= 1023){   //Obtención de coordenadas +Y.
    CoordenadaY = map(Y, 520, 1023, 0, 5);
  }else{
    CoordenadaY = 0;
  }
  /*Serial.println(): Método que escribe un mensaje en la consola serial de Arduino, a la cual se puede
  acceder en la esquina superior derecha donde se encuentra una lupa. Si queremos concatenar esto con 
  un valor se debe usar el método String() para convertir su contenido en una cadena de caracteres.*/
  Serial.println("Posicion Joystick = (" + String(CoordenadaX) + "V, " + String(CoordenadaY) + "V)");
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(intervaloMuestreo);
}
