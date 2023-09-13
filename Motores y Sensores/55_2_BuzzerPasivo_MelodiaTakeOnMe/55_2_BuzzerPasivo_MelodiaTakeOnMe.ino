/*55.2.-Buzzer Pasivo: Este tipo de Buzzer recibe una señal, cuya frecuencia indicará a que tono debe de 
sonar, por lo cual se le pueden programar melodías o secuencias de sonido. En este caso sonará la melodía
de Take on Me, además de este enlace se pueden obtener más secuencias de melodías conocidas:
https://github.com/robsoncouto/arduino-songs/tree/master.*/

/*#define: Instrucción utilizada para crear constantes globales en el programa de Arduino siguiendo la 
sintaxis descrita a continuación:
  #define nombreConstante valorConstante
En este caso las constantes indican todas las notas musicales que pueden sonar en el buzzer pasivo para 
crear una melodía simulando como si se tuviera un piano de 88 teclas, que es el más común y contiene casi 
todas las frecuencias de las 8 octavas musicales. Una octava es un intervalo musical que abarca ocho notas 
consecutivas, esto incluye las notas naturales do, re, mi, fa, sol, la, si y sus correspondientes 
sostenidos o bemoles, donde la frecuencia de una nota en una octava superior es del doble en su equivalente
de la misma nota pero en una octava inferior. Cuanto mayor sea la frecuencia de una onda sonora, más agudo 
será el sonido percibido y viceversa.*/
//OCTAVA CERO: Sonidos más graves.
#define NOTE_La0   27    //1.-A0:   Nota La   indicada en 27 Hz.
#define NOTE_LaS0  29    //2.-AS0:  Nota La Sostenido/Si Bemol indicada en 29 Hz.
#define NOTE_Si0   31    //3.-B0:   Nota Si   indicada en 31 Hz.
//PRIMERA OCTAVA:
#define NOTE_Do1   33    //4.-C1:   Nota Do   indicada en 33 Hz.
#define NOTE_DoS1  35    //5.-CS1:  Nota Do Sostenido/Re Bemol indicada en 35 Hz.
#define NOTE_Re1   37    //6.-D1:   Nota Re   indicada en 37 Hz.
#define NOTE_ReS1  39    //7.-DS1:  Nota Re Sostenido/Mi Bemol indicada en 39 Hz.
#define NOTE_Mi1   41    //8.-E1:   Nota Mi   indicada en 41 Hz.
#define NOTE_Fa1   44    //9.-F1:   Nota Fa   indicada en 44 Hz.
#define NOTE_FaS1  46    //10.-FS1: Nota Fa Sostenido/Sol Bemol indicada en 46 Hz.
#define NOTE_Sol1  49    //11.-G1:  Nota Sol  indicada en 49 Hz.
#define NOTE_SolS1 52    //12.-GS1: Nota Sol Sostenido/La Bemol indicada en 52 Hz.
#define NOTE_La1   55    //13.-A1:  Nota La   indicada en 55 Hz.
#define NOTE_LaS1  58    //14.-AS1: Nota La Sostenido/Si Bemol indicada en 58 Hz.
#define NOTE_Si1   62    //15.-B1:  Nota Si   indicada en 62 Hz.
//SEGUNDA OCTAVA:
#define NOTE_Do2   65    //16.-C2:  Nota Do   indicada en 65 Hz.
#define NOTE_DoS2  69    //17.-CS2: Nota Do Sostenido/Re Bemol indicada en 69 Hz.
#define NOTE_Re2   73    //18.-D2:  Nota Re   indicada en 73 Hz.
#define NOTE_ReS2  78    //19.-DS2: Nota Re Sostenido/Mi Bemol indicada en 78 Hz.
#define NOTE_Mi2   82    //20.-E2:  Nota Mi   indicada en 82 Hz.
#define NOTE_Fa2   87    //21.-F2:  Nota Fa   indicada en 87 Hz.
#define NOTE_FaS2  93    //22.-FS2: Nota Fa Sostenido/Sol Bemol indicada en 93 Hz.
#define NOTE_Sol2  98    //23.-G2:  Nota Sol  indicada en 98 Hz.
#define NOTE_SolS2 104   //24.-GS2: Nota Sol Sostenido/La Bemol indicada en 104 Hz.
#define NOTE_La2   110   //25.-A2:  Nota La   indicada en 110 Hz.
#define NOTE_LaS2  117   //26.-AS2: Nota La Sostenido/Si Bemol indicada en 117 Hz.
#define NOTE_Si2   123   //27.-B2:  Nota Si   indicada en 123 Hz.
//TERCERA OCTAVA:
#define NOTE_Do3   131   //28.-C3:  Nota Do   indicada en 131 Hz.
#define NOTE_DoS3  139   //29.-CS3: Nota Do Sostenido/Re Bemol indicada en 139 Hz.
#define NOTE_Re3   147   //30.-D3:  Nota Re   indicada en 147 Hz.
#define NOTE_ReS3  156   //31.-DS3: Nota Re Sostenido/Mi Bemol indicada en 156 Hz.
#define NOTE_Mi3   165   //32.-E3:  Nota Mi   indicada en 165 Hz.
#define NOTE_Fa3   175   //33.-F3:  Nota Fa   indicada en 175 Hz.
#define NOTE_FaS3  185   //34.-FS3: Nota Fa Sostenido/Sol Bemol indicada en 185 Hz.
#define NOTE_Sol3  196   //35.-G3:  Nota Sol  indicada en 196 Hz.
#define NOTE_SolS3 208   //36.-GS3: Nota Sol Sostenido/La Bemol indicada en 208 Hz.
#define NOTE_La3   220   //37.-A3:  Nota La   indicada en 220 Hz.
#define NOTE_LaS3  233   //38.-AS3: Nota La Sostenido/Si Bemol indicada en 233 Hz.
#define NOTE_Si3   247   //39.-B3:  Nota Si   indicada en 247 Hz.
//CUARTA OCTAVA:
#define NOTE_Do4   262   //40.-C4:  Nota Do   indicada en 262 Hz.
#define NOTE_DoS4  277   //41.-CS4: Nota Do Sostenido/Re Bemol indicada en 277 Hz.
#define NOTE_Re4   294   //42.-D4:  Nota Re   indicada en 294 Hz.
#define NOTE_ReS4  311   //43.-DS4: Nota Re Sostenido/Mi Bemol indicada en 311 Hz.
#define NOTE_Mi4   330   //44.-E4:  Nota Mi   indicada en 330 Hz.
#define NOTE_Fa4   349   //45.-F4:  Nota Fa   indicada en 349 Hz.
#define NOTE_FaS4  370   //46.-FS4: Nota Fa Sostenido/Sol Bemol indicada en 370 Hz.
#define NOTE_Sol4  392   //47.-G4:  Nota Sol  indicada en 392 Hz.
#define NOTE_SolS4 415   //48.-GS4: Nota Sol Sostenido/La Bemol indicada en 415 Hz.
#define NOTE_La4   440   //49.-A4:  Nota La   indicada en 440 Hz.
#define NOTE_LaS4  466   //50.-AS4: Nota La Sostenido/Si Bemol indicada en 466 Hz.
#define NOTE_Si4   494   //51.-B4:  Nota Si   indicada en 494 Hz.
//QUINTA OCTAVA:
#define NOTE_Do5   523   //52.-C5:  Nota Do   indicada en 523 Hz.
#define NOTE_DoS5  554   //53.-CS5: Nota Do Sostenido/Re Bemol indicada en 554 Hz.
#define NOTE_Re5   587   //54.-D5:  Nota Re   indicada en 587 Hz.
#define NOTE_ReS5  622   //55.-DS5: Nota Re Sostenido/Mi Bemol indicada en 622 Hz.
#define NOTE_Mi5   659   //56.-E5:  Nota Mi   indicada en 659 Hz.
#define NOTE_Fa5   698   //57.-F5:  Nota Fa   indicada en 698 Hz.
#define NOTE_FaS5  740   //58.-FS5: Nota Fa Sostenido/Sol Bemol indicada en 740 Hz.
#define NOTE_Sol5  784   //59.-G5:  Nota Sol  indicada en 784 Hz.
#define NOTE_SolS5 831   //60.-GS5: Nota Sol Sostenido/La Bemol indicada en 831 Hz.
#define NOTE_La5   880   //61.-A5:  Nota La   indicada en 880 Hz.
#define NOTE_LaS5  932   //62.-AS5: Nota La Sostenido/Si Bemol indicada en 932 Hz.
#define NOTE_Si5   988   //63.-B5:  Nota Si   indicada en 988 Hz.
//SEXTA OCTAVA:
#define NOTE_Do6   1047  //64.-C6:  Nota Do   indicada en 1047 Hz.
#define NOTE_DoS6  1109  //65.-CS6: Nota Do Sostenido/Re Bemol indicada en 1109 Hz.
#define NOTE_Re6   1175  //66.-D6:  Nota Re   indicada en 1175 Hz.
#define NOTE_ReS6  1245  //67.-DS6: Nota Re Sostenido/Mi Bemol indicada en 1245 Hz.
#define NOTE_Mi6   1319  //68.-E6:  Nota Mi   indicada en 1319 Hz.
#define NOTE_Fa6   1397  //69.-F6:  Nota Fa   indicada en 1397 Hz.
#define NOTE_FaS6  1480  //70.-FS6: Nota Fa Sostenido/Sol Bemol indicada en 1480 Hz.
#define NOTE_Sol6  1568  //71.-G6:  Nota Sol  indicada en 1568 Hz.
#define NOTE_SolS6 1661  //72.-GS6: Nota Sol Sostenido/La Bemol indicada en 1661 Hz.
#define NOTE_La6   1760  //73.-A6:  Nota La   indicada en 1760 Hz.
#define NOTE_LaS6  1865  //74.-AS6: Nota La Sostenido/Si Bemol indicada en 1865 Hz.
#define NOTE_Si6   1976  //75.-B6:  Nota Si   indicada en 1976 Hz.
//SÉPTIMA OCTAVA:
#define NOTE_Do7   2093  //76.-C7:  Nota Do   indicada en 2093 Hz.
#define NOTE_DoS7  2217  //77.-CS7: Nota Do Sostenido/Re Bemol indicada en 2217 Hz.
#define NOTE_Re7   2349  //78.-D7:  Nota Re   indicada en 2349 Hz.
#define NOTE_ReS7  2489  //79.-DS7: Nota Re Sostenido/Mi Bemol indicada en 2489 Hz.
#define NOTE_Mi7   2637  //80.-E7:  Nota Mi   indicada en 2637 Hz.
#define NOTE_Fa7   2794  //81.-F7:  Nota Fa   indicada en 2794 Hz.
#define NOTE_FaS7  2960  //82.-FS7: Nota Fa Sostenido/Sol Bemol indicada en 2960 Hz.
#define NOTE_Sol7  3136  //83.-G7:  Nota Sol  indicada en 3136 Hz.
#define NOTE_SolS7 3322  //84.-GS7: Nota Sol Sostenido/La Bemol indicada en 3322 Hz.
#define NOTE_La7   3520  //85.-A7:  Nota La   indicada en 3520 Hz.
#define NOTE_LaS7  3729  //86.-AS7: Nota La Sostenido/Si Bemol indicada en 3729 Hz.
#define NOTE_Si7   3951  //87.-B7:  Nota Si   indicada en 3951 Hz.
//OCTAVA OCTAVA: Sonidos más agudos.
#define NOTE_Do8   4186  //88.-C8:  Nota Do   indicada en 4186 Hz.
#define REST       0
/*Pin digital del Buzzer Pasivo = 8*/
#define BUZZER_PASIVO 8

