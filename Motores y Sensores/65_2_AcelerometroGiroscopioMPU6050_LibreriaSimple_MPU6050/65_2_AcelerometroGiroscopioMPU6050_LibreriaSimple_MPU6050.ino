/*65.2.-El MPU6050 es un módulo de tipo IMU (Inertial Measure Unit), el cual sirve para medir la orientación de un dispositivo, 
tanto en inclinación, como en su dirección en el plano 2D, para ello incluye un acelerómetro que detecta la aceleración lineal 
y un giróscopo que censa la aceleración angular, esto se realiza a través de 6 grados de libertad y el sensor se debe colocar 
en el centro de masa del dispositivo que se quiere controlar, para así lograr una medición correcta. Cuando se utilice la librería 
i2cdevlib para establecer la comunicación I2C y la biblioteca Simple_MPU6050 para el manejo del sensor, los datos recibidos del 
acelerómetro y giróscopo serán combinados y procesados para obtener la orientación de forma automática por medio del DMP (Digital 
Motion Processor) incluido dentro del sensor, además de que elimina el error acumulativo en la lectura del Yaw. Este sensor es 
muy utilizado en aeronaves, celulares, drones, gimbals, etc.*/
//IMPORTACIÓN DE LA LIBRERÍA Simple_MPU6050, QUE UTILIZA LAS LIBRERÍAS i2cdevlib Y Simple_Wire:
#include "Simple_MPU6050.h" //Librería que habilita la comunicación I2C en los pines A4 (SDA), A5 (SCL) y Pin2 (INT) del Arduino.

//DECLARACIÓN DE CONSTANTES:
//Si el PIN A0 está conectado a GND o a nada, el address del MPU6050 será 0X68, pero si está conectado a 5V, será 0X69.
#define MPU6050_ADDRESS_AD0_LOW   0X68                     //Adress del MPU6050 si el Pin A0 está conectado a GND.
#define MPU6050_ADDRESS_AD0_HIGH  0X69                     //Adress del MPU6050 si el Pin A0 está conectado a 5V.
#define MPU6050_DEFAULT_ADDRESS   MPU6050_ADDRESS_AD0_LOW  //Dirección I2C del SLAVE MPU6050 = 0X68.
/*La calibración se realiza de manera automática por el DMP incluido en el módulo MPU6050, pero si por alguna razón esos valores 
calculados son erróneos, también se pueden incluir de forma manual. Cabe mencionar que los datos calculados automáticamente por 
el sensor, siempre se muestran hasta el principio de la lectura de valores en consola y si estos no se incluyen en el código, 
al ejecutar el programa aparecerá un mensaje en pantalla que no lo dejará correr hasta que introduzcamos cualquier letra y demos
clic en enter, al agregar la constante OFFSETS esto se deja de mostrar.*/
#define OFFSETS    388,     112,    1320,      68,      28,      40 //Valores personalizados de calibración para el sensor.

//DECLARACIÓN DE OBJETOS:
/*Objeto de la clase Simple_MPU6050 que permite establecer la comunicación I2C con el sensor MPU6050, obteniendo los 
datos de aceleración lineal y rotativa para realizar cálculos con ellos y obtener la orientación del dispositivo.*/
Simple_MPU6050 mpu;

