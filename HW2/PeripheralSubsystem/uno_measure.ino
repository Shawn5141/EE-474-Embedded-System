unsigned int temperatureRaw = 75;
unsigned int systolicPressRaw = 80;
unsigned int diastolicPressRaw = 80;
unsigned int pulseRateRaw = 50;
int temperatureRaw_count = 1; // 1 means even time, -1 means odd time;
int systolicPressRaw_count = 1;
int diastolicPressRaw_count = 1;
int pulseRateRaw_count = 1;
bool temperatureRaw_flip = true;
bool systolicPressRaw_flip = true; // if set to false, it means complete
bool diastolicPressRaw_flip = true; // if set to false, it means complete
bool pulseRateRaw_flip = true;
String whichTask;
String measureData = "";

void setup(){
  Serial.begin(9600);
}

void loop(){
  if ( Serial.available() > 0 ) {
    whichTask = Serial.readStringUntil('\0');  
    measureData = String(get_temperatureRaw()) + " " + String(get_systolicPressRaw()) + " " +
            String(get_diastolicPressRaw()) + " " + String(get_pulseRateRaw());
    Serial.println(measureData);
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

  temperatureRaw_count *= -1 ;
  return temperatureRaw;
}

unsigned int get_systolicPressRaw(){
  
  if( !systolicPressRaw_flip ){
    systolicPressRaw = 80;
    systolicPressRaw_flip = true;
  }

  if( systolicPressRaw_count > 0 ) systolicPressRaw += 3;
  else systolicPressRaw -= 1;

  if( systolicPressRaw > 100 ) systolicPressRaw_flip = false; // complete 
  
  
  systolicPressRaw_count *= -1 ;
  return systolicPressRaw;
}

unsigned int get_diastolicPressRaw(){
  if( !diastolicPressRaw_flip ){
    diastolicPressRaw = 80;
    diastolicPressRaw_flip = true;
  }

  if( diastolicPressRaw_count > 0 ) diastolicPressRaw -= 2;
  else diastolicPressRaw += 1;

  if( diastolicPressRaw < 40 ) diastolicPressRaw_flip = false; // complete 
  
  
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

  pulseRateRaw_count *= -1 ;
  return pulseRateRaw;
}
