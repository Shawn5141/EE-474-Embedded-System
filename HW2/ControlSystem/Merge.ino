#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Elegoo_TFTLCD tft;


/*Shared varibles*/
//Measurements
unsigned int temperatureRaw=75, systolicPressRaw=80;
unsigned int diastolicPressRaw=80, pulseRateRaw=50;
//Display
unsigned char *tempCorrected=NULL, *systolicPressCorrected=NULL;
unsigned char *diastolicPressCorrected=NULL, *pulseRateCorrected=NULL;
//Status
unsigned short batteryState=200;
//Alarms
unsigned char bpOutOfRange=0,tempOutOfRange=0, pulseOutOfRange=0;
//Warning
bool bpHigh=false, tempHigh=false, pulseLow=false;


/*Define tasks*/
#define numTask 5
typedef struct TCB{
  void (*myTask)(void*);
  void* taskDataPtr;
}TCB;
TCB Measure, Compute, Display;
TCB WarningAlarm, Status, Schedule;
TCB* taskque[numTask];

/*Define data structures*/
//Task Measure's data
typedef struct DataStructMeasure{
  unsigned int *temperatureRawPtr, *systolicPressRawPtr;
  unsigned int *diastolicPressRawPtr, *pulseRateRawPtr;
}DataStructMeasure;
DataStructMeasure MeasureData;
//Task Compute's data
typedef struct DataStructCompute{
  unsigned int *temperatureRawPtr, *systolicPressRawPtr;
  unsigned int *diastolicPressRawPtr, *pulseRateRawPtr;
  unsigned char **tempCorrectedPtr, **systolicPressCorrectedPtr;
  unsigned char **diastolicPressCorrectedPtr, **pulseRateCorrectedPtr;
}DataStructCompute;
DataStructCompute ComputeData;
//Task Display's data
typedef struct DataStructDisplay{
  unsigned char **tempCorrectedPtr, **systolicPressCorrectedPtr;
  unsigned char **diastolicPressCorrectedPtr, **pulseRateCorrectedPtr;
  unsigned short *batteryStatePtr;
}DataStructDisplay;
DataStructDisplay DisplayData;
//Task WarningAlarm's data
typedef struct DataStructWarningAlarm{
  unsigned int *temperatureRawPtr, *systolicPressRawPtr;
  unsigned int *diastolicPressRawPtr, *pulseRateRawPtr;
  unsigned short *batteryStatePtr;
}DataStructWarningAlarm;
DataStructWarningAlarm WarningAlarmData;
//Task Status's data
typedef struct DataStructStatus{
  unsigned short *batteryStatePtr;
}DataStructStatus;
DataStructStatus StatusData;

/*Tasks's function*/
void Measure_function(void *uncast_data){
  DataStructMeasure* data;
  data = (DataStructMeasure*)uncast_data;
  String  serialResponse;
  Serial.write('1');
  int value[4];
  
if ( Serial.available()) {
    serialResponse = Serial.readStringUntil('\n');

    // Convert from String Object to String.
    char buf[sizeof(serialResponse)];
    serialResponse.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    int i = 0;
    String cast_str;
    while ((str = strtok_r(p, " ", &p)) != NULL) // delimiter is the semicolon
    {  
      cast_str=(String)str;
      Serial.println(cast_str.toInt());
      value[i] = cast_str.toInt();
      i++;
    }
    for(int j=0; j<4; j++) Serial.print(value[j]);
  }



    *(data->temperatureRawPtr)=value[0];
    *(data->systolicPressRawPtr)=value[1];
    *(data->diastolicPressRawPtr)=value[2];
    *(data->pulseRateRawPtr)=value[3];



}

void Compute_function(void *uncast_data){
  DataStructCompute* data;
  data = (DataStructCompute *)uncast_data; 
  *(data->tempCorrectedPtr) = String(5+0.75*(*(data->temperatureRawPtr))).c_str();
  *(data->systolicPressCorrectedPtr) = String(9+2*(*(data->systolicPressRawPtr))).c_str();
  *(data->diastolicPressCorrectedPtr) = String(6+1.5*(*(data->diastolicPressRawPtr))).c_str(); 
  *(data->pulseRateCorrectedPtr) = String(8+3*(*(data->pulseRateRawPtr))).c_str();
 

}
 

