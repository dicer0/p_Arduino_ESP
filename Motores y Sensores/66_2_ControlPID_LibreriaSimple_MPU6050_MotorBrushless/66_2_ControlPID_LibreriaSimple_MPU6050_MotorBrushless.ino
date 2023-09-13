/*66.1.-El control PID es un algoritmo utilizado en el código de sistemas embebidos, su objetivo es lograr que alguna medición 
en un robot o sistema se alcance y mantenga en un valor deseado o punto de referencia (setpoint) con una mínima desviación o 
error; como lo puede ser una posición, distancia, temperatura, flujo, orientación, entre otros.
Para que un controlador PID pueda trabajar necesita contar con al menos un sensor, un actuador y un sistema embebido que realice 
el análisis del sistema (microcontrolador, FPGA, Raspberry Pi, etc.). Con este código se utilizan las librerías i2cdevlib para 
establecer la comunicación I2C y la librería Simple_MPU6050 para obtener los datos de orientación del sensor.*/
//IMPORTACIÓN DE LA LIBRERÍA Simple_MPU6050, QUE UTILIZA LAS LIBRERÍAS i2cdevlib Y Simple_Wire:
#include "Simple_MPU6050.h" //Librería que habilita la comunicación I2C en los pines A4 (SDA), A5 (SCL) y Pin2 (INT) del Arduino.
#include <Servo.h>  //Librería de control de servomotores y motores brushless.



//DECLARACIÓN DE LAS VARIABLES, OBJETOS Y CONSTANTES DEL SENSOR: ACELERÓMETRO GIROSCOPIO MPU6050
//Si el PIN A0 está conectado a GND o a nada, el address del MPU6050 será 0X68, pero si está conectado a 5V, será 0X69.
#define MPU6050_ADDRESS_AD0_LOW   0X68                     //Adress del MPU6050 si el Pin A0 está conectado a GND.
#define MPU6050_ADDRESS_AD0_HIGH  0X69                     //Adress del MPU6050 si el Pin A0 está conectado a 5V.
#define MPU6050_DEFAULT_ADDRESS   MPU6050_ADDRESS_AD0_LOW  //Dirección I2C del SLAVE MPU6050 = 0X68.
/*La calibración se realiza de manera automática por el DMP incluido en el módulo MPU6050, pero si por alguna razón esos valores 
calculados son erróneos, también se pueden incluir de forma manual. Cabe mencionar que los datos calculados automáticamente por 
el sensor, siempre se muestran hasta el principio de la lectura de valores en consola y si estos no se incluyen en el código, 
al ejecutar el programa aparecerá un mensaje en pantalla que no lo dejará correr hasta que introduzcamos cualquier letra y demos
clic en enter, al agregar la constante OFFSETS esto se deja de mostrar.*/
#define OFFSETS    414,     132,    1322,      70,      30,      43 //Valores personalizados de calibración para el sensor.
/*Objeto de la clase Simple_MPU6050 que permite establecer la comunicación I2C con el sensor MPU6050, obteniendo los 
datos de aceleración lineal y rotativa para realizar cálculos con ellos y obtener la orientación del dispositivo.*/
Simple_MPU6050 mpu;
float elapsedTime, currentTime, previousTime;              //Tiempos recabados por medio del método millis().
//Número de muestras saltadas para evitar detectar los errores del proceso de calibración del DMP (Digital Motion Processor).
int muestrasConError = 300;                                //Muestras con error causadas por la calibracion del DMP.

//DECLARACIÓN DE LOS OBJETOS Y LAS VARIABLES DEL ACTUADOR: MOTOR BRUSHLESS KV = 2300
/*Objeto de la clase Servo, este no recibe ningún parámetro, solamente sirve para poder usar los 
métodos de la librería.*/
Servo motorBLDC;                //Objeto que permite utilizar la librería Servo para crear la señal PWM.
int PINPWM_BLDC = 3;            //Pin 3 = Cable de señal PWM conectada al PIN3~ digital del Arduino.
/*Cuando el valor bajo del duty cycle en la señal PWM dura 1000 µs = 1 ms, la velocidad del BLDC
está en 0 [rpm]. Esta es la teoría, pero si el motor no gira, este valor se debe calibrar para 
encontrar el óptimo, además al editar esto se puede cambiar el alcance de la velocidad de giro 
del motor.*/
int DUTYCYCLEMIN = 0;           //Calibración BLDC: High Duty Cycle Mínimo ≈ 0 µs = 0 s.
/*Cuando el valor alto del duty cycle en la señal PWM dura 2000 µs = 2 ms, la velocidad del BLDC
está en KV * V_ESC [rpm]. Esta es la teoría, pero si el motor deja de girar, este valor se debe 
calibrar para encontrar el óptimo, además al editar esto se puede cambiar el alcance de la 
velocidad de giro del motor.*/
int DUTYCYCLEMAX = 3000;        //Calibración BLDC: High Duty Cycle Máximo ≈ 3000 µs = 3 ms.
//Velocidad mínima alcanzada con la señal PWM generada con la librería Servo = 20; Velocidad máxima = 180.
float PWM_PID = 20;             //Ajuste en la señal PWM por medio del control PID, empieza en el mínimo que es de 20.

