// initial value for the measurements
unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 50;
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

// string for serial communication
String whichTask;
String measureData = "";

// set up serial port
void setup(){
  Serial.begin(2000000);
  Serial.setTimeout(5);
}

// main loop
void loop(){
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

unsigned int get_systolicPressRaw(){

  // if diastolic is completed and haven't reset
  if( !diastolicPressRaw_flip && !systolic_reset ){
    systolicPressRaw = 80;
    systolic_reset = true;
  }

  // when first go out of range
  else if( systolicPressRaw > 100 && systolicPressRaw_flip == true ){
    systolicPressRaw_flip = false; // complete
    diastolicPressRaw_flip = true;
    systolic_reset = false;
  }

  // when out of range
  else if( systolicPressRaw > 100 && systolicPressRaw_flip == false ){
    systolicPressRaw = systolicPressRaw;
  }

  // when in the range
  else{
    if( systolicPressRaw_count > 0 ) systolicPressRaw += 3;
    else systolicPressRaw -= 1;
  }
  
  // flip counter
  systolicPressRaw_count *= -1 ;
  return systolicPressRaw;
}

unsigned int get_diastolicPressRaw(){

  // if systolic is completed and haven't reset
  if( !systolicPressRaw_flip && !diastolic_reset ){
    diastolicPressRaw = 80;
    diastolic_reset = true;
  }

  // when first go out of range
  else if( diastolicPressRaw < 40 && diastolicPressRaw_flip == true ){
    diastolicPressRaw_flip = false; // complete
    systolicPressRaw_flip = true; 
    diastolic_reset = false;
  }

  // when go out of range
  else if( diastolicPressRaw < 40 && diastolicPressRaw_flip == false ){
    diastolicPressRaw = diastolicPressRaw;
  }

  // when in the range
  else{
    if( diastolicPressRaw_count > 0 ) diastolicPressRaw -= 2;
    else diastolicPressRaw += 1;
  }
  
  // flip counter
  diastolicPressRaw_count *= -1 ;
  return diastolicPressRaw;
}

unsigned int get_pulseRateRaw(){

  // before pulseRateRaw exceed 50
  if( pulseRateRaw_flip ){
    if( pulseRateRaw_count > 0 ) pulseRateRaw -= 1;
    else pulseRateRaw += 3;

    if( pulseRateRaw > 40 ) pulseRateRaw_flip = false; 
  }

  // before pulseRateRaw lower than 15
  else{
    if( pulseRateRaw_count > 0 ) pulseRateRaw += 1;
    else pulseRateRaw -= 3;

    if( pulseRateRaw < 15 ) pulseRateRaw_flip = true; 
  }

//  pulseRateRaw *= (1.0+(random(-20, 20)/100.0));
//  if( pulseRateRaw > 200) pulseRateRaw = 200;
//  else if( pulseRateRaw < 0) pulseRateRaw = 0;
//  
//  // flip counter
//  pulseRateRaw_count *= -1 ;
  return pulseRateRaw;
}

unsigned short get_batteryStatus(){
  if( batteryStatus > 0 ) batteryStatus -= 1;
  return batteryStatus;
}