//FUNCIONES PROPIAS DECLARADAS EN UNA LÍNEA:
/*La sintaxis utilizada en C++, que es el lenguaje que se usa para programar los microcontroladores de Arduino, es la sig:
#define   nombreFunción(parámetros)   Acción a ejecutar*/
/*spamtimer(): Función propia de 1 sola línea que crea un delay para evitar problemas al mostrar los datos de orientación 
en consola.*/
#define spamtimer(t) for (static uint32_t SpamTimer; (uint32_t) (millis() - SpamTimer) >= (t); SpamTimer = millis())
/*printfloatx(): Función propia de 1 sola línea que proporciona una forma personalizada de imprimir datos en consola, para 
ello recibe 5 parámetros: 
  Texto que muestra el nombre de la variable, 
  variable, 
  Número de caracteres que muestran el valor de la variable, 
  número de decimales mostrados,
  texto puesto al final.
Esto es muy útil al mostrar los resultados de orientación Roll, Pitch y Yaw obtenidos del sensor MPU6050.*/
#define printfloatx(Name,Variable,Spaces,Precision,EndTxt) print(Name); {char S[(Spaces + Precision + 3)];Serial.print(F(" ")); Serial.print(dtostrf((float)Variable,Spaces,Precision ,S));}Serial.print(EndTxt);

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL:
void setup() {
  /*uint8_t: Número binario sin signo que puede adoptar valores de 0 a 255. La presencia de la letra "t" del final significa 
  "tipo" y se utiliza para hacer que la variable sea portátil y consistente en diferentes plataformas.*/
  uint8_t val;
  /*Condicional que evalúa a través de un condicional if cuál herramienta de la librería i2cdevlib se utilizará para implementar
  la comunicación del protocolo I2C, ya sea la proporcionada por la biblioteca Wire de Arduino (I2CDEV_ARDUINO_WIRE) o una 
  implementación optimizada llamada Fastwire (I2CDEV_BUILTIN_FASTWIRE). En ambas opciones de inicialización se elige una señal 
  de reloj de 400,000 Hz = 400 kHz. Se utiliza la sintaxis de directiva #if para el condicional porque en esta parte se está 
  analizando una condición de la configuración del programa, por lo que se ejecuta en el momento de la compilación.*/
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    /*Wire.begin(): Método que inicializa la comunicación I2C, después de este se deberá indicar con qué dirección de SLAVE se 
    está estableciendo la conexión y la velocidad del reloj.*/
    Wire.begin();               //Método que inicia la comunicación I2C con la librería Wire.
    /*Wire.setClock(): Método que configura la velocidad de transmisión (frecuencia del reloj) del bus I2C en Arduino.*/
    Wire.setClock(400000);      //Reloj de 400,000 Hz = 400 kHz = 2.5 µs.
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true); //Método que inicia la comunicación I2C con la librería Fastwire.
  #endif
  /*Serial.begin(baudRate): Este método inicializa la comunicación serial entre la placa Arduino y la computadora, además de 
  que configura su velocidad de transmisión dada en unidad de baudios (bit trasmitido por segundo) que recibe como su único 
  parámetro:
      - En general, 9600 baudios es una velocidad de transmisión comúnmente utilizada y es compatible con la mayoría de los 
        dispositivos y programas. 
      - Si se necesita una transferencia de datos más rápida y el hardware/software lo admiten, se puede optar por velocidades 
        más altas como 115200 o 57600 baudios, pero en comunicación I2C habilitada por la librería Wire, se pueden usar 
        velocidades de 19,200 hasta 115,200 baudios.
  Es importante asegurarse de que la velocidad de transmisión especificada coincida con la velocidad de comunicación del otro 
  dispositivo al que se conecta el Arduino. Si la velocidad de transmisión no coincide, los datos pueden no transmitirse o 
  recibirse correctamente.*/
  Serial.begin(115200);  //Baudios = 115,200
  while(!Serial);
  /*Serial.println(): Método que escribe un mensaje en la consola serial de Arduino, a la cual se puede acceder en la esquina 
  superior derecha donde se encuentra una lupa, pero si en vez de usar el Monitor Serie, nos introducimos en la opción de: 
  Herramientas -> Serial Plotter, podremos ver tres gráficas que indican el cambio de cada magnitud. La función F(), permite que 
  el texto se almacene en la memoria de programa, lo que ayuda a ahorrar espacio en la memoria RAM.*/
  Serial.println(F("Inicio: "));
  /*Condicional que evalúa si se han declarado valores de calibración para el sensor en la directiva OFFSETS o no, luego pasa a
  indicar el address SLAVE del MPU6050, calibrar su rango de medición y cargar dichos datos al DMP (Digital Motion Processor) para
  que se encargue de realizar los cálculos de orientación.*/
  #ifdef OFFSETS
    Serial.println(F("Usando offsets predefinidos"));
    /*Simple_MPU6050.SetAddress(): Este método de la librería Simple_MPU6050 lo que hace es establecer la dirección SLAVE del 
    sensor MPU6050 para poder efectuar una comunicación I2C entre el Arduino y él.
    Simple_MPU6050.CalibrateMPU(): Este método de la librería Simple_MPU6050 lo que hace es indicar el rango de medición del 
    acelerómetro y giroscopio del MPU6050 para ajustar la precisión de sus lecturas al uso que se le vaya a dar.
    Simple_MPU6050.load_DMP_Image(): Este método carga una imagen (firmware) en el sensor MPU6050, utilizando el DMP (Digital 
    Motion Processor), el cual es un procesador dentro del sensor que puede realizar cálculos y filtrar datos de forma autónoma, 
    lo que simplifica el procesamiento en el microcontrolador principal y corrige el error acumulativo en la lectura del Yaw que 
    se tenía al utilizar la librería Wire. Cargar una imagen DMP en el sensor permite aprovechar estas capacidades.
    OFFSETS es una directiva declarada en este programa que permite calibrar el sensor, */
    mpu.SetAddress(MPU6050_ADDRESS_AD0_LOW).load_DMP_Image(OFFSETS);
  #else
    Serial.println(F("No se establecieron Offsets, haremos unos nuevos.\n"  // muestra texto estatico
                   " Colocar el sensor en un superficie plana y esperar unos segundos\n"
                   " Colocar los nuevos Offsets en #define OFFSETS\n"
                   " para saltar la calibracion inicial \n"
                   " \t\tPresionar cualquier tecla y ENTER"));
    while(Serial.available() && Serial.read());
    while(!Serial.available());
    while(Serial.available() && Serial.read());
    /*Simple_MPU6050.SetAddress().CalibrateMPU().load_DMP_Image(): Este método combinado permite indicar la dirección SLAVE del MPU, 
    calibrar su rango de medición dependiendo de su aplicación y cargar una imagen al DMP (Digital Motion Processor) que corrija el 
    error acumulativo en la lectura del Yaw que se tenía al usar la librería Wire.*/
    mpu.SetAddress(MPU6050_ADDRESS_AD0_LOW).CalibrateMPU().load_DMP_Image();
  #endif
  /*Simple_MPU6050.on_FIFO(): Este método se utiliza para habilitar la lectura continua de datos desde el FIFO del sensor MPU6050. 
  FIFO es una pequeña memoria introducida dentro del sensor, cuyas siglas significan First Input First Output y cuando está llena de 
  información es porque el DMP (Digital Motion Processor) ha terminado de realizar el cálculo de los datos de orientación: Roll, 
  Pitch y Yaw, por lo que están listos para mostrarse en consola, por eso recibe como parámetro la función mostrar_valores().*/
  mpu.on_FIFO(mostrar_valores);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Interrupciones que identifican el sentido de giro y orientación del sensor MPU6050.
