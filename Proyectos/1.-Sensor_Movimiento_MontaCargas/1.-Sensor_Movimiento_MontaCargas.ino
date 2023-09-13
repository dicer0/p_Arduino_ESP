boolean estadoParpadeo = true;
int tiempo = 0;
unsigned long tiempoDelay = 300;
int estadoLED = LOW;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), manejadorPulsador, RISING);
  delay(50);
  attachInterrupt(digitalPinToInterrupt(3), manejadorSensor, CHANGE);
  digitalWrite(13,estadoLED);
  Serial.begin(9600);
}

void manejadorPulsador(){
  digitalWrite(13,LOW);
  Serial.println("3: changos");
  estadoParpadeo = false;
  delay(1000);
}

void manejadorSensor(){
  digitalWrite(13,LOW);
  delay(1000);
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  delay(1000);
  digitalWrite(13,HIGH);
  delay(1000);
}

void loop() {
  estadoLED = LOW;
  unsigned long tiempoActual = millis();
  if(tiempo != 0){
    tiempo++;
    Serial.print("1: Tiempo sin sensar actualizado");
  }else{
    estadoLED = LOW;
  }
  
  Serial.println(tiempo);
  
  if(tiempo > tiempoDelay){
    digitalWrite(13,LOW);
    estadoParpadeo = false;
    delay(1000);
    tiempo = 0;
    if(tiempoDelay%tiempoActual == 1){
      tiempo = 0;
    }else{
      tiempo++;
    }
    Serial.print("2: Tiempo reiniciado sin censar");
    Serial.println(tiempo);
  }
  
  if(estadoParpadeo == false){
    delay(5000);
    estadoParpadeo = true;
    Serial.print("3: Estado ");
    Serial.println(estadoParpadeo);
  }
  
  if(digitalRead(3) == HIGH){
    if(tiempoDelay%tiempoActual == 1){
      tiempo = 0;
    }else{
      tiempo++;
      Serial.print("3: Tiempo sensado ");
      Serial.println(tiempo);
    }
    
    if(tiempo <= tiempoDelay && estadoParpadeo == true){
      Serial.print("4: Tiempo actual ");
      Serial.println(tiempo);
      estadoLED = HIGH;
    }else if(tiempo>tiempoDelay && estadoParpadeo == true){
      tiempo = 0;
      digitalWrite(13,LOW);
      delay(1000);
      if(tiempoDelay%tiempoActual == 1){
        tiempo = 0;
      }else{
        tiempo++;
      }
      Serial.print("5: Tiempo reiniciado ");
      Serial.println(tiempo);
      estadoParpadeo = false;
    }else{
      digitalWrite(13,LOW);
      estadoParpadeo = false;
    }
    digitalWrite(13,estadoLED);
  }
} 
