
// initial value for the measurements
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 0;
unsigned int respirationRateRaw = 0;
unsigned short batteryStatus = 200;

// count the execution time in each measurement. 1 means even time, -1 means odd time
int temperatureRaw_count = 1;
int systolicPressRaw_count = 1;
int diastolicPressRaw_count = 1;
int pulseRateRaw_count = 1;

// determine whether the task is complete
bool temperatureRaw_flip = true;
bool systolicPressRaw_flip = true; // if set to false, it means complete
bool systolic_reset = false;
bool diastolicPressRaw_flip = true; // if set to false, it means complete
bool diastolic_reset = false;
bool pulseRateRaw_flip = true;

// for pulse rate interrupt
volatile unsigned long pulse = 0;
unsigned long last_pulse = 0;
unsigned long pulse_lasttime = 0;
unsigned long pulse_thistime = 0;

// for respiration rate interrupt
volatile unsigned long respiration = 0;
unsigned long last_respiration = 0;
unsigned long respiration_lasttime = 0;
unsigned long respiration_thistime = 0;

// for pressure interrupt
int increment = 1;
unsigned int pressure = 95;
volatile byte LEDstate = LOW;
byte pressure_switch_interrupt, last_pressure_switch_interrupt = HIGH;
byte pressure_count_interrupt, last_pressure_count_interrupt = HIGH;

// for EKG timer interrupt
unsigned char EKG_buffer_pos1 = 0;
short EKG_buffer1[256];
unsigned char EKG_buffer_pos2 = 0;
short EKG_buffer2[256];
short EKG_buffer_switch = 1;


// string for serial communication
String whichTask;
String measureData = "";

// pin setup
int pulse_pin = 2;
int respiration_pin = 3;
int pressure_switch_pin = 4;
int pressure_count_pin = 5;
// temperature_pin = "A0";
// EKF pin = A1;

// set up serial port
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(2000000);
  Serial.setTimeout(5);
  // pulse rate
  pinMode(pulse_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulse_pin), pulse_count, RISING);
  // respiration
  pinMode(respiration_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(respiration_pin), respiration_count, RISING);
  pinMode(pressure_switch_pin, INPUT_PULLUP);
  pinMode(pressure_count_pin, INPUT_PULLUP);
  pinMode(A0, INPUT); // temperature
  pinMode(A1, INPUT); // EKG


  cli();//stop interrupts
  //set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();//allow interrupts
}


ISR(TIMER2_COMPA_vect) { //timer1 interrupt 8kHz toggles pin 9
  if (EKG_buffer_switch > 0) {
    EKG_buffer1[EKG_buffer_pos1] = analogRead(A1)-512;
    EKG_buffer_pos1 ++;
  }
  else {
    EKG_buffer2[EKG_buffer_pos2] = analogRead(A1)-512;
    EKG_buffer_pos2 ++;
  }
}

void get_EKG() {
//  String result = "";
  if (EKG_buffer_switch > 0) {
    EKG_buffer_switch = -1;
    unsigned char temp_pos = EKG_buffer_pos1;
    for (unsigned char i = temp_pos; i != (unsigned char)(temp_pos - 1); i++) {
      Serial.println(String(EKG_buffer1[i]));
    }
  }
  else {
    EKG_buffer_switch = 1;
    unsigned char temp_pos = EKG_buffer_pos2;
    for (unsigned char i = temp_pos; i != (unsigned char)(temp_pos - 1); i++) {
      Serial.println(String(EKG_buffer2[i]));
    }
  }

}


// main loop
void loop() {
  digitalWrite(13, LEDstate);

  // software interrupt
  pressure_switch_interrupt = digitalRead(4);
  pressure_count_interrupt = digitalRead(5);
  delay(5);
  if (last_pressure_switch_interrupt != pressure_switch_interrupt) {
    pressure_switch();
  }
  if (last_pressure_count_interrupt == HIGH && last_pressure_count_interrupt != pressure_count_interrupt) {
    pressure_count();
  }
  last_pressure_switch_interrupt = pressure_switch_interrupt;
  last_pressure_count_interrupt = pressure_count_interrupt;



  if ( Serial.available() > 0 ) {
    whichTask = Serial.readStringUntil('\0');

    // if receive 1, return measurement
    if ( whichTask == "1" ) {
      measureData = String(get_temperatureRaw());
      Serial.println(measureData);
    }
    else if ( whichTask == "2" ) {
      measureData = String(get_systolicPressRaw());
      measureData += " ";
      measureData += String(get_diastolicPressRaw());
      Serial.println(measureData);
    }
    else if ( whichTask == "3" ) {
      measureData = String(get_pulseRateRaw());
      Serial.println(measureData);
    }
    else if ( whichTask == "5" ) {
      measureData = String(get_respirationRateRaw());
      Serial.println(measureData);
    }

    // if receive 4, return battery status
    else if ( whichTask == "4" ) {
      measureData = String(get_batteryStatus());
      Serial.println(measureData);
    }

    // if receive 4, return battery status
    else if ( whichTask == "6" ) {
      //        measureData = get_EKG();
      //        Serial.println(measureData);
      get_EKG();
    }
  }
}

unsigned int get_temperatureRaw() {
  temperatureRaw = analogRead(A0) / 1024.0 * 35 + 15;
  // 0 - 1023 : 15 - 50
  // 1:? = 1023:
  return temperatureRaw;
}


unsigned int get_systolicPressRaw() {
  if (pressure >= 110) systolicPressRaw = pressure;
  return systolicPressRaw;
}

unsigned int get_diastolicPressRaw() {
  if (pressure <= 80) diastolicPressRaw = pressure;
  return diastolicPressRaw;
}

unsigned int get_pulseRateRaw() {
  pulse_thistime = micros();
  pulseRateRaw = (unsigned int)((double)(pulse - last_pulse) * 60.0 * 1000000.0 / (double)(pulse_thistime - pulse_lasttime));

  last_pulse = pulse;
  pulse_lasttime = pulse_thistime;
  return pulseRateRaw;
}

unsigned int get_respirationRateRaw() {
  respiration_thistime = micros();
  respirationRateRaw = (unsigned int)((double)(respiration - last_respiration) * 60.0 * 1000000.0 / (double)(respiration_thistime - respiration_lasttime));

  last_respiration = respiration;
  respiration_lasttime = respiration_thistime;
  return respirationRateRaw;
}

unsigned short get_batteryStatus() {
  if ( batteryStatus > 0 ) batteryStatus -= 1;
  return batteryStatus;
}

void pulse_count() {
  pulse ++;
}

void respiration_count() {
  respiration ++;
}

void pressure_count() {
  if (increment > 0) pressure = (unsigned int)((double)pressure * 1.1);
  else pressure = (unsigned int)((double)pressure * 0.9);
}

void pressure_switch() {
  increment *= (-1);
  LEDstate = !LEDstate;
}