void loop() {
  /*Simple_MPU6050.dmp_read_fifo():  Este método se utiliza para realizar la lectura de datos que se encuentran dentro de la memoria 
  FIFO en el sensor MPU6050.*/
  mpu.dmp_read_fifo();
}

//FUNCIÓN PROPIA DECLARADA EN VARIAS LINEAS:
/*mostrar_valores(): Esta función propia es llamada cada que se ejecute la interrupción conectada al Pin digital 2.*/
void mostrar_valores(int16_t*gyro, int16_t*accel, int32_t*quat, int32_t*timestamp){
  uint8_t SpamDelay = 100;                  //Delay aplicado en ms cada vez que se muestran los valores de orientación en consola.
  /*Quaternion: Es un tipo de dato que representa una rotación en el espacio tridimensional, el cual almacena la orientación 
  calculada por el sensor.*/
  Quaternion q;
  /*VectorFloat: Es un tipo de dato que representa un vector de tres dimensiones con valores decimales de punto flotante que se 
  utilizan para almacenar la información de la gravedad calculada por el sensor.*/
  VectorFloat gravity;
  float ypr[3] = {0, 0, 0};                 //Vector que almacena los resultados de la orientación: Yaw, Pitch y Roll (ypr).
  float xyz[3] = {0, 0, 0};                 //Vector que considera los 3 ejes coordenados tridimensionales XYZ.
  /*spamtimer(SpamDelay): Función propia que se ejecuta cada 150 milisegundos (indicado por la variable SpamDelay), la cual realiza 
  los cálculos del ángulo de Euler 3D para obtener los resultados de orientación: Yaw, Pitch y Roll en unidad de grados.*/
  spamtimer(SpamDelay){
    mpu.GetQuaternion(&q, quat);            //Representa la orientación del sensor en términos de una rotación tridimensional.
    mpu.GetGravity(&gravity, &q);           //Cálculo de la gravedad en términos de un vector tridimensional.
    mpu.GetYawPitchRoll(ypr, &q, &gravity); //Cálculo para la obtención de los resultados de orientación: Yaw, Pitch y Roll (ypr).
    mpu.ConvertToDegrees(ypr, xyz);         //Conversión de radianes a grados, ya que el resultado del cálculo anterior se da en rad.
    /*Serial.printfloatx(): Método que permite utilizar el formato de la función propia printfloatx() para imprimir en consola 
    los datos recabados del sensor MPU6050 de forma personalizada, recibiendo como parámetros: El texto que muestra antes del valor de 
    la variable, el valor de la variable, número de caracteres que muestran el valor de la variable, número de decimales mostrados y 
    el texto que aparece al final.*/
    Serial.printfloatx(F("Roll: "),   xyz[2], 9, 4, F(",\t"));  //Giro alrededor del eje X = Roll.
    Serial.printfloatx(F("Pitch: "),  xyz[1], 9, 4, F(",\t"));  //Giro alrededor del eje Y = Pitch.
    Serial.printfloatx(F("Yaw: "),    xyz[0], 9, 4, F(",\t"));  //Giro alrededor del eje Z = Yaw.
    Serial.println();
    //OPERACIONES CONDICIONALES UTILIZANDO LOS VALORES DE ORIENTACIÓN OBTENIDOS POR EL SENSOR:
    if(xyz[2] >= 90){
      Serial.println("Roll = 90°;   Rotación alrededor del eje X.");
    }else if(xyz[1] >= 90){
      Serial.println("Pitch = 90°;  Rotación alrededor del eje Y.");
    }else if(xyz[0] >= 90){
      Serial.println("Yaw   = 90°;  Rotación alrededor del eje Z.");
    }
  }
}
