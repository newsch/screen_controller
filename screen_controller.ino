/*
  Da-Lite Low Voltage Screen Controller
  Creates a pseudo lower limit for the screen based on potentiometer value
*/

// pins
#define UP_IN 0
#define DN_IN 1
#define UP_OUT 4
#define DN_OUT 3

// ranges for pot to time mapping
#define POT_MIN 0
#define POT_MAX = 255
#define TIME_MIN = 0
#define TIME_MAX = 5000

// time
unsigned long current_time;
unsigned long start_time;  // time screen started lowering
int desired_time;  // desired time spent lowering
int total_time;  // time spent lowering in ms

// flags
bool is_lowering = false;
bool is_rising = false;

void setup() {
  pinMode(UP_IN, INPUT);
  pinMode(DN_IN, INPUT);
  pinMode(UP_OUT, OUTPUT);
  pinMode(DN_OUT, OUTPUT);
  digitalWrite(UP_OUT, HIGH);
  digitalWrite(DN_OUT, HIGH);

  update_desired_time();
  total_time = 0;
}

void loop() {
  update_desired_time();
  update_total_time();

  if (is_lowering && total_time >= desired_time) {  // if screen reaches lower limit
    stop_screen();
  } else if (is_rising && total_time <= 0) {  // if screen reaches upper limit
    stop_screen();
  }

  if (!digitalRead(UP_IN)) raise_screen();

  if (!digitalRead(DN_IN)) lower_screen();

  if ((is_lowering || is_rising) && (!digitalRead(UP_IN) || !digitalRead(DN_IN))
  {  // stop screen if either button pressed and raising/lowering
    stop_screen();
  }
}

void lower_screen() {
  is_lowering = true;
  start_time = millis();
  // lower screen
  digitalWrite(DN_OUT, LOW);
  delay(200);
  digitalWrite(DN_OUT, HIGH);
}

void raise_screen() {
  is_rising = true;
  start_time = millis();
  // raise screen
  digitalWrite(UP_OUT, LOW);
  delay(200);
  digitalWrite(UP_OUT, HIGH);
}

void stop_screen() {
  // stop screen by pressing both
  digitalWrite(UP_OUT, LOW);
  digitalWrite(DN_OUT, LOW);
  delay(200);
  digitalWrite(UP_OUT, HIGH);
  digitalWrite(DN_OUT, HIGH);

  update_total_time();
  is_lowering = false;
  is_rising = false;
}

void update_total_time(){
  current_time = millis();
  if (is_rising) {  // update current time
    total_time -= current_time - start_time;
  } else if (is_lowering) {
    total_time += current_time - start_time;
  }
}

void update_desired_time() {
  // TODO: read from pot, map to desired_time
  int pot_val = 69;
  desired_time = map(pot_val, POT_MIN, POT_MAX, TIME_MIN, TIME_MAX);
  desired_time = 2400;  // DEBUG
}
