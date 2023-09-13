/*23.2.-MAQUINA DE ESTADO FINITO TIPO MOORE: Las FSM (Finite State Machines) están conformadas de 
entradas, transiciones y estados, donde cada estado tiene asignada una o varias salidas. Este tipo 
de programación es excelente para detectar o ejecutar secuencias. 
En la máquina de Moore sus salidas se generan solo cuando se alcanza un estado específico, por lo que 
su velocidad es más lenta, eso es bueno cuando la secuencia es ingresada manualmente, pero no cuando 
se ingresa por medios digitales, ya que la velocidad de la entrada supera la de la detección.
El estado se refiere a un concepto abstracto que indica como una FSM reacciona automáticamente 
a una secuencia de entradas según un flujo prestablecido. Donde un mismo sistema, si se 
encuentra en diferente estado, podrá reaccionar de forma distinta a una misma entrada.*/
//DECLARACIÓN DE CONSTANTES:
/*#define: Instrucción utilizada para crear con una directiva una constante global en el programa de 
Arduino siguiendo la sintaxis descrita a continuación:
  #define nombreConstante valorConstante*/
#define boton 2         //Botón que acciona la secuencia de la máquina de estados, conectado al pin 2.
#define ledSecuencia 13 //Led que indica cuando la secuencia se ha completado, conectado al pin 13.
//DECLARACIÓN DE VARIABLES:
int estadoPresente = 0;
int transicionEstado = 0;
/*unsigned long: Variable sin signo de 32 bits, puede abarcar números muy grandes de 0 a 4,294,967,295
y su principal uso es el de guardar el tiempo transcurrido durante la ejecución del programa, recopilado 
por el metodo millis() posteriormente.*/
unsigned long tiempoFSM = 0;          //Guarda el tiempo transcurrido desde que se prendió el Arduino.
unsigned long tiempoDeEsperaFSM = 0;  //Variable que se actualiza para medir intervalos de tiempo.
int intervaloDuracionEstados = 500;  //Duración de un estado sin pasar automáticamente al otro = 2 s.
int delayAntirrebotes = 150;  //Tiempo de espera que el programa deja pasar como antirrebotes = 150 ms.



//CONFIGURACIÓN DE LOS PINES Y LA COMUNICACIÓN SERIAL:
void setup() {
  /*pinMode(): Método que indica cuales pines del Arduino son entradas y cuales son salidas:
      - primer parámetro: Indica el pin de Arduino que será asignado como salida o entrada.
      - segundo parámetro: Usa la instrucción OUTPUT para indicar que el pin es una salida o 
        INPUT para indicar que el pin es una entrada.
  El número del pin que recibe este método como primer parámetro se puede declarar directamente
  como un número o se puede declarar al inicio del programa como una variable.*/
  pinMode(boton, INPUT);          //El botón del pin 2 es una entrada en configuración pull-down.
  /*Resistencia pull-down: Siempre está en nivel lógico bajo (0 lógico) y al ser presionada cambia a ser 
  1 lógico.*/
  pinMode(ledSecuencia, OUTPUT);  //El led del pin 13 es una salida.
  /*Serial.begin(baudRate): Este método inicializa la comunicación serial entre la placa Arduino 
  y la computadora, además de que configura su velocidad de transmisión dada en unidad de baudios 
  (bit trasmitido por segundo) que recibe como su único parámetro:
      - En general, 9600 baudios es una velocidad de transmisión comúnmente utilizada y es 
        compatible con la mayoría de los dispositivos y programas. 
      - Sin embargo, si se necesita una transferencia de datos más rápida y el hardware/software 
        lo admiten, se puede optar por velocidades más altas como 115200 o 57600 baudios.
  Es importante asegurarse de que la velocidad de transmisión especificada coincida con la 
  velocidad de comunicación del otro dispositivo al que se conecta el Arduino. Si la velocidad de 
  transmisión no coincide, los datos pueden no transmitirse o recibirse correctamente, además lo que 
  sea que se quiera imprimir en consola, sin haber ejecutado este método no aparecerá.*/
  Serial.begin(9600);   //Comunicación serial de 9600 baudios, para recibir la secuencia 1011 del botón.
  /*Serial.println(): Método que escribe un mensaje en la consola serial de Arduino, a la cual se puede
  acceder en la esquina superior derecha donde se encuentra una lupa.*/
  Serial.println("Máquina de Estados Mealy - Código Arduino: Secuencia 1011");
}