//CANCIÓN DE TAKE ON ME:
int melodia [] = {
  NOTE_FaS5, NOTE_FaS5, NOTE_Re5, NOTE_Si4, REST, NOTE_Si4, REST, NOTE_Mi5, 
  REST, NOTE_Mi5, REST, NOTE_Mi5, NOTE_SolS5, NOTE_SolS5, NOTE_La5, NOTE_Si5,
  NOTE_La5, NOTE_La5, NOTE_La5, NOTE_Mi5, REST, NOTE_Re5, REST, NOTE_FaS5, 
  REST, NOTE_FaS5, REST, NOTE_FaS5, NOTE_Mi5, NOTE_Mi5, NOTE_FaS5, NOTE_Mi5, 
  NOTE_FaS5, NOTE_FaS5, NOTE_Re5, NOTE_Si4, REST, NOTE_Si4, REST, NOTE_Mi5, 

  REST, NOTE_Mi5, REST, NOTE_Mi5, NOTE_SolS5, NOTE_SolS5, NOTE_La5, NOTE_Si5,
  NOTE_La5, NOTE_La5, NOTE_La5, NOTE_Mi5, REST, NOTE_Re5, REST, NOTE_FaS5, 
  REST, NOTE_FaS5, REST, NOTE_FaS5, NOTE_Mi5, NOTE_Mi5, NOTE_FaS5, NOTE_Mi5,
  NOTE_FaS5, NOTE_FaS5, NOTE_Re5, NOTE_Si4, REST, NOTE_Si4, REST, NOTE_Mi5, 
  REST, NOTE_Mi5, REST, NOTE_Mi5, NOTE_SolS5, NOTE_SolS5, NOTE_La5, NOTE_Si5,

  NOTE_La5, NOTE_La5, NOTE_La5, NOTE_Mi5, REST, NOTE_Re5, REST, NOTE_FaS5, 
  REST, NOTE_FaS5, REST, NOTE_FaS5, NOTE_Mi5, NOTE_Mi5, NOTE_FaS5, NOTE_Mi5 
};
//Las duraciones de las notas se realiza al dividir un segundo en fracciones, osea 1/4, 1/8, etc.
int duraciones [] = {
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8
};

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
  pinMode(BUZZER_PASIVO, OUTPUT); //La constante BUZZER_PASIVO asignada al pin digital 8 es una salida.
}

//EJECUCIÓN DEL PROGRAMA EN UN BUCLE INFINITO
void loop() {
  for(int i = 0; i < 84; i++){
    int duracion = 1000/duraciones[i]; //Operación para indicar todas las duraciones en segundos.
    /*tone(): Método que permite mandar una señal con una frecuencia específica, que además dure un 
    tiempo determinado, recibe como parámetros lo siguiente:
      - Primer parámetro: Pin, este será cualquier pin digital.
      - Segundo parámetro: Frecuencia, indicada en Hertz (ciclos/segundo).
      - Tercer parámetro: Duración, indicada en milisegundos.*/ 
    tone(BUZZER_PASIVO, melodia[i], duracion);
    int pausa = duracion * 1.5;
    /*delay(ms): Método que detiene la ejecución del programa un cierto tiempo dado en milisegundos.*/
    delay(pausa);
    /*noTone(): Método que permite detener la señal mandada a un pin específico previamente mandada a 
    través del método tone():
      - Primer parámetro: Pin, este será cualquier pin digital al que previamente
        se haya aplicado el método tone().*/ 
    noTone(BUZZER_PASIVO);
  }
  delay(10000);
}