void Display_function(void *uncast_data){
  DataStructDisplay* data;
  data=(DataStructDisplay*)uncast_data;
   tft.setRotation(1);
   tft.setCursor(0, 30);
   tft.setTextSize(2);
   tft.setTextColor(WHITE,BLACK);
   tft.println("Blood Pressure: ");
   tft.print(" Systolic Pressure: ");
   if (pulseLow==true){
      tft.setTextColor(RED,BLACK);
      tft.print(String((char*)*(data->systolicPressCorrectedPtr)));
      tft.println(" mmHg");}
   else{
      tft.setTextColor(GREEN,BLACK);
      tft.print(String((char*)*(data->systolicPressCorrectedPtr)));
      tft.println(" mmHg");};
    
   tft.setTextColor(WHITE,BLACK);
   tft.print(" Diastolic Pressure:");
   if (pulseLow==true){
      tft.setTextColor(RED,BLACK);
      tft.print(String((char*)*(data->diastolicPressCorrectedPtr)));
      tft.println(" mmHg");}
   else{
      tft.setTextColor(GREEN,BLACK);
      tft.print(String((char*)*(data->diastolicPressCorrectedPtr)));
      tft.println(" mmHg");};
   
   tft.setTextColor(WHITE,BLACK);
   tft.print("Temperature:        ");
   if (tempHigh==true){tft.setTextColor(RED,BLACK);}
   else{tft.setTextColor(GREEN,BLACK);};
   tft.print(String((char*)*(data->tempCorrectedPtr)));
   tft.println("deg C");

  
   tft.setTextColor(WHITE,BLACK);
   tft.print("Pulse Rate:         ");
   if (pulseLow==true){tft.setTextColor(RED,BLACK);}
   else{tft.setTextColor(GREEN,BLACK);};
   tft.print(String((char*)*(data->pulseRateCorrectedPtr)));
   tft.println(" BPM ");

   
   tft.setTextColor(WHITE,BLACK);
   tft.print("Battery status:     ");
   if (batteryState<20){tft.setTextColor(RED,BLACK);}
   else{tft.setTextColor(GREEN,BLACK);};
   tft.print(*(data->batteryStatePtr));
   tft.println(" % ");

   tft.println();
   tft.println();
}

void WarningAlarm_function(void *uncast_data){
  DataStructWarningAlarm * data;
  data = ( DataStructWarningAlarm *)uncast_data;
  if(*(data->temperatureRawPtr)<36.1 || *(data->temperatureRawPtr)>37.8){
    tempOutOfRange=*(data->temperatureRawPtr);
    if (*(data->temperatureRawPtr)>37.8){
      tempHigh=true;
    }else{
      tempHigh=false;
    }
  }
  if (*(data->systolicPressRawPtr)>120 || *(data->diastolicPressRawPtr)<80){
    bpOutOfRange=*(data->systolicPressRawPtr);
    if (*(data->systolicPressRawPtr)>120){
      bpHigh=true;
    }else{
      bpHigh=false;
    }
  }
  
  if (*(data->pulseRateRawPtr)<60 || *(data->pulseRateRawPtr)>100){
    pulseOutOfRange = *(data->pulseRateRawPtr);
    if(*(data->pulseRateRawPtr)<60){
      pulseLow=true;
    }else{
      pulseLow=false;
    }
  }
}

void Status_function(void *uncast_data){
  String  serialResponse;
  DataStructStatus* data;
  int value;
  data=(DataStructStatus *)uncast_data;
  Serial.write('1');
  
if ( Serial.available()) {
    serialResponse = Serial.readStringUntil('\n');

    // Convert from String Object to String.
    char buf[sizeof(serialResponse)];
    serialResponse.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    
    int i = 0;
    String cast_str;
    while ((str = strtok_r(p, " ", &p)) != NULL) // delimiter is the semicolon
    {  
      cast_str=(String)str;
      Serial.println(cast_str.toInt());
      value = cast_str.toInt();
      i++;
    }
    for(int j=0; j<1; j++) Serial.print(value);
  }
  *(data->batteryStatePtr)=value;
}

void Schedule_function(void *data){
  
}

String taskName[numTask] = {"Measure", "Compute", "Display", "WarningAlarm", "Status"};
String message; //message of task time
#define taskqueFinishPin 30 //pin to be toggled after one cycle of task que
unsigned long start_time; //the start time of each task
unsigned long taskTime[numTask]; //stored this execution time of each task