//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO: FSM tipo Moore que identifica la secuencia 1011, la salida se mantiene estática.
void loop() {
  /*Condicional else if: Con este condicional se evalúa la variable estadoPresente para describir la acción
  que se ejecuta en cada estado y sus transiciones a los siguientes.*/
  //MÁQUINA DE ESTADOS TIPO MOORE: Las salidas de la FSM se declaran en sus estados.
  if(estadoPresente == 0){        //Estado 0 = Inicio.
    //SALIDAS DEL ESTADO 0 = Inicio:
    Serial.println("Estado 0 = Inicio: No se ha presionado nada.");
    /*digitalWrite(Pin, State): Lo que hace este método es mandar una salida digital a un pin en 
    específico que se indica como su primer parámetro, en su segundo parámetro se puede mandar la 
    constante HIGH para mandar 5V al pin o LOW para mandar 0V, osea no mandar nada.*/
    digitalWrite(ledSecuencia, LOW); //Salida del Estado 0 = Led del Pin 13 Apagado.
    
    /*TRANSICIONES DEL Estado de Inicio:*/
    /*Antirrebotes: El código de antirrebotes lo que hace es esperar cierto tiempo después de haber sido 
    presionado un botón, para evitar censar falsos pulsos, ya que el botón mecánico rebota después de 
    haber sido presionado.*/
    //DELAY ANTIRREBOTES DE 150 MS Y ACTUALIZACIÓN DEL INTERVALO DE TIEMPO PARA EL TEMPORIZADOR DEL ESTADO 1:
    /*millis(): Método que devuelve el tiempo transcurrido en milisegundos, empezando a contar desde que 
    se enciende la placa Arduino y no deteniéndose hasta que esta se apague o llegue a su límite, que es 
    el mismo dado por el tipo de dato unsigned long: De 0 a 4,294,967,295 milisegundos = 1,193.0464 horas
    49.7102 días = 49 días y 17 horas.
    La función se puede utilizar para detener el programa, sustituyendo al método delay(), la razón de 
    ello es que el método delay() detiene completamente la ejecución del programa, mientras que el método 
    millis() permite que otros procesos se ejecuten mientras solo una parte del código se pausa.
    Cada que se utilice el método millis, se estará actualizando el tiempo guardado en una variable 
    cualquiera, en este caso se guarda en dos variables: 
      - tiempoDeEsperaFSM:  Variable que se ejecuta cada que se realice una transición entre estados, 
        guardando así el tiempo de duración del estado al que se va a brincar y permitiendo que se 
        ejecute el delay del antirrebotes una vez presionado el botón, para que no se capten falsas 
        pulsaciones.
      - tiempoFSM:  Variable que guarda el tiempo cuando se inicia cada estado, para así compararlo con 
        el tiempo guardado en la variable tiempoDeEsperaFSM y lograr que, pasado un intervalo de tiempo, 
        la máquina de estados brinque automáticamente al siguiente estado. Además, se compara también con
        la variable delayAntirrebotes para pausar el programa cierto tiempo, evitando que se capten 
        falsas pulsaciones.
    Por lo tanto, con el método millis también se pueden crear temporizadores que mida un tiempo de 
    duración, realizando una simple resta entre dos medidas de tiempo y creando una comparación con un 
    operador de mayor o menor que y un condicional if.*/
    tiempoFSM = millis();   //Actualización del tiempo para ejecutar el antirrebotes.
    //DELAY ANTIRREBOTES DE 150 MS:
    /*Con esta instrucción se crea el código antirrebotes, que no deja recopilar datos después de haber 
    presionado el botón una vez para no captar falsas pulsaciones.*/
    if(tiempoFSM - tiempoDeEsperaFSM > delayAntirrebotes){
      /*digitalRead(Pin): Lo que hace este método es leer una entrada digital en un pin en específico 
      que se indica como su primer parámetro, se puede ver si este valor está en nivel alto con la 
      constante HIGH o 1 y si está en su nivel bajo con la constante LOW o 0.*/
      transicionEstado = digitalRead(boton);  //Lectura del botón para ver si es presionado.
      //TRANSICIONES MANUALES DEL ESTADO 0 = Inicio:
      if(transicionEstado == 1){
        estadoPresente = 1; //Transición manual al Estado 1 cuando se presione el botón.
        tiempoDeEsperaFSM = millis(); //Iniciación del temporizador del Estado 1.
      } 
    }
  }else if(estadoPresente == 1){  //ESTADO 1 = uno:
    //SALIDAS DEL ESTADO 1 = uno:
    Serial.println("Estado 1 = uno: Se ingresó el primer 1 de la secuencia 1011.");
    digitalWrite(ledSecuencia, LOW); //Salida del Estado 1 = Led del Pin 13 Encendido.
    
    //DELAY ANTIRREBOTES DE 150 MS Y ACTUALIZACIÓN DEL INTERVALO DE TIEMPO PARA EL ANTIRREBOTES DEL ESTADO 2:
    tiempoFSM = millis();   //Actualización del tiempo de inicio del Estado 1 = uno y para el antirrebotes.
    /*Con esta instrucción se indica que cuando la máquina de estados haya pasado más de cierto tiempo en 
    este Estado, automáticamente se pase al Estado siguiente.*/
    //TRANSICIONES AUTOMÁTICAS DEL ESTADO 1 = uno:
    if(tiempoFSM - tiempoDeEsperaFSM > intervaloDuracionEstados){
      estadoPresente = 2;   //Transición automática al Estado 2 cuando se haya pasado un intervalo de tiempo.
      tiempoDeEsperaFSM = millis(); //Iniciación del temporizador del Estado 2.
    }
  }else if(estadoPresente == 2){  //ESTADO 2 = unoCero:
    //SALIDAS DEL ESTADO 2 = unoCero:
    Serial.println("Estado 2 = unoCero: Se ha ingresado la parte 10 de la secuencia 1011.");
    digitalWrite(ledSecuencia, LOW); //Salida del Estado 2 = Led del Pin 13 Apagado.

    //DELAY ANTIRREBOTES DE 150 MS Y ACTUALIZACIÓN DEL INTERVALO DE TIEMPO PARA EL ANTIRREBOTES DEL ESTADO 3:
    tiempoFSM = millis();   //Actualización del tiempo de inicio del Estado 2 = unoCero y para el antirrebotes.
    /*Con esta instrucción se crea el código antirrebotes, que no deja recopilar datos después de haber 
    presionado el botón una vez para no captar falsas pulsaciones.*/
    if(tiempoFSM - tiempoDeEsperaFSM > delayAntirrebotes){
      transicionEstado = digitalRead(boton);  //Lectura del botón para ver si es presionado.
      //TRANSICIONES MANUALES DEL ESTADO 2 = unoCero:
      if(transicionEstado == 1){
        estadoPresente = 3; //Transición manual al Estado 3 cuando se presione el botón.
        tiempoDeEsperaFSM = millis(); //Iniciación del temporizador del Estado 3.
      } 
    }
  }else if(estadoPresente == 3){  //ESTADO 3 = unoCeroUno:
    //SALIDAS DEL ESTADO 3 = unoCeroUno:
    Serial.println("Estado 3 = unoCeroUno: Se ha ingresado la parte 101 de la secuencia 1011.");
    digitalWrite(ledSecuencia, LOW); //Salida del Estado 3 = Led del Pin 13 Apagado.

    //DELAY ANTIRREBOTES DE 150 MS Y ACTUALIZACIÓN DEL INTERVALO DE TIEMPO PARA EL TEMPORIZADOR Y ANTIRREBOTES DEL ESTADO 4:
    tiempoFSM = millis();   //Actualización del tiempo de inicio del Estado 3 = unoCeroUno y para el antirrebotes.
    /*Con esta instrucción se crea el código antirrebotes, que no deja recopilar datos después de haber 
    presionado el botón una vez para no captar falsas pulsaciones.*/
    if(tiempoFSM - tiempoDeEsperaFSM > delayAntirrebotes){
      transicionEstado = digitalRead(boton);  //Lectura del botón para ver si es presionado.
      //TRANSICIONES MANUALES DEL ESTADO 3 = unoCeroUno:
      if(transicionEstado == 1){
        estadoPresente = 4; //Transición manual al Estado 4 cuando se presione el botón.
        tiempoDeEsperaFSM = millis(); //Iniciación del temporizador del Estado 4.
      } 
    }    
  }else if(estadoPresente == 4){  //ESTADO 4 = unoCeroUnoUno:
    //SALIDAS DEL ESTADO 4 = unoCeroUnoUno:
    Serial.println("Estado 4 = unoCeroUnoUno: Se ha ingresado la secuencia completa 1011.");
    digitalWrite(ledSecuencia, HIGH); //Salida del Estado 4 = Led del Pin 13 Encendido.

    //DELAY ANTIRREBOTES DE 150 MS Y ACTUALIZACIÓN DEL INTERVALO DE TIEMPO PARA EL ANTIRREBOTES DEL ESTADO 0:
    tiempoFSM = millis();
    /*Con esta instrucción se crea el código antirrebotes, que no deja recopilar datos después de haber 
    presionado el botón una vez para no captar falsas pulsaciones.*/
    if(tiempoFSM - tiempoDeEsperaFSM > delayAntirrebotes){
      transicionEstado = digitalRead(boton);  //Lectura del botón para ver si es presionado.
      //TRANSICIONES MANUALES O AUTOMÁTICAS DEL ESTADO 4 = unoCeroUnoUno:
      /*Con esta instrucción se indica que cuando la máquina de estados haya pasado más de cierto tiempo en 
      este Estado, automáticamente se pase al Estado Inicial o además que puede hacer la transición si el 
      botón es presionado.*/
      if((transicionEstado == 1) or (tiempoFSM - tiempoDeEsperaFSM > intervaloDuracionEstados)){
        //Transición manual al Estado 0 cuando se presione el botón o de forma automática cuando pase cierto tiempo.
        estadoPresente = 0;
      } 
    }
  }else{
    Serial.println("Estado no disponible");
  }
}
