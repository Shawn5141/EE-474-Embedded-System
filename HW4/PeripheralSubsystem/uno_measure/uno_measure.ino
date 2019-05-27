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

// string for serial communication
String whichTask;
String measureData = "";

// set up serial port
void setup(){
  pinMode(13, OUTPUT);
  Serial.begin(2000000);
  Serial.setTimeout(5);
  // pulse rate
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), pulse_count, CHANGE);
  // respiration
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), respiration_count, CHANGE);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
}

// main loop
void loop(){
  digitalWrite(13, LEDstate);
  
  // software interrupt
  pressure_switch_interrupt = digitalRead(4);
  pressure_count_interrupt = digitalRead(5);
  delay(30);
  if(last_pressure_switch_interrupt != pressure_switch_interrupt){
    pressure_switch();
  }
  if(last_pressure_count_interrupt== HIGH && last_pressure_count_interrupt != pressure_count_interrupt){
    pressure_count();
  }
  last_pressure_switch_interrupt = pressure_switch_interrupt;
  last_pressure_count_interrupt = pressure_count_interrupt;
  
  if ( Serial.available() > 0 ) {
    whichTask = Serial.readStringUntil('\0');

    // if receive 1, return measurement
    if( whichTask == "1" ){  
        measureData = String(get_temperatureRaw());
        Serial.println(measureData);
    }
    else if( whichTask == "2" ){
        measureData = String(get_systolicPressRaw());
        measureData += " ";
        measureData += String(get_diastolicPressRaw());
        Serial.println(measureData);
    }
    else if( whichTask == "3" ){
        measureData = String(get_pulseRateRaw());
        Serial.println(measureData);
    }
    else if( whichTask == "5" ){
        measureData = String(get_respirationRateRaw());
        Serial.println(measureData);
    }

    // if receive 4, return battery status
    else if( whichTask == "4" ){
        measureData = String(get_batteryStatus());
        Serial.println(measureData);
    }
  }
}

unsigned int get_temperatureRaw(){
  // before temperatureRaw exceed 50
  if( temperatureRaw_flip ){
    if( temperatureRaw_count > 0 ) temperatureRaw += 2;
    else temperatureRaw -= 1;

    if( temperatureRaw > 50 ) temperatureRaw_flip = false; 
  }
  // before temperatureRaw lower than 15
  else{
    if( temperatureRaw_count > 0 ) temperatureRaw -= 2;
    else temperatureRaw += 1;

    if( temperatureRaw < 15 ) temperatureRaw_flip = true; 
  }

  // flip counter
  temperatureRaw_count *= -1 ;
  return temperatureRaw;
}

//unsigned int get_systolicPressRaw(){
//
//  // if diastolic is completed and haven't reset
//  if( !diastolicPressRaw_flip && !systolic_reset ){
//    systolicPressRaw = 80;
//    systolic_reset = true;
//  }
//
//  // when first go out of range
//  else if( systolicPressRaw > 100 && systolicPressRaw_flip == true ){
//    systolicPressRaw_flip = false; // complete
//    diastolicPressRaw_flip = true;
//    systolic_reset = false;
//  }
//
//  // when out of range
//  else if( systolicPressRaw > 100 && systolicPressRaw_flip == false ){
//    systolicPressRaw = systolicPressRaw;
//  }
//
//  // when in the range
//  else{
//    if( systolicPressRaw_count > 0 ) systolicPressRaw += 3;
//    else systolicPressRaw -= 1;
//  }
//  
//  // flip counter
//  systolicPressRaw_count *= -1 ;
//  return systolicPressRaw;
//}

//unsigned int get_diastolicPressRaw(){
//
//  // if systolic is completed and haven't reset
//  if( !systolicPressRaw_flip && !diastolic_reset ){
//    diastolicPressRaw = 80;
//    diastolic_reset = true;
//  }
//
//  // when first go out of range
//  else if( diastolicPressRaw < 40 && diastolicPressRaw_flip == true ){
//    diastolicPressRaw_flip = false; // complete
//    systolicPressRaw_flip = true; 
//    diastolic_reset = false;
//  }
//
//  // when go out of range
//  else if( diastolicPressRaw < 40 && diastolicPressRaw_flip == false ){
//    diastolicPressRaw = diastolicPressRaw;
//  }
//
//  // when in the range
//  else{
//    if( diastolicPressRaw_count > 0 ) diastolicPressRaw -= 2;
//    else diastolicPressRaw += 1;
//  }
//  
//  // flip counter
//  diastolicPressRaw_count *= -1 ;
//  return diastolicPressRaw;
//}

unsigned int get_systolicPressRaw(){
  if(pressure >= 110) systolicPressRaw = pressure;
  return systolicPressRaw;
}

unsigned int get_diastolicPressRaw(){
  if(pressure <= 80) diastolicPressRaw = pressure;
  return diastolicPressRaw;
}

unsigned int get_pulseRateRaw(){
  pulse_thistime = micros();
  pulseRateRaw = (unsigned int)((double)(pulse-last_pulse)*60.0 *1000000.0 / (double)(pulse_thistime - pulse_lasttime));

  last_pulse = pulse;
  pulse_lasttime = pulse_thistime;
  return pulseRateRaw;
}

unsigned int get_respirationRateRaw(){
  respiration_thistime = micros();
  respirationRateRaw = (unsigned int)((double)(respiration-last_respiration)*60.0 *1000000.0 / (double)(respiration_thistime - respiration_lasttime));

  last_respiration = respiration;
  respiration_lasttime = respiration_thistime;
  return respirationRateRaw;
}

unsigned short get_batteryStatus(){
  if( batteryStatus > 0 ) batteryStatus -= 1;
  return batteryStatus;
}

void pulse_count() {
  pulse ++;
}

void respiration_count() {
  respiration ++;
}

void pressure_count(){
  if(increment > 0) pressure = (unsigned int)((double)pressure*1.1);
  else pressure = (unsigned int)((double)pressure*0.9);
}

void pressure_switch(){
  increment*=(-1);
  LEDstate = !LEDstate;
}
