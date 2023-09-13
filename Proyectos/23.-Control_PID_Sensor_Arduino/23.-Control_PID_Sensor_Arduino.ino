#include <OneWire.h>
#include <DallasTemperature.h>
const int pinDataDQ = 8;
OneWire oneWireObject(pinDataDQ);
DallasTemperature sensorDS18B20(&oneWireObject);
float temperature;            //Valor de temperatura leido.
const int digiPin=7;          //Salida al SSR.
int period;                   //Periodo del controlador.
float set_point;              //Set point.
float tT;                     //Tiempo total de operación del controlador.
float ti;                     //Tiempo integral.
float td;                     //Tiempo diferencial.
float u_lim;                  //Cota máxima de temperatura.
float in_lim;                 //Cota inferior de temperatura.
float kp;                     //Constante propocional.
float u_max;                  //Valor maximo de la funcion u(t).
float int_tk;                 //valor de la integral numerica.
float tk;                     //Tiempo k-ésimo.
float error=0;                //Error del controlador.
float u_pid=0;                //Valor de la funcion del controlador u(t).
float der_tk;                 //Valor relacionado al término derivativo del controlador.
float past_temp=0;            //Valor previo de temperatura.
String in_param="";           //String de los parametros del controlador que envia la PC.
bool flag_init_param = false;
bool flag_start_PID = false;
bool flag_stop_serial = false;
int tH;                       //Tiempo en alto del controlador.
int n = 0;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(digiPin, OUTPUT);
  Serial.begin(19200);
}

void loop() {
  if (Serial.available() > 0 && flag_stop_serial == false){
    in_param=Serial.readString();
    while(in_param.length() == 25){
      period = in_param.substring(0,4).toInt();
      set_point = in_param.substring(5,7).toFloat();
      tT=in_param.substring(8,12).toFloat();
      ti=in_param.substring(13,17).toFloat();
      td=in_param.substring(18,23).toFloat();
      flag_init_param = true;
      flag_stop_serial = true;
      break;
    }//while(in_param.length()==25)
  }//if (Serial.available()>0 && flag_stop_serial==false){
  
  while(flag_init_param == true){
    u_lim = 1.05*set_point;   //5% arriba del set point
    in_lim = 0.85*set_point;  //855 abajo del set point
    kp = (100.0/(u_lim-in_lim));
    u_max = kp*set_point;
    flag_start_PID = true;
    flag_init_param = false;
    tk = 0;
    delay(100);
  }//while(flag_init_param==true)

  if (tk<tT && flag_start_PID == true){
    flag_init_param = false;
    sensorDS18B20.requestTemperatures();
    temperature=sensorDS18B20.getTempCByIndex(0);
    delay(500);
    error = set_point-temperature;
    int_tk = int_tk+(period/1000.0)*error;
    der_tk = (temperature-past_temp)/(period/1000.0);
    u_pid = kp*(error+(1/ti)*int_tk+td*der_tk);
    past_temp = temperature;
    tk = tk+(period/1000.0);
    Serial.print(tk);
    Serial.print(",");
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(u_pid);
    Serial.print(",");
    pulse_width(u_pid,u_max);
    delay(50);
  }//if (tk<tT && flag_start_PID==true)
  
  if(tk > tT){
    tT = 0;
    tk = 0;
    error = 0;
    u_pid = 0;
    int_tk = 0;
    der_tk = 0;
    past_temp = 0;
    in_param = "";
    flag_init_param = false;
    flag_start_PID = false;
    flag_stop_serial = false;
    tH = 0;
    setup();
  }//if(tk>tT)

  //La operación módulo ve si el número n es par.
  if(n%2 == 0){
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN,LOW);
  }
  if(n == 100){
    n = 0;
  }
  n++;
}//loop

void pulse_width(float u_pid, float u_max){
  if(u_pid <= u_max && u_pid >= 0){
    tH = (int)(period*(u_pid/u_max));
  }
  if(u_pid > u_max){
    tH = period;
  }
  if(u_pid < 0){
    tH = 0;
  }
  Serial.print(tH);
  Serial.print(",");
  Serial.print((int)(period-tH));
  Serial.println();
  Serial.flush();
  digitalWrite(digiPin,HIGH);
  delay((int)tH);
  digitalWrite(digiPin,LOW);
  delay((int)period-tH);
}//pulse_width