void setup() {
  //Initialized task Measure
  Measure.myTask = Measure_function;
  MeasureData.temperatureRawPtr = &temperatureRaw;
  MeasureData.systolicPressRawPtr = &systolicPressRaw;
  MeasureData.diastolicPressRawPtr = &diastolicPressRaw;
  MeasureData.pulseRateRawPtr = &pulseRateRaw;
  Measure.taskDataPtr = &MeasureData;
  taskque[0] = &Measure;
  //Initialized task Compute
  Compute.myTask = Compute_function;
  ComputeData.temperatureRawPtr = &temperatureRaw;
  ComputeData.systolicPressRawPtr = &systolicPressRaw;
  ComputeData.diastolicPressRawPtr = &diastolicPressRaw;
  ComputeData.pulseRateRawPtr = &pulseRateRaw;
  ComputeData.tempCorrectedPtr = &tempCorrected;
  ComputeData.systolicPressCorrectedPtr = &systolicPressCorrected;
  ComputeData.diastolicPressCorrectedPtr = &diastolicPressCorrected;
  ComputeData.pulseRateCorrectedPtr = &pulseRateCorrected;
  Compute.taskDataPtr = &ComputeData;
  taskque[1] = &Compute;
  //Initialized task Display
  Display.myTask = Display_function;
  DisplayData.tempCorrectedPtr = &tempCorrected;
  DisplayData.systolicPressCorrectedPtr = &systolicPressCorrected;
  DisplayData.diastolicPressCorrectedPtr = &diastolicPressCorrected;
  DisplayData.pulseRateCorrectedPtr = &pulseRateCorrected;
  DisplayData.batteryStatePtr = &batteryState;
  Display.taskDataPtr = &DisplayData;
  taskque[2] = &Display;
  //Initialized task WarningAlarm
  WarningAlarm.myTask = WarningAlarm_function;
  WarningAlarmData.temperatureRawPtr = &temperatureRaw;
  WarningAlarmData.systolicPressRawPtr = &systolicPressRaw;
  WarningAlarmData.diastolicPressRawPtr = &diastolicPressRaw;
  WarningAlarmData.pulseRateRawPtr = &pulseRateRaw;
  WarningAlarmData.batteryStatePtr = &batteryState;
  WarningAlarm.taskDataPtr = &WarningAlarmData;
  taskque[3] = &WarningAlarm;
  //Initialized task Status
  Status.myTask = Status_function;
  StatusData.batteryStatePtr = &batteryState;
  Status.taskDataPtr = &StatusData;
  taskque[4] = &Status;
  //Initialized task Schedule
  Schedule.myTask = Schedule_function;
  Schedule.taskDataPtr = NULL;
  //Initialized taskqueFinishPin
  pinMode(taskqueFinishPin, OUTPUT);
  digitalWrite(taskqueFinishPin, LOW);
  //Initialized serial port 0 & 1
  Serial.begin(9600);
  Serial1.begin(9600);
  //Initialized for TFT

  Serial.println(F("TFT LCD test"));
  #ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.4\" TFT Arduino Shield Pinout"));
  #else
  Serial.println(F("Using Elegoo 2.4\" TFT Breakout Board Pinout"));
  #endif
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  tft.reset();
   uint16_t identifier = tft.readID();
   if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }
  else if(identifier==0x1111)
  {     
      identifier=0x9328;
       Serial.println(F("Found 0x9328 LCD driver"));
  }
  else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9328;
  
  }
  tft.begin(identifier);
  tft.fillScreen(BLACK);
}

void loop() {
  for (int i=0; i<numTask; i++){
    start_time = micros();
    (taskque[i]->myTask)(taskque[i]->taskDataPtr); //execute task
    taskTime[i] = micros() - start_time;
    while(micros() - start_time < 5000){} //schedule each task for 5 ms
  }
  //toggle pin after one cycle of task que
  digitalWrite(taskqueFinishPin, !digitalRead(taskqueFinishPin));
  //show execution time for each task in serial monitor
  message = "";
  for (int i=0; i<numTask; i++){
    message += taskName[i] + ": " + taskTime[i] + " us\n";
  }
  Serial.write(message.c_str());
}
