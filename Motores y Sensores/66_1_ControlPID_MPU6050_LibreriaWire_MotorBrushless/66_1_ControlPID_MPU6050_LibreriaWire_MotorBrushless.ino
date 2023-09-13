/*66.1.-El control PID es un algoritmo utilizado en el código de sistemas embebidos, su objetivo es lograr que alguna medición 
en un robot o sistema se alcance y mantenga en un valor deseado o punto de referencia (setpoint) con una mínima desviación o 
error; como lo puede ser una posición, distancia, temperatura, flujo, orientación, entre otros.
Para que un controlador PID pueda trabajar necesita contar con al menos un sensor, un actuador y un sistema embebido que realice 
el análisis del sistema (microcontrolador, FPGA, Raspberry Pi, etc.). La librería que se usa en este ejemplo es Wire.*/
//IMPORTACIÓN DE LA LIBRERÍA WIRE Y SERVO:
#include <Wire.h>   //Librería que habilita la comunicación I2C en los pines A4 (SDA) y A5 (SCL) del Arduino.
#include <Servo.h>  //Librería de control de servomotores y motores brushless.



//DECLARACIÓN DE LAS VARIABLES DEL SENSOR: ACELERÓMETRO GIROSCOPIO MPU6050
//Si el PIN A0 está conectado a GND o a nada, el address del MPU6050 será 0X68, pero si está conectado a 5V, será 0X69.
const int MPU = 0X68;                                             //Dirección I2C del SLAVE MPU6050 = 0X68.
//Datos crudos recabados del sensor MPU6050:
float AccX, AccY, AccZ;                                           //3 grados de libertad acelerómetro.
float GyroX, GyroY, GyroZ;                                        //3 grados de libertad giroscopio.
//Datos previos a la orientación resultante después de ejecutar las primeras operaciones matemáticas sobre los datos crudos:
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;   //Orientación: Dirección XY y ángulo de inclinación 3D.
float accErrorX, accErrorY, gyroErrorX, gyroErrorY, gyroErrorZ;   //Errores de orientación XY y ángulo de inclinación.
int pruebasError = 0;                                             //Número de pruebas realizadas para promediar el error.
//Resultados de orientación: Roll (giro eje x), Pitch (giro eje y) y Yaw (giro eje z).
float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;                     //Tiempos recabados por medio del método millis().

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



