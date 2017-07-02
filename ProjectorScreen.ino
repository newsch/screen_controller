#define UP_IN 0
#define DN_IN 1
#define UP_OUT 4
#define DN_OUT 3

void setup() {
  pinMode(UP_IN, INPUT);
  pinMode(DN_IN, INPUT);
  pinMode(UP_OUT, OUTPUT);
  pinMode(DN_OUT, OUTPUT);
  digitalWrite(UP_OUT, HIGH);
  digitalWrite(DN_OUT, HIGH);
  

}

void loop() {
  if(!digitalRead(UP_IN)){
    digitalWrite(UP_OUT, LOW);
    delay(200);
    digitalWrite(UP_OUT, HIGH);
  }

  if(!digitalRead(DN_IN)){
    digitalWrite(DN_OUT, LOW);
    delay(200);
    digitalWrite(DN_OUT, HIGH);
  }

}
