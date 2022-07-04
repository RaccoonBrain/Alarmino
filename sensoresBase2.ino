
int PIR = 2;
int LED = 3;
int ESTADO = 0;
int GAS = A0;
int LED2 = 13;
int ESTADO2 = 0;
int SONIDO = 5;
int LED3 = 7;
int ESTADO3 = 0;
int ENVIAR = 0;



void setup() {
  pinMode(PIR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(GAS, INPUT);
  pinMode(LED2, OUTPUT);
  pinMode(SONIDO, INPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3, LOW);
  Serial.begin(9600);
  delay(100);
}

void loop() {
  ESTADO = digitalRead(PIR);
  if (ESTADO == HIGH) {
    ENVIAR = 1;
    digitalWrite(LED, HIGH);
    delay(100);
  } else {
    digitalWrite(LED, LOW);
  }

  ESTADO2 = analogRead(GAS);
  if (ESTADO2 > 420) {
    ENVIAR = 1;
    digitalWrite(LED2, HIGH);
    delay(100);
  } else {
    digitalWrite(LED2, LOW);
  }

  ESTADO3 = digitalRead(SONIDO);
  if (ESTADO3 == HIGH) {
    ENVIAR = 1;
    digitalWrite(LED3, HIGH);
    delay(10);
  } else {
    digitalWrite(LED3, LOW);
  }

  if (ENVIAR == 1){
    SendData();
    ENVIAR = 0;
  }
  delay (500);
}

void SendData(){
  String sonido=String(ESTADO3);
  String gas=String(ESTADO2);
  String movimiento=String(ESTADO);
  Serial.println(sonido + "_" + gas + "_" + movimiento);

}
