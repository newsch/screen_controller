/*
  Da-Lite Low Voltage Screen Controller
  Creates a pseudo lower limit for the screen based on potentiometer value
*/

unsigned long current_time;
unsigned long start_time;  // time screen started lowering
int desired_time;  // desired time spent lowering
int total_time;  // time spent lowering in ms

bool is_lowering = false;
bool is_rising = false;

int pot_min = 0, pot_max = 255;
int time_min = 0, time_max = 5000;

void setup() {
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

  // TODO: if up button pressed
    raise_screen();
  
  // TODO: if down button pressed
    lower_screen();
  
  // TODO: stop screen if either button pressed and raising/lowering
    stop_screen();
}

void lower_screen() {
  is_lowering = true;
  start_time = millis();
  // TODO: lower screen
}

void raise_screen() {
  is_rising = true;
  start_time = millis();
  // TODO: raise screen
}

void stop_screen() {
  // TODO: stop screen
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
  desired_time = map(pot_val, pot_min, pot_max, time_min, time_max);
  desired_time = 2400;  // DEBUG
}
