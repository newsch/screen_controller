/*
  Da-Lite Low Voltage Screen Controller
  Creates a pseudo lower limit for the screen based on potentiometer value
*/

// #define DEBUG

#ifdef DEBUG
  #define UP_IN 4
  #define DN_IN 3
  #define UP_OUT 12
  #define DN_OUT 13
  #define POT_IN 1
#else
  #define UP_IN 0
  #define DN_IN 1
  #define UP_OUT 4
  #define DN_OUT 3
  #define POT_IN 1
#endif
// pins
#define UP_IN 4
#define DN_IN 3
#define UP_OUT 12
#define DN_OUT 13
#define POT_IN 1

// bedouncing
unsigned long last_up_bedounce = 0;
unsigned long last_dn_bedounce = 0;
unsigned long bedounce_delay = 50;
int up_state;
int dn_state;
int last_up_state = false;
int last_dn_state = false;

// ranges for pot to time mapping
#define POT_MIN 0
#define POT_MAX 1023
#define TIME_MIN 0
#define TIME_MAX 14000

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

  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("assup my dude");
  #endif
}

void loop() {
  update_desired_time();
  update_total_time();

  // bedouncing/input
  bool up_reading = !digitalRead(UP_IN);  // buttons are normally closed
  bool dn_reading = !digitalRead(DN_IN);
  // up_state = !digitalRead(UP_IN);  // buttons are normally closed
  // dn_state = !digitalRead(DN_IN);

  // reset bedouncing timers
  if (up_reading != last_up_state) {
    last_up_bedounce = millis();
  }
  if (dn_reading != last_dn_state) {
    last_dn_bedounce = millis();
  }

  if ((millis() - last_up_bedounce) > bedounce_delay) {
    if (up_reading != up_state) {
      up_state = up_reading;
    }
  }
  if ((millis() - last_dn_bedounce) > bedounce_delay) {
    if (dn_reading != dn_state) {
      dn_state = dn_reading;
    }
  }
  last_up_state = up_reading;
  last_dn_state = dn_reading;

  if (at_lower_limit() && is_lowering) {
    stop_screen();
  } else if (is_lowering && up_state || is_rising && dn_state) {  // stop screen if either button pressed and raising/lowering
    stop_screen();
  } else if (up_state && !is_rising) {
    raise_screen();
  } else if (dn_state && !is_lowering && !at_lower_limit()) {
    lower_screen();
  }
}

void lower_screen() {
  is_rising = false;
  is_lowering = true;
  start_time = millis();
  // lower screen
  digitalWrite(DN_OUT, LOW);
  delay(200);
  digitalWrite(DN_OUT, HIGH);
}

void raise_screen() {
  is_lowering = false;
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
  if (is_rising) {  // update total time
    total_time -= current_time - start_time;
  } else if (is_lowering) {
    total_time += current_time - start_time;
  }
  start_time = current_time;
}

void update_desired_time() {
  // TODO: read from pot, map to desired_time
  int pot_val = analogRead(POT_IN);
  desired_time = map(pot_val, POT_MIN, POT_MAX, TIME_MIN, TIME_MAX);
  // desired_time = 2400;  // DEBUG
}

bool at_lower_limit() {
  return total_time >= desired_time;
}
