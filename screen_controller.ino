/*
  Da-Lite Low Voltage Screen Controller
  Creates a pseudo lower limit for the screen based on potentiometer value
*/

#define DEBUG

// pins
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

// bedouncing
unsigned long last_up_bedounce = 0;
unsigned long last_dn_bedounce = 0;
unsigned long bedounce_delay = 100;
bool up_state = false;
bool dn_state = false;
bool last_up_reading = false;
bool last_dn_reading = false;
bool last_up_state = false;
bool last_dn_state = false;


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
    Serial.println("PROGRAM STARTING");
  #endif
}

void loop() {
  // bedouncing/input
  bool up_reading = !digitalRead(UP_IN);  // buttons are normally closed
  bool dn_reading = !digitalRead(DN_IN);
  // up_state = !digitalRead(UP_IN);  // buttons are normally closed
  // dn_state = !digitalRead(DN_IN);

  // reset bedouncing timers
  if (up_reading != last_up_reading) {
    last_up_bedounce = millis();
  }
  if (dn_reading != last_dn_reading) {
    last_dn_bedounce = millis();
  }

  if ((millis() - last_up_bedounce) > bedounce_delay) {
    up_state = up_reading;

  }
  if ((millis() - last_dn_bedounce) > bedounce_delay) {
    dn_state = dn_reading;
  }
  last_up_reading = up_reading;
  last_dn_reading = dn_reading;

  update_desired_time();
  update_total_time();

  // main control flow
  if (at_lower_limit() && is_lowering) {
    #ifdef DEBUG
    Serial.println("BOTTOM\t" + String(total_time));
    #endif
    stop_screen();
  } else if (total_time <= 0 && is_rising) {
    is_rising = false;
    // start_time = millis();
    // update_total_time();
    total_time = 0;  // reset timer
    #ifdef DEBUG
    Serial.println("TOP\t" + String(total_time));
    #endif
  }
  if ((!last_up_state && up_state) || (!last_dn_state && dn_state)) {
    if ((is_lowering || is_rising) && (up_state || dn_state)) {  // stop screen if either button pressed when raising/lowering
      stop_screen();
    } else if (up_state && !is_rising) {
      raise_screen();
    } else if (dn_state && !is_lowering && !at_lower_limit()) {
      lower_screen();
    }
  }
  // update states
  last_up_state = up_state;
  last_dn_state = dn_state;
}


void lower_screen() {
  is_rising = false;
  is_lowering = true;
  start_time = millis();
  // lower screen
  digitalWrite(DN_OUT, LOW);
  delay(200);
  digitalWrite(DN_OUT, HIGH);
  #ifdef DEBUG
  Serial.println("LOWER\t" + String(total_time));
  #endif
}

void raise_screen() {
  is_lowering = false;
  is_rising = true;
  start_time = millis();
  // raise screen
  digitalWrite(UP_OUT, LOW);
  delay(200);
  digitalWrite(UP_OUT, HIGH);
  #ifdef DEBUG
  Serial.println("RAISE\t" + String(total_time));
  #endif
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

  #ifdef DEBUG
  Serial.println("STOP\t" + String(total_time));
  #endif
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