//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL:
void setup() {
  //CONFIGURACIÓN DEL SENSOR ACELERÓMETRO Y GIRÓSCOPO MPU6050 QUE OBTIENE LA ORIENTACIÓN DEL SISTEMA:
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
  Serial.begin(19200);          //Velocidad de transmisión serial I2C: 19,200 baudios.
  /*Wire.begin(): Método que inicializa la comunicación I2C, después de este se deberá indicar con qué dirección de SLAVE se 
  está estableciendo la conexión y la velocidad del reloj.*/
  Wire.begin();                 //Método que inicia la comunicación I2C.
  /*Wire.beginTransmission(dirección): Método que permite a un dispositivo mandar o recibir datos de una dirección SLAVE en 
  específico. MASTER es un dispositivo que puede mandar datos y SLAVE es uno que los puede recibirlos mediante el protocolo 
  I2C, pero para que un SLAVE pueda recibir datos se debe indicar su dirección, ya que un MASTER puede mandar datos a varios 
  SLAVE a la vez.*/
  //CONFIGURACIÓN DE LA SEÑAL DE RELOJ Y MODO DE BAJO CONSUMO DEL MPU6050:
  Wire.beginTransmission(MPU);  //Método que indica a qué dirección de SLAVE se enviarán datos con el protocolo I2C.
  /*Wire.write(): Método que se debe usar dos veces, la primera vez permite indicar a qué registro del SLAVE se quiere acceder 
  y la segunda vez permite mandar un byte de información a dicho registro del SLAVE, cuya dirección fue previamente 
  inicializada con el método Wire.beginTransmission().
  El registro 6B = 107 del MPU6050 permite encender un modo de bajo consumo e indicar cuál será la fuente de la señal de reloj. 
  Mandando el valor de 0X00 se indica que el modo de bajo consumo se apague y que la señal de reloj sea la interna del módulo 
  de 8MHz.*/
  Wire.write(0x6B);             //Método que accede al registro 6B del SLAVE con dirección 0X68.
  Wire.write(0x00);             //Método que escribe el valor 00 en el registro 6B del SLAVE con dirección 0X68.
  Wire.endTransmission(true);   //Método que finaliza la transmisión de datos mandados a cualquier dirección de SLAVE.
  //CONFIGURACIÓN DE LA SENSIBILIDAD DE MEDICIÓN DE LA ACELERACIÓN EN EL MPU6050:
  Wire.beginTransmission(MPU);  //Método que indica a qué dirección de SLAVE se enviarán datos con el protocolo I2C.
  /*El registro 1C = 28 del MPU6050 permite indicar el rango de medición del acelerómetro, yendo desde ±2g hasta ±16g, se elije 
  un rango mayor cuando el dispositivo al que se quiere incorporar el MPU6050 estará sometido a aceleraciones muy grandes o 
  expuesto a fuerzas extremas, como lo puede ser en vehículos y aeronaves, monitoreo de impactos, robótica y sistemas de 
  movimiento rápido, etc. Mandando el valor de 0X00 se indica que el rango de medición del acelerómetro sea el mínimo de ±2g.*/
  Wire.write(0x1C);             //Método que accede al registro 1C del SLAVE con dirección 0X68.
  Wire.write(0x00);             //Método que escribe el valor 00 en el registro 1C del SLAVE con dirección 0X68, obteniendo un rango de ±2g.
  Wire.endTransmission(true);   //Método que finaliza la transmisión de datos mandados a cualquier dirección de SLAVE.
  //CONFIGURACIÓN DE LA SENSIBILIDAD DE MEDICIÓN DE LA ACELERACIÓN EN EL MPU6050:
  Wire.beginTransmission(MPU);  //Método que indica a qué dirección de SLAVE se enviarán datos con el protocolo I2C.
  /*El registro 1B = 27 del MPU6050 permite indicar el rango de medición del giroscopio, yendo desde ±250°/s hasta ±2000°/s, se 
  elije un rango mayor cuando el dispositivo al que se quiere incorporar el MPU6050 estará sometido a aceleraciones muy grandes 
  o expuesto a fuerzas extremas, como lo puede ser en vehículos y aeronaves, monitoreo de impactos, robótica, sistemas de 
  movimiento rápido, etc. Mandando el valor de 0X00 se indica que el rango de medición del acelerómetro sea el mínimo de ±250°/s.*/
  Wire.write(0x1B);             //Método que accede al registro 1B del SLAVE con dirección 0X68.
  Wire.write(0x00);             //Escribe el valor 00 en el registro 1B del SLAVE con dirección 0X68, obteniendo un rango de ±250°/s.
  Wire.endTransmission(true);   //Método que finaliza la transmisión de datos mandados a cualquier dirección de SLAVE.
  //FUNCIÓN PROPIA DE ESTE CÓDIGO PARA CALCULAR EL ERROR DE LOS DATOS RECABADOS DEL ACELERÓMETRO GIROSCOPIO MPU6050:
  calcularErrorSensor();

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



//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Operaciones matemáticas que calculan el sentido de giro y orientación del sensor.
void loop() {
  //OBTENCIÓN DE DATOS DE ORIENTACIÓN DEL MPU6050 A TRAVÉS DE SU ACELERÓMETRO Y GIRÓSCOPO:
  //LECTURA DE LOS DATOS DEL ACELERÓMETRO:
  /*En los 6 registros 3B, 3C, 3D, 3E, 3F y 40, cuyo correspondiente decimal son los 59, 60, ..., 64, viene contenida la 
  información de los acelerómetros correspondientes a los 3 ejes X, Y, Z. Estos vienen agrupados en dos registros diferentes 
  porque la información completa es de 16 bits, pero cada registro como máximo puede transportar 8.*/
  Wire.beginTransmission(MPU);  //Método que indica a qué dirección de SLAVE se enviarán datos con el protocolo I2C.
  Wire.write(0x3B);             //Método que accede al registro 3B del SLAVE con dirección 0X68.  
  Wire.endTransmission(false);  //Método que deja abierta la transmisión de datos mandados a cualquier dirección de SLAVE.
  /*Wire.requestFrom(address, quantity, stop): Método que permite solicitar datos a un dispositivo SLAVE conectado al bus I2C, 
  para ello previamente se tuvieron que haber ejecutado los métodos: Wire.beginTransmission(address), Wire.write(registro) y 
  Wire.endTransmission(false), ya que con ellos se indica a partir de qué registro de un SLAVE en específico se extraerá el 
  número de datos con longitud de 8 bits (byte) indicado en este método.*/
  Wire.requestFrom(MPU, 6, true); //Método que extrae datos de 6 registros (2 por acelerómetro), desde el 3B hasta el 40 del SLAVE con dirección 0X68.
  /*Wire.read(): Permite leer un dato de un SLAVE, para ello previamente se tuvo que haber ejecutado el método 
  Wire.requestFrom(address, quantity, stop). Como la aceleración X, Y, Z se comparte a través de 6 registros, donde cada 2 
  corresponden a una medición de 16 bits, se realiza la operación shift << y luego se aplica una compuerta OR | para primero 
  recorrer 8 bits a la derecha el contenido del primer registro dentro de un número binario y luego unir eso con el contenido 
  del segundo registro, obteniendo al final la información completa de 16 bits en una misma variable. Dependiendo del rango de 
  medición del acelerómetro, yendo desde ±2g hasta ±16g, se deberá dividir el dato obtenido de cada una de las 3 aceleraciones 
  entre cierto número, indicado en la página 29 del datasheet, bajo el nombre de LSB sensitibity. Para ±2g, LSB = 16,384.*/
  AccX = (Wire.read() << 8 | Wire.read())/16384.0;  //Aceleración lineal medida en el eje X.
  AccY = (Wire.read() << 8 | Wire.read())/16384.0;  //Aceleración lineal medida en el eje Y.
  AccZ = (Wire.read() << 8 | Wire.read())/16384.0;  //Aceleración lineal medida en el eje Z.
  
  //CALCULO DE LOS ÁNGULOS DE ROTACIÓN ALREDEDOR DEL EJE "X" (ROLLL), "Y" (PITCH): Se deben convertir a grados, porque 
  //inicialmente vienen en radianes.
  //CORRECCIÓN DE LAS SALIDAS DEL ACELERÓMETRO CON EL ERROR CALCULADO POR MEDIO DE LA FUNCIÓN PROPIA calcularErrorSensor():
  //accErrorX obtenido de la función calcularErrorSensor() = -1.93
  accAngleX = (atan((AccY)/sqrt(pow(AccX, 2) + pow(AccZ, 2))) * (180/PI)) - (-1.93);    //Inclinación alrededor del eje X = Roll  = arctan(Ay/√(Ax^2+Az^2))
  //accErrorY obtenido de la función calcularErrorSensor() = -1.88
  accAngleY = (atan(-1*(AccX)/sqrt(pow(AccY, 2) + pow(AccZ, 2))) * (180/PI)) - (-1.88); //Inclinación alrededor del eje Y = Pitch = arctan(Ax/√(Ay^2+Az^2))
  
  //LECTURA DE LOS DATOS DEL GIROSCOPIO:
  previousTime = currentTime;   //Variable que guarda el tiempo de ejecución del loop anterior para medir intervalos de tiempo entre ejecuciones.
  currentTime = millis();       //Actualización del tiempo transcurrido cada que se ejecute la función loop, para realizar la medición del intervalo de tiempo.
  elapsedTime = (currentTime - previousTime)/1000;  //Intervalo de tiempo medido en segundos.
  /*En los 6 registros 43, 44, 45, 46, 47 y 48, cuyo correspondiente decimal son los 67, 68, ..., 72, viene contenida la 
  información de los giróscopios correspondientes a los 3 ejes X, Y, Z. Estos vienen agrupados en dos registros diferentes 
  porque la información completa es de 16 bits, pero cada registro como máximo puede transportar 8.*/
  Wire.beginTransmission(MPU);  //Método que indica a qué dirección de SLAVE se enviarán datos con el protocolo I2C.
  Wire.write(0x43);             //Método que accede al registro 43 del SLAVE con dirección 0X68.  
  Wire.endTransmission(false);  //Método que deja abierta la transmisión de datos mandados a cualquier dirección de SLAVE.
  /*Wire.requestFrom(address, quantity, stop): Método que permite solicitar datos a un dispositivo SLAVE conectado al bus I2C, 
  para ello previamente se tuvieron que haber ejecutado los métodos: Wire.beginTransmission(address), Wire.write(registro) y 
  Wire.endTransmission(false), ya que con ellos se indica a partir de qué registro de un SLAVE en específico se extraerá el 
  número de datos con longitud de 8 bits (byte) indicado en este método.*/
  Wire.requestFrom(MPU, 6, true); //Método que extrae datos de 6 registros (2 por giróscopo), desde el 43 hasta el 48 del SLAVE con dirección 0X68.
  /*Wire.read(): Permite leer un dato de un SLAVE, para ello previamente se tuvo que haber ejecutado el método 
  Wire.requestFrom(address, quantity, stop). Como la aceleración angular X, Y, Z se comparte a través de 6 registros, donde 
  cada 2 corresponden a una medición de 16 bits, se realiza la operación shift << y luego se aplica una compuerta OR | para 
  primero recorrer 8 bits a la derecha el contenido del primer registro dentro de un número binario y luego unir eso con el 
  contenido del segundo registro, obteniendo al final la información completa de 16 bits en una misma variable. Dependiendo del 
  rango de medición del acelerómetro, yendo desde ±2g hasta ±16g, se deberá dividir el dato obtenido de cada una de las 3 
  aceleraciones entre cierto número, indicado en la página 31 del datasheet, bajo el nombre de LSB sensitibity. Para ±250°/s, 
  LSB sensitibity = 131.*/
  GyroX = (Wire.read() << 8 | Wire.read())/131.0;  //Aceleración angular medida en el eje X.
  GyroY = (Wire.read() << 8 | Wire.read())/131.0;  //Aceleración angular medida en el eje Y.
  GyroZ = (Wire.read() << 8 | Wire.read())/131.0;  //Aceleración angular medida en el eje Z.
  
  //CORRECCIÓN DE LAS SALIDAS DEL GIROSCOPIO CON EL ERROR CALCULADO POR MEDIO DE LA FUNCIÓN PROPIA calcularErrorSensor():
  GyroX = GyroX - (-2.09); //gyroErrorX = -2.09
  GyroY = GyroY - (-0.87); //gyroErrorY = -0.87
  GyroZ = GyroZ - (-1.29); //gyroErrorZ = -1.29

  //CÁLCULO DEL ÁNGULO DE ROTACIÓN XY AL MULTIPLICAR EL DATO DEL GIROSCOPIO °/S POR EL TIEMPO DE DURACIÓN DEL TEMPORIZADOR, 
  //ESTO ES EL EQUIVALENTE A INTEGRAR EL VALOR DE LA VELOCIDAD ANGULAR PARA OBTENER EL ÁNGULO Y ADEMÁS SE OBTIENE EL VALOR DE YAW:
  gyroAngleX = gyroAngleX + GyroX * elapsedTime;    //gyroAngleX = °/s * s = grados.
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;    //gyroAngleX = °/s * s = grados.
  yaw = yaw + GyroZ * elapsedTime;                  //gyroAngleZ = yaw = °/s * s = grados.

  //EL ÁNGULO EN XY MEDIDO CON EL ACELERÓMETRO SE COMBINA CON EL ÁNGULO XY MEDIDO CON EL GIRÓSCOPO PARA OBTENER EL RESULTADO DE 
  //ROLL Y PITCH, ESTE SE CONSIDERA COMO UN FILTRO DE LA SEÑAL:
  roll  = 0.96*gyroAngleX + 0.04*accAngleX;
  pitch = 0.96*gyroAngleY + 0.04*accAngleY;
  /*Serial.println(): Método que escribe un mensaje en la consola serial de Arduino, a la cual se puede acceder en la esquina 
  superior derecha donde se encuentra una lupa, pero si en vez de usar el Monitor Serie, nos introducimos en la opción de 
  Herramientas -> Serial Plotter, podremos ver tres gráficas que indican el cambio de cada magnitud.*/
  Serial.print("Roll:" + String(roll)); 
  Serial.print(String(", "));
  Serial.print("Pitch:" + String(pitch)); 
  Serial.print(String(", "));
  Serial.print("Yaw:" + String(yaw));
  Serial.print(String("\n"));

  
  //CONTROL PID: En este caso se busca que el ángulo de inclinación alrededor del eje y (osea el pitch) sea igual a cero, si no 
  //es así, se activa el actuador (motor BLDC), el cual hará lo posible para corregir el error y alcanzar el valor del setpoint.
  /*El error e(t) del sistema se obtiene al restar el punto deseado donde se busca que se encuentre el sistema, ya sea un ángulo, 
  distancia, temperatura, orientación, etc. menos el punto en el que realmente se encuentra el sistema, dado por el sensor.*/
  //error = ÁnguloAlrededorDelEje_y - ValorDeseado; La fórmula es al revés, pero en este caso se hace así porque setPoint = 0.
  error = pitch - setPoint;                             //Error actual.
  
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
  /*Servo.write(): Método usado para controlar la velocidad de giro del eje perteneciente a 
  un motor sin escobillas. Recibe como argumento un número que representa la velocidad deseada.*/
  motorBLDC.write(PWM_PID);       //Velocidad mínima = 20; Velocidad máxima = 180.
  
  //Actualización del error anterior, para que este se considere al calcular el control Derivativo.
  previous_error = error;
}



//FUNCIÓN PROPIA calcularErrorSensor(), PARA OBTENER UNA BUENA LECTURA DEL ERROR, EL SENSOR SE DEBE ENCONTRAR INICIALMENTE EN 
//UNA ESTRUCTURA PLANA:
void calcularErrorSensor(){
  /*Para calcular el error se realizan 200 lecturas durante el setup del programa, se suma el resultado de todas ellas y luego 
  se divide entre 200 para obtener así un promedio del error, este se imprimirá en pantalla y se deberá restar en el cálculo de 
  la función loop() para así hacer la corrección de dicho error.*/
  //CÁLCULO DEL ERROR EN EL ACELERÓMETRO:
  while(pruebasError < 200){
    Wire.beginTransmission(MPU);  //Método que indica a qué dirección de SLAVE se enviarán datos con el protocolo I2C.
    Wire.write(0x3B);             //Método que accede al registro 3B del SLAVE con dirección 0X68.  
    Wire.endTransmission(false);  //Método que deja abierta la transmisión de datos mandados a cualquier dirección de SLAVE.
    Wire.requestFrom(MPU, 6, true); //Método que extrae datos de 6 registros (2 por acelerómetro), desde el 3B hasta el 40 del SLAVE con dirección 0X68.
    AccX = (Wire.read() << 8 | Wire.read())/16384.0;  //Aceleración lineal medida en el eje X.
    AccY = (Wire.read() << 8 | Wire.read())/16384.0;  //Aceleración lineal medida en el eje Y.
    AccZ = (Wire.read() << 8 | Wire.read())/16384.0;  //Aceleración lineal medida en el eje Z.
    accErrorX = accErrorX + (atan((AccY)/sqrt(pow(AccX, 2) + pow(AccZ, 2))) * (180/PI));     //Actualización del error en el eje X.
    accErrorY = accErrorY + (atan(-1*(AccX)/sqrt(pow(AccY, 2) + pow(AccZ, 2))) * (180/PI));  //Actualización del error en el eje Y.
    pruebasError++;               //Aumento de la variable pruebasError, para ejecutar el loop de 200 iteraciones.
  }
  //CÁLCULO DEL ERROR PROMEDIO EN EL ACELERÓMETRO:
  accErrorX = accErrorX / 200;
  accErrorY = accErrorY / 200;
  pruebasError = 0;               //Reinicio de la variable pruebasError
  //CÁLCULO DEL ERROR EN EL GIROSCOPIO:
  while(pruebasError < 200){
    Wire.beginTransmission(MPU);  //Método que indica a qué dirección de SLAVE se enviarán datos con el protocolo I2C.
    Wire.write(0x43);             //Método que accede al registro 43 del SLAVE con dirección 0X68.  
    Wire.endTransmission(false);  //Método que deja abierta la transmisión de datos mandados a cualquier dirección de SLAVE.
    Wire.requestFrom(MPU, 6, true); //Método que extrae datos de 6 registros (2 por acelerómetro), desde el 43 hasta el 48 del SLAVE con dirección 0X68.
    GyroX = (Wire.read() << 8 | Wire.read());     //Aceleración angular medida en el eje X.
    GyroY = (Wire.read() << 8 | Wire.read());     //Aceleración angular medida en el eje Y.
    GyroZ = (Wire.read() << 8 | Wire.read());     //Aceleración angular medida en el eje Z.
    gyroErrorX = gyroErrorX + (GyroX / 131.0);    //Actualización del error en el eje X.
    gyroErrorY = gyroErrorY + (GyroY / 131.0);    //Actualización del error en el eje Y.
    gyroErrorZ = gyroErrorZ + (GyroZ / 131.0);    //Actualización del error en el eje Z.
    pruebasError++;               //Aumento de la variable pruebasError, para ejecutar el loop de 200 iteraciones.
  }
  //CÁLCULO DEL ERROR PROMEDIO EN EL GIRÓSCOPO:
  gyroErrorX = gyroErrorX / 200;
  gyroErrorY = gyroErrorY / 200;
  gyroErrorZ = gyroErrorZ / 200;
  pruebasError = 0;               //Reinicio de la variable pruebasError

  //IMPRESIÓN EN CONSOLA DEL PROMEDIO DE LOS ERRORES: Estps siempre se deberán restar, por lo que el signo obtenido aquí, será invertido en el código.
  Serial.print("\naccErrorX: ");
  Serial.println(accErrorX);      //accErrorX calculado   = -1.97
  Serial.print("accErrorY: ");
  Serial.println(accErrorY);      //accErrorY calculado   = -1.26
  Serial.print("gyroErrorX: ");
  Serial.println(gyroErrorX);     //gyroErrorX calculado  = -2.07
  Serial.print("gyroErrorY: ");
  Serial.println(gyroErrorY);     //gyroErrorY calculado  = -0.89
  Serial.print("gyroErrorZ: ");
  Serial.println(gyroErrorZ);     //gyroErrorZ calculado  = -1.28
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos. El valor de este específico 
  delay se cambia para observar los resultados arrojados por la función y así poder restarlo en sus ecuaciones correspondientes.*/
  delay(100);
}