//DECLARACIÓN DE LAS VARIABLES PID: Error, Ecuación PID Proporcional, Integral, Derivativa y Constantes Kp, Ki y Kd.
float error, previous_error;    //Error actual y anterior.
/*La ecuación diferencial del PID se compone de tres partes, la proporcional, la integral y la derivativa, cada una de ellas 
está encargada de eliminar el error en el sistema en un punto del tiempo diferente:
Proporcional = Presente; Integral = Pasado; Derivativo = Futuro.
Dicho error e(t) se obtiene al restar el punto deseado donde se busca que se encuentre el sistema menos el punto en el que 
realmente se encuentra el sistema, dado por el sensor; el punto puede ser una distancia, ángulo, temperatura, orientación, etc. 
La ecuación diferencial del controlador PID es:
PID = kp*e(t) + (PID_I + ki*e(t)) + kd*(e(t)/t) = PID_P + PID_I + PID_D 
Donde: PID_P = kp * e(t);      PID_I = PID_I + ki * e(t);      PID_D = kd * (e(t)/t)
Se deben elegir distintos valores para las constantes kp, ki y kd, de la ecuación, al hacerlo se calibrará el controlador PID.*/
float PID_P = 0;                //Reinicio de componente proporcional de la ecuación PID.
float PID_I = 0;                //Reinicio de componente integral de la ecuación PID.
float PID_D = 0;                //Reinicio de componente derivativa de la ecuación PID.
//Constantes de calibración kp, ki y kd.
float kp = 3.55;                //Constante proporcional kp = Presente; Esta se empieza a calibrar con valores entre 1 y 10.
float ki= 0.005;                //Constante integral ki = Pasado; Esta se empieza a calibrar con valores entre 0 y 1.
float kd = 2.05;                //Constante derivativa kd = Futuro; Esta se empieza a calibrar con valores entre 1 y 10.
//Ecuación PID final = PID = kp*e(t) + (PID_I + ki*e(t)) + kd*((e(t)-e(t-1))/t) = PID_P + PID_I + PID_D.
float PID;                      
//Constante que busca alcanzar o mantener por medio del control PID.
float setPoint = 0;             //Ángulo pitch (alrededor del eje y) = 0°.



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
  //CONFIGURACIÓN DEL SENSOR ACELERÓMETRO Y GIRÓSCOPO MPU6050 QUE OBTIENE LA ORIENTACIÓN DEL SISTEMA:
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

  //CONFIGURACIÓN DEL ACTUADOR DE TIPO MOTOR BRUSHLESS QUE SE ACCIONA PARA ELIMINAE EL ERROR DEL SISTEMA:
  /*servomotor.attach(): Método que sirve para inicializar el objeto del servomotor:
    - El primer parámetro indica el Pin del Arduino al que se conectó el cable de la señal PWM.
    - El segundo parámetro indica la duración mínima en µs de la señal PWM y el tercero la máxima.*/
  motorBLDC.attach(PINPWM_BLDC, DUTYCYCLEMIN, DUTYCYCLEMAX);

  //CONFIGURACIÓN PARA EL CONTROLADOR PID:
  /*millis(): Método que devuelve el tiempo transcurrido en milisegundos, empezando a contar desde que se enciende la placa 
  Arduino y no deteniéndose hasta que esta se apague o llegue a su límite, que es el mismo dado por el tipo de dato unsigned 
  long: De 0 a 4,294,967,295 milisegundos = 1,193.0464 horas = 49.7102 días = 49 días y 17 horas.
  Cada que se utilice el método millis, se estará actualizando el tiempo guardado en una variable cualquiera, en este caso se 
  guarda en dos variables para crear un temporizador que mida un tiempo de duración, realizando una simple resta entre dos 
  medidas de tiempo.*/
  currentTime = millis();       //Guarda el tiempo transcurrido desde que se prendió el Arduino.
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(20);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Interrupciones que identifican el sentido de giro y orientación del sensor MPU6050.
void loop() {
  /*Simple_MPU6050.dmp_read_fifo():  Este método se utiliza para realizar la lectura de datos que se encuentran dentro de la memoria 
  FIFO en el sensor MPU6050.*/
  mpu.dmp_read_fifo();
  
  /*Servo.write(): Método usado para controlar la velocidad de giro del eje perteneciente a 
  un motor sin escobillas. Recibe como argumento un número que representa la velocidad deseada.*/
  motorBLDC.write(PWM_PID);       //Velocidad mínima = 20; Velocidad máxima = 180.
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
  /*spamtimer(SpamDelay): Función propia que se ejecuta cada 100 milisegundos (indicado por la variable SpamDelay), la cual realiza 
  los cálculos del ángulo de Euler 3D para obtener los resultados de orientación: Yaw, Pitch y Roll en unidad de grados. Además 
  dentro se incluye el control PID.*/
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
    
    
    /*CONTROL PID: En este caso se busca que el ángulo de inclinación alrededor del eje y (osea el pitch) sea igual a cero, si no 
    es así, se activa el actuador (motor BLDC), el cual hará lo posible para corregir el error y alcanzar el valor del setpoint.*/
    //Variable que guarda el tiempo de ejecución del loop anterior para medir intervalos de tiempo entre ejecuciones.
    previousTime = currentTime;   
    //Actualización del tiempo transcurrido cada que se ejecute la función loop, para realizar la medición del intervalo de tiempo.
    currentTime = millis();       
    elapsedTime = (currentTime - previousTime)/1000;      //Intervalo de tiempo medido en segundos.
    /*El error e(t) del sistema se obtiene al restar el punto deseado donde se busca que se encuentre el sistema, ya sea un ángulo, 
    distancia, temperatura, orientación, etc. menos el punto en el que realmente se encuentra el sistema, dado por el sensor.*/
    //error = ÁnguloAlrededorDelEje_y - ValorDeseado; La fórmula es al revés, pero en este caso se hace así porque setPoint = 0.
    error = xyz[1] - setPoint;                            //Error actual = Pitch != 0°;  Rotación alrededor del eje Y.
    /*La ecuación diferencial del PID se compone de tres partes, la proporcional, la integral y la derivativa, cada una de ellas 
    está encargada de eliminar el error en el sistema en un punto del tiempo diferente: 
    Proporcional = Presente;      Donde: PID_P = kp * e(t).*/
    PID_P = kp*error;                                     //Control P: Proporcional.
    /*La ecuación diferencial del PID se compone de tres partes, la proporcional, la integral y la derivativa, cada una de ellas 
    está encargada de eliminar el error en el sistema en un punto del tiempo diferente: 
    Integral = Pasado;            Donde: PID_I = PID_I + ki * e(t).
    En este caso la parte integral se utiliza para detectar errores muy pequeños, cuando esto sea asi, la componente del control 
    integral irá aumentando gradualmente su valor para contrarrestar el error con una fuerza acumulativa, en este caso el rango 
    de acción del control integral es de ±3°.*/
    if(-3 < error < 3){
      PID_I = PID_I + (ki*error);                         //Control I: Integral.  
    }
    /*La ecuación diferencial del PID se compone de tres partes, la proporcional, la integral y la derivativa, cada una de ellas 
    está encargada de eliminar el error en el sistema en un punto del tiempo diferente: 
    Derivativo = Futuro;          Donde: PID_D = kd * (e(t)/t)
    Como la parte derivativa depende del tiempo, al ser una derivada, lo que se hace es dividirla entre el lapso de tiempo calculada
    para la medición del sensor MPU6050 y además se considera un error anterior para realizar de igual manera un intervalo del error.*/
    PID_D = kd*((error - previous_error) / elapsedTime);  //Control D: Derivativo.
    /*Por lo tanto, la ecuación que conforma a todo el controlador PID es la siguiente:
    PID = kp*e(t) + (PID_I + ki*e(t)) + kd*(e(t)/t) = PID_P + PID_I + PID_D 
    De esta ecuación se deben elegir distintos valores para las constantes kp, ki y kd, al hacerlo se calibrará el controlador PID.*/
    PID = PID_P + PID_I + PID_D;                          //Ecuación PID completa.
  
    /*Ya habiendo declarado las ecuaciones PID se debe sumar o restar el resultado de su ecuación final al actuador para así generar 
    la automatización del sistema, ya que dependiendo de a donde se mueva el drone, el ángulo del pitch será positivo o negativo, pero 
    para ello se deben establecer límites, porque el BLDC solo acepta velocidades de 20 a 180 utilizando el método Servo.write() y 
    empieza con el valor de 20. Por lo tanto, el controlador PID puede adoptar valores de -20 a 160.*/
    if(PID <= -20){                 //Condición que limita el valor del controlador PID.
      PID = -20;
    }else if(PID >= 160){
      PID = 160;
    }
    
    /*Después de haber considerado el límite del valor en el control PID, este se suma al valor de la variable PWM_PID que moverá 
    al motor, o también se podría restar dependiendo del sentido de giro y el lado del drone en el que se encuentre el motor BLDC. 
    Recordemos que cuando esto se aplique a un drone, el valor de PID se sumaría a la señal PWM de un motor y se le restaría al del 
    otro lado para que el motor se pueda equilibrar, ya que el ángulo de orientación pitch es positivo hacia un lado y negativo hacia 
    el otro. Posteriormente se creará otra condición que limite el valor entregado hacia la señal PWM de 20 a 180, que son sus valores
    mínimos y máximos de velocidad.*/
    //Control PID aplicado a la señal PWM que activa un motor sin escobillas.
    PWM_PID = PWM_PID + PID;        //Control PID del actuador.
    //Límite de velocidad mínima y máxima entregada al motor BLDC.
    if(PWM_PID <= 20){              //Condición que limita el duty cycle de la señal PWM entregada al actuador.
      PWM_PID = 20;                 //Velocidad mínima = 20.
    }else if(PWM_PID >= 180){
      PWM_PID = 180;                //Velocidad máxima = 180.
    }
    
    //Actualización del error anterior, para que este se considere al calcular el control Derivativo.
    previous_error = error;
  }
}
