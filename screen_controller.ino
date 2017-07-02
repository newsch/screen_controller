/*
  Da-Lite Low Voltage Screen Controller
  Creates a pseudo lower limit for the screen based on potentiometer value
*/

// pins
#define UP_IN 0
#define DN_IN 1
#define UP_OUT 4
#define DN_OUT 3
#define POT_IN 1

// bedouncing
unsigned long last_up_bedounce = 0;
unsigned long last_dn_bedounce = 0;
int bedounce_delay = 100;
int up_state = LOW;
int dn_state = LOW;
int last_up_state = HIGH;
int last_dn_state = HIGH;

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
}

void loop() {
  update_desired_time();
  update_total_time();

  if (is_lowering && at_lower_limit()) {  // if screen reaches lower limit
    stop_screen();
  }
  // else if (is_rising && total_time <= 0) {  // if screen reaches upper limit
  //   stop_screen();
  // }

  // bedouncing/input
  int up_reading = !digitalRead(UP_IN);  // buttons are normally closed
  int dn_reading = !digitalRead(DN_IN);

  // reset bedouncing timers
  if (up_reading != last_up_state) {
    last_up_bedounce = millis();
  }
  if (dn_reading != last_dn_state) {
    last_dn_bedounce = millis();
  }

  if ((millis() - last_up_bedounce) > bedounce_delay) {
    if (up_reading != up_state) {
      last_up_state = up_state;
      up_state = up_reading;
    }
  }
  if ((millis() - last_dn_bedounce) > bedounce_delay) {
    if (dn_reading != dn_state) {
      last_dn_state = dn_state;
      dn_state = dn_reading;
    }
  }


  if (up_state && is_rising) {
    raise_screen();
  } else if (dn_state && !is_lowering) {
    lower_screen();
  } else if ((is_lowering || is_rising) && (up_state || dn_state)){  // stop screen if either button pressed and raising/lowering
    stop_screen();
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
