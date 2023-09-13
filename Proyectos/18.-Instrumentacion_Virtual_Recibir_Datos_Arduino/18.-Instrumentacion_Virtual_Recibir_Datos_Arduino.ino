//PROGRAMA PARA RECOPILAR DATOS Y MANDARLOS A PYTHON PARA QUE LOS GRAFIQUE EN TIEMPO REAL
int led = 13;           //Puerto de salida donde hay un led integrado en el Arduino
int voltage = A0;       //Declaración del puerto de entrada analógico que se quiere leer
int n = 0;              //variable que lleva la cuenta de los ciclos de parpadeo del LED.

//CONFIGURACIÓN DE LOS PINES Y COMUNICACIÓN SERIAL
void setup() {
  /*En esta parte del código Arduino se indican los puertos de salida, de entrada y la velocidad 
  de la comunicación serial*/
  /*pinMode(): Método que indica cuales pines del Arduino son entradas y cuales son salidas:
      - primer parámetro: Indica el pin de Arduino que será asignado como salida o entrada.
      - segundo parámetro: Usa la insctrucción OUTPUT para indicar que el pin es una salida o 
        INPUT para indicar que el pin es una entrada.
  El número del pin que recibe este método como primer parámetro se puede declarar directamente
  como un número o se puede declarar al inicio del programa como una variable.*/
  pinMode(led, OUTPUT); //El pin 13 es una salida digital.
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
  Serial.begin(9600);   //Comunicación serial de 9600 baudios.
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  /*El código principal se coloca dentro de la instrucción loop() para que se ejecute 
  interminablemente en el microcontrolador ATMEGA328P de Arduino.*/
  /*La operación % significa módulo y lo que hace esta es dividir un número y ver el resultado de 
    su residuo, en el caso de la operación 3%2 == 0, lo que está haciendo es dividir 3/2 y ver si 
    su residuo es cero, que en este caso no lo sería, ya que residuo = 1.
      - n%2 == 0: La operación verifica si el valor de n es divisible por 2 sin dejar residuo. 
        En otras palabras, verifica si n es un número par.*/
  if(n%2 == 0){         //Si n es par se prende el led
    /*digitalWrite(Pin, State): Lo que hace este método es mandar una salida digital a un pin en 
    específico que se indica como su primer parámetro, en su segundo parámetro se puede mandar la 
    constante HIGH para mandar 5V al pin o LOW para mandar 0V, osea no mandar nada.*/
    digitalWrite(led, HIGH);  //Con esta línea de código se prende el led
  }else{                //Si n es impar NO se prende el led
    digitalWrite(led, LOW);   //Con esta línea de código se apaga el led
  }
  n = n+1;              //Después de ver si n es par, se le suma 1 antes de su siguiente ejecución.

  //Posteriormente se reinicia la variable después de que haya llegado a n = 100.
  if(n == 100){
    n = 0;
  }

  /*analogRead(): El método se utiliza para leer valores analógicos de un pin específico, permitiendo 
  leer la tensión analógica presente en un pin y convertirla en un valor digital.
      - Pines Analógicos A0, A1,..., A5: No es necesario configurarlos explícitamente, ya que el método 
        se encarga de establecerlos como entradas analógicas automáticamente.
      - Pines Digitales 0, 1,..., 13: Estos pines antes de utilizarlos se deben establecer por medio del
        método pinMode() como entradas.
  El ADC del Arduino es de 10 bits, esto significa que cuando reciba su valor máximo de 5V, en la consola 
  imprimirá (2^10)-1 = 1023, ya que es el valor máximo que puede convertir de analógico a digital porque 
  recibe tensiones de 0 a 5V y por lo tanto cuenta con valores digitales de 0 a 1023 en formato decimal.*/
  int data = analogRead(voltage);//Lectura analógica del puerto A0 para imprimirlo en la consola de Arduino

  /*Serial.println(): Método que imprime en las Herramientas Monitor Serie y Serial Plotter el valor dado
  en su parámetro.*/
  Serial.println(data);
  
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(1000);          //Esto retrasa 500 milisegundos el código antes de volver a ejecutarse.
}
