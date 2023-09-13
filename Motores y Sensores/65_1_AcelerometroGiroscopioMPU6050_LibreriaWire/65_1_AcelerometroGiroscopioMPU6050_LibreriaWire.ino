/*65.1.-El MPU6050 es un módulo de tipo IMU (Inertial Measure Unit), el cual sirve para medir la orientación de un dispositivo, 
tanto en inclinación, como en su dirección en el plano 2D, para ello incluye un acelerómetro que detecta la aceleración lineal 
y un giróscopo que censa la aceleración angular, esto se realiza a través de 6 grados de libertad y el sensor se debe colocar 
en el centro de masa del dispositivo que se quiere controlar, para así lograr una medición correcta. Los datos recibidos del 
acelerómetro y giróscopo serán combinados para medir de forma precisa la orientación. Aunque, en el programa que utiliza la 
librería Wire existe el problema de que el YAW tiene un error que aumenta al pasar el tiempo, además de que la calibración se 
debe realizar de forma manual, pero solamente se utilizará de forma didáctica y luego se mostrará un programa que utilice la 
librería i2cdevlib que realiza una calibración automática, además de que utiliza el DMP (Digital Motion Processor) incluido 
en el sensor para realizar los cálculos también de manera automática. Este sensor es muy utilizado en aeronaves, celulares, 
drones, gimbals, etc.*/
//IMPORTACIÓN DE LA LIBRERÍA WIRE:
#include <Wire.h> //Librería que habilita la comunicación I2C en los pines A4 (SDA) y A5 (SCL) del Arduino.

//DECLARACIÓN DE VARIABLES:
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

//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL:
void setup() {
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
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
  delay(20);
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: Operaciones matemáticas que calculan el sentido de giro y orientación del sensor.
void loop() {
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
  //accErrorX obtenido de la función calcularErrorSensor() = -1.59
  accAngleX = (atan((AccY)/sqrt(pow(AccX, 2) + pow(AccZ, 2))) * (180/PI)) - (-1.59);    //Inclinación alrededor del eje X = Roll  = arctan(Ay/√(Ax^2+Az^2))
  //accErrorY obtenido de la función calcularErrorSensor() = -1.68
  accAngleY = (atan(-1*(AccX)/sqrt(pow(AccY, 2) + pow(AccZ, 2))) * (180/PI)) - (-1.68); //Inclinación alrededor del eje Y = Pitch = arctan(Ax/√(Ay^2+Az^2))
  //LECTURA DE LOS DATOS DEL GIROSCOPIO:
  previousTime = currentTime;   //Variable que se actualiza para medir intervalos de tiempo.
  /*millis(): Método que devuelve el tiempo transcurrido en milisegundos, empezando a contar desde que se enciende la placa 
  Arduino y no deteniéndose hasta que esta se apague o llegue a su límite, que es el mismo dado por el tipo de dato unsigned 
  long: De 0 a 4,294,967,295 milisegundos = 1,193.0464 horas = 49.7102 días = 49 días y 17 horas.
  Cada que se utilice el método millis, se estará actualizando el tiempo guardado en una variable cualquiera, en este caso se 
  guarda en dos variables para crear un temporizador que mida un tiempo de duración, realizando una simple resta entre dos 
  medidas de tiempo.*/
  currentTime = millis();       //Guarda el tiempo transcurrido desde que se prendió el Arduino.
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
  GyroX = GyroX - (-2.03); //gyroErrorX = -2.03
  GyroY = GyroY - (-0.88); //gyroErrorY = -0.88
  GyroZ = GyroZ - (-1.21); //gyroErrorZ = -1.21

  //CÁLCULO DEL ÁNGULO DE ROTACIÓN XY AL MULTIPLICAR EL DATO DEL GIROSCOPIO °/S POR EL TIEMPO DE DURACIÓN DEL TEMPORIZADOR, 
  //ESTO ES EL EQUIVALENTE A INTEGRAR EL VALOR DE LA VELOCIDAD ANGULAR PARA OBTENER EL ÁNGULO Y ADEMÁS SE OBTIENE EL VALOR DE YAW:
  gyroAngleX = gyroAngleX + GyroX * elapsedTime;    //gyroAngleX = °/s * s = grados.
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;    //gyroAngleY = °/s * s = grados.
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
  Serial.println(accErrorX);      //accErrorX calculado   = -1.59
  Serial.print("accErrorY: ");
  Serial.println(accErrorY);      //accErrorY calculado   = -1.68
  Serial.print("gyroErrorX: ");
  Serial.println(gyroErrorX);     //gyroErrorX calculado  = -2.03
  Serial.print("gyroErrorY: ");
  Serial.println(gyroErrorY);     //gyroErrorY calculado  = -0.88
  Serial.print("gyroErrorZ: ");
  Serial.println(gyroErrorZ);     //gyroErrorZ calculado  = -1.21
  /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos. El valor de este específico 
  delay se cambia para observar los resultados arrojados por la función y así poder restarlo en sus ecuaciones correspondientes.*/
  delay(1000);
}
