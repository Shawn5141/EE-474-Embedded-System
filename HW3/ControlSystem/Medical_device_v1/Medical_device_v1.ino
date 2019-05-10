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
unsigned int temperatureRawBuf[8], bloodPressRawBuf[16];
unsigned int pulseRateRawBuf[8];
//Buffer index
unsigned char tempIndex=0, bloodPressIndex=0, pulseRateIndex=0;
//Display
unsigned char tempCorrectedBuf[8], bloodPressCorrectedBuf[16]; 
unsigned char pulseRateCorrectedBuf[8];
//Status
unsigned short batteryState=200;
//Alarms
unsigned char bpOutOfRange=0, tempOutOfRange=0, pulseOutOfRange=0;
//Warning
bool bpHigh=false, tempHigh=false, pulseLow=false;
//TFT Keypad
unsigned short functionSelect=0, measurementSelection=0, alarmAcknowledge=0;


/*Define tasks*/
#define numTask 7
typedef struct TCB{
  void (*myTask)(void*);
  void *taskDataPtr;
  TCB *next=NULL, *prev=NULL;
}TCB;
String taskName[numTask] = {"Display", "TFTKeypad", "WarningAlarm", "Compute"\
                            "Measure", "Status", "Communications"};
TCB Measure, Compute;
TCB Display, WarningAlarm, Status;
TCB TFTKeypad, Communications;
TCB *head=NULL, *tail=NULL, *currentTask=NULL;
TCB *taskArray[numTask];
bool taskAddFlag[numTask]={false};
bool taskInQue[numTask]={false};


/*Scheduler data*/
String message; //message of task time
#define taskqueFinishPin 30 //pin to be toggled after one cycle of task que
unsigned long mStart_time[4]; //the start time of each measurement
bool mAvailable[4]={true}; //availibility of each measurement
unsigned long start_time; //the start time of each task
unsigned long taskTime[numTask]; //store the execution time of each task


/*Define data structures*/
//Task Measure's data
typedef struct DataStructMeasure{
  unsigned char id;
  unsigned int *temperatureRawBufPtr, *bloodPressRawBufPtr, *pulseRateRawBufPtr;
  unsigned short *measurementSelectionPtr;
  unsigned char *tempIndexPtr, *bloodPressIndexPtr, *pulseRateIndexPtr;
  bool *addFlagPtr;
}DataStructMeasure;
DataStructMeasure MeasureData;

//Task Compute's data
typedef struct DataStructCompute{
  unsigned char id;
  unsigned int *temperatureRawBufPtr, *bloodPressRawBufPtr, *pulseRateRawBufPtr;
  unsigned char *tempCorrectedBufPtr, *bloodPressCorrectedBufPtr;
  unsigned char *pulseRateCorrectedBufPtr;
  unsigned short *measurementSelectionPtr;
  unsigned char *tempIndexPtr, *bloodPressIndexPtr, *pulseRateIndexPtr;
  bool *addFlagPtr;
}DataStructCompute;
DataStructCompute ComputeData;

//Task Display's data
typedef struct DataStructDisplay{
  unsigned char id;
  unsigned char *tempCorrectedBufPtr, *bloodPressCorrectedBufPtr;
  unsigned char *pulseRateCorrectedBufPtr;
  unsigned short *batteryStatePtr;
  unsigned short *functionSelectPtr, *measurementSelectionPtr, *alarmAcknowledgePtr;
  unsigned char *tempIndexPtr, *bloodPressIndexPtr, *pulseRateIndexPtr;
  bool *addFlagPtr;
}DataStructDisplay;
DataStructDisplay DisplayData;

//Task WarningAlarm's data
typedef struct DataStructWarningAlarm{
  unsigned char id;
  unsigned int *temperatureRawBufPtr, *bloodPressRawBufPtr, *pulseRateRawBufPtr;
  unsigned short *batteryStatePtr;
  unsigned char *tempIndexPtr, *bloodPressIndexPtr, *pulseRateIndexPtr;
  bool *addFlagPtr, *addComFlagPtr;
}DataStructWarningAlarm;
DataStructWarningAlarm WarningAlarmData;

//Task Status's data
typedef struct DataStructStatus{
  unsigned char id;
  unsigned short *batteryStatePtr;
  bool *addFlagPtr;
}DataStructStatus;
DataStructStatus StatusData;

//Task TFTKeypad's data
typedef struct DataStructTFTKeypad{
  unsigned char id;
  unsigned short *functionSelectPtr, *measurementSelectionPtr, *alarmAcknowledgePtr;
  bool *addFlagPtr;
}DataStructTFTKeypad;
DataStructTFTKeypad TFTKeypadData;

//Task Communications's data
typedef struct DataStructCommunications{
  unsigned char id;
  unsigned char *tempCorrectedBufPtr, *bloodPressCorrectedBufPtr;
  unsigned char *pulseRateCorrectedBufPtr;
  unsigned short *batteryStatePtr;
  unsigned char *tempIndexPtr, *bloodPressIndexPtr, *pulseRateIndexPtr;
  bool *addFlagPtr;
}DataStructCommunications;
DataStructCommunications CommunicationsData;

void Insert(TCB* node){
   if(NULL==head){
        head=node;
        tail=node;

    }else{
        head->prev=node;
        node->next=head;
        head=head->prev;
          
    }
    return;
}

void Delete(TCB* node){
   
    if (head->next ==NULL){
        return;
    }else if (head==tail){
        head=tail=NULL;
    }else if(head==node){
        head= head->next;


            
    }else if (tail==node){
        tail=tail->prev;
    }else{              
        node->prev->next=node->next;
        node->next->prev=node->prev;
        
    }
    free(node);
    return;


}


/*Tasks's function*/
void Measure_function(void *uncast_data){
  DataStructMeasure* data;
  data = (DataStructMeasure*)uncast_data;
  String  serialResponse;
  char *p;
  char *str;
  int i = 0;
  String cast_str;
  int value[2];
  char buf[10];

  switch(*(data->measurementSelectionPtr)){
    // Temperature
    case 1:
      Serial1.write("1");
      while ( !Serial1.available()){}
      serialResponse = Serial1.readStringUntil('\n');
      *(data->tempIndexPtr) = (*(data->tempIndexPtr) + 1 ) % 8;
      *(data->temperatureRawBufPtr + *(data->tempIndexPtr)) = serialResponse.toInt();
      break;

    // Pressure
    case 2:
      Serial1.write("2");
      while ( !Serial1.available()){}
      serialResponse = Serial1.readStringUntil('\n');
      serialResponse.toCharArray(buf, sizeof(buf));
      p = buf;
      i = 0;
      
      while ((str = strtok_r(p, " ", &p)) != NULL) // delimiter is the semicolon
      {  
        cast_str=(String)str;
        value[i] = cast_str.toInt();
        i++;
      }

      *(data->bloodPressIndexPtr) = (*(data->bloodPressIndexPtr) + 1 ) % 4;
      *(data->bloodPressRawBufPtr + *(data->bloodPressIndexPtr) ) = value[0];
      *(data->bloodPressRawBufPtr + *(data->bloodPressIndexPtr) + 4 ) = value[1];
      break;

    // Pulse rate
    case 3:
      Serial1.write("3");
      while ( !Serial1.available()){}
      serialResponse = Serial1.readStringUntil('\n');
      if(abs(*(data->pulseRateRawBufPtr + *(data->pulseRateIndexPtr)) - serialResponse.toInt())/(*(data->pulseRateRawBufPtr + *(data->pulseRateIndexPtr))) >= 0.15){
        *(data->pulseRateIndexPtr) = (*(data->pulseRateIndexPtr) + 1 ) % 8;
        *(data->pulseRateRawBufPtr + *(data->pulseRateIndexPtr)) = serialResponse.toInt();
      }
      break;

    default:
      // nothing
      break;
  }

  *(data->addFlagPtr) = false;
}

void Compute_function(void *uncast_data){
  DataStructCompute* data;
  data = (DataStructCompute *)uncast_data;

  switch(*(data->measurementSelectionPtr)){
    // Temperature
    case 1:
      *(data->tempCorrectedBufPtr + *(data->tempIndexPtr)) = 5+0.75*(*(data->temperatureRawBufPtr + *(data->tempIndexPtr)));
      break;

    // blood pressure
    case 2:
      *(data->bloodPressCorrectedBufPtr + *(data->bloodPressIndexPtr)) = 9+2*(*(data->bloodPressRawBufPtr + *(data->bloodPressIndexPtr)));
      *(data->bloodPressCorrectedBufPtr + *(data->bloodPressIndexPtr) + 4) = 6+1.5*(*(data->bloodPressRawBufPtr + *(data->bloodPressIndexPtr) + 4));
      break;

    // pulse rate
    case 3:
      *(data->pulseRateCorrectedBufPtr + *(data->pulseRateIndexPtr)) = 8+3*(*(data->pulseRateRawBufPtr + *(data->pulseRateIndexPtr)));
      break;

    default:
      break;
    // nothing
    
  }
  *(data->addFlagPtr) = false;
}

void Communications_function(void *uncast_data){
  DataStructDisplay* data;
  data=(DataStructDisplay*)uncast_data;

//  Serial.write("Temperature:          " + String(*(data->tempCorrectedBufPtr + *(data->tempIndexPtr))) + " C");
//  Serial.write("Systolic pressure:    " + String(*(data->bloodPressCorrectedBufPtr + *(data->bloodPressIndexPtr))) + " mm Hg");
//  Serial.write("Diastolic pressure:   " + String(*(data->bloodPressCorrectedBufPtr + *(data->bloodPressIndexPtr) + 4)) + " mm Hg");
//  Serial.write("Pulse rate:           " + String(*(data->pulseRateCorrectedBufPtr + *(data->pulseRateIndexPtr))) + " BPM");
//  Serial.write("Battery:              " + String(*(data->batteryStatePtr)));
  Serial.write("Temperature:          ");
  Serial.write(*(data->tempCorrectedBufPtr + *(data->tempIndexPtr)));
  Serial.write(" C\n");
  Serial.write("Systolic pressure:    ");
  Serial.write(*(data->bloodPressCorrectedBufPtr + *(data->bloodPressIndexPtr)));
  Serial.write(" mm Hg\n");
  Serial.write("Diastolic pressure:   ");
  Serial.write(*(data->bloodPressCorrectedBufPtr + *(data->bloodPressIndexPtr) + 4));
  Serial.write(" mm Hg\n");
  Serial.write("Pulse rate:           ");
  Serial.write(*(data->pulseRateCorrectedBufPtr + *(data->pulseRateIndexPtr)));
  Serial.write(" BPM\n");
  Serial.write("Battery:              ");
  Serial.write(*(data->batteryStatePtr));
}
 

void Display_function(void *uncast_data){
  DataStructDisplay* data;
  data=(DataStructDisplay*)uncast_data;
   tft.setRotation(1);
   tft.setCursor(0, 30);
   tft.setTextSize(2);
   tft.setTextColor(WHITE);
   tft.println("Blood Pressure: ");
   tft.setTextSize(1);
   tft.println("        ");
   tft.setTextSize(2);
   tft.print(" Systolic : ");
   if (bpHigh==true){
      tft.setTextColor(RED,BLACK);
      
      tft.print((char*)(data->systolicPressCorrectedPtr));
      
      tft.println(" mmHg   ");}
   else{
      tft.setTextColor(GREEN,BLACK);
      tft.print((char*)(data->systolicPressCorrectedPtr));
      tft.println(" mmHg   ");};
    
   tft.setTextColor(WHITE);
   tft.setTextSize(1);
   tft.println("        ");
   tft.setTextSize(2);
   tft.print(" Diastolic :");
   if (pulseLow==true){
      tft.setTextColor(RED,BLACK);
      
      tft.print((char*)(data->diastolicPressCorrectedPtr));
      tft.println(" mmHg ");}
   else{
      tft.setTextColor(GREEN,BLACK);
      tft.print((char*)(data->diastolicPressCorrectedPtr));
      tft.println(" mmHg  ");};
   
   tft.setTextColor(WHITE);
   tft.println("        ");
   tft.print("Temperature:    ");
   if (tempHigh==true){tft.setTextColor(RED,BLACK);}
   else{tft.setTextColor(GREEN,BLACK);};
   tft.print((char*)(data->tempCorrectedPtr));
   tft.setTextSize(1);
   tft.print((char)223);
   tft.setTextSize(2);
   tft.println("C  ");

  
   tft.setTextColor(WHITE);
   tft.println("        ");
   tft.print("Pulse Rate:     ");
   if (pulseLow==true){tft.setTextColor(RED,BLACK);}
   else{tft.setTextColor(GREEN,BLACK);};
   tft.print((char*)(data->pulseRateCorrectedPtr));
   tft.println(" BPM ");

   
   tft.setTextColor(WHITE);
   tft.println("        ");
   tft.print("Battery status: ");
   if (batteryState<=20){tft.setTextColor(RED,BLACK);}
   else{tft.setTextColor(GREEN,BLACK);};
   tft.print(*(data->batteryStatePtr));
   tft.println("   ");

   tft.println();
   tft.println();
}

void WarningAlarm_function(void *uncast_data){
  DataStructWarningAlarm * data;
  data = ( DataStructWarningAlarm *)uncast_data;
  if(*(data->temperatureRawPtr)<36.1 || *(data->temperatureRawPtr)>37.8){
    tempOutOfRange=1;
      tempHigh=true;
    }else{
      tempHigh=false;
      tempOutOfRange=0;
    
  }
  if (*(data->systolicPressRawPtr)>120 || *(data->diastolicPressRawPtr)<80){
      bpOutOfRange=1;
      bpHigh=true;
    }else{
      bpHigh=false;
      bpOutOfRange=0;
  }
  
  if (*(data->pulseRateRawPtr)<60 || *(data->pulseRateRawPtr)>100){
      pulseOutOfRange=1;
      pulseLow=true;
    }else{
      pulseLow=false;
      pulseOutOfRange=0;
    
  }
}



void Status_function(void *uncast_data){
  String  serialResponse;
  DataStructStatus* data;
  int value;
  data=(DataStructStatus *)uncast_data;
  Serial1.write("4");
  
  while( !Serial1.available()) {}
  serialResponse = Serial1.readStringUntil('\n');

  // Convert from String Object to String.
  char buf[serialResponse.length()+1];
  serialResponse.toCharArray(buf, sizeof(buf));
  char *p = buf;
  char *str;
  
  int i = 0;
  String cast_str;
  while ((str = strtok_r(p, " ", &p)) != NULL) // delimiter is the semicolon
  {  
    cast_str=(String)str;
    value = cast_str.toInt();
  }
  //for(int j=0; j<1; j++) {
    
  //  Serial.print("This is Status value");
    //Serial.println(value);
    //}

  *(data->batteryStatePtr)=value;
}


void setup() {

  //Initialized buffers
  temperatureRawBuf[0] = 75; pulseRateRawBuf[0] = 0;
  bloodPressRawBuf[0] = 80; bloodPressRawBuf[8] = 80;
  
  //Initialized task Measure
  Measure.myTask = Measure_function;
  MeasureData.temperatureRawBufPtr = temperatureRawBuf;
  MeasureData.bloodPressRawBufPtr = bloodPressRawBuf;
  MeasureData.pulseRateRawBufPtr = pulseRateRawBuf;
  MeasureData.measurementSelectionPtr = &measurementSelection;
  MeasureData.tempIndexPtr = &tempIndex;
  MeasureData.bloodPressIndexPtr = &bloodPressIndex;
  MeasureData.pulseRateIndexPtr = &pulseRateIndex;
  MeasureData.id = 4;
  MeasureData.addFlagPtr = &taskAddFlag[4];
  Measure.taskDataPtr = &MeasureData;
  taskArray[4] = &Measure;

  //Initialized task Compute
  Compute.myTask = Compute_function;
  ComputeData.temperatureRawBufPtr = temperatureRawBuf;
  ComputeData.bloodPressRawBufPtr = bloodPressRawBuf;
  ComputeData.pulseRateRawBufPtr = pulseRateRawBuf;
  ComputeData.tempCorrectedBufPtr = tempCorrectedBuf;
  ComputeData.bloodPressCorrectedBufPtr = bloodPressCorrectedBuf;
  ComputeData.pulseRateCorrectedBufPtr = pulseRateCorrectedBuf;
  ComputeData.measurementSelectionPtr = &measurementSelection;
  ComputeData.tempIndexPtr = &tempIndex;
  ComputeData.bloodPressIndexPtr = &bloodPressIndex;
  ComputeData.pulseRateIndexPtr = &pulseRateIndex;
  ComputeData.id = 3;
  ComputeData.addFlagPtr = &taskAddFlag[3];
  Compute.taskDataPtr = &ComputeData;
  taskArray[3] = &Compute;

  //Initialized task Display
  Display.myTask = Display_function;
  DisplayData.tempCorrectedBufPtr = tempCorrectedBuf;
  DisplayData.bloodPressCorrectedBufPtr = bloodPressCorrectedBuf;
  DisplayData.pulseRateCorrectedBufPtr = pulseRateCorrectedBuf;
  DisplayData.batteryStatePtr = &batteryState;
  DisplayData.measurementSelectionPtr = &measurementSelection;
  DisplayData.alarmAcknowledgePtr = &alarmAcknowledge;
  DisplayData.functionSelectPtr = &functionSelect;
  DisplayData.tempIndexPtr = &tempIndex;
  DisplayData.bloodPressIndexPtr = &bloodPressIndex;
  DisplayData.pulseRateIndexPtr = &pulseRateIndex;
  DisplayData.id = 0;
  DisplayData.addFlagPtr = &taskAddFlag[0];
  Display.taskDataPtr = &DisplayData;
  taskArray[0] = &Display;

  //Initialized task WarningAlarm
  WarningAlarm.myTask = WarningAlarm_function;
  WarningAlarmData.temperatureRawBufPtr = temperatureRawBuf;
  WarningAlarmData.bloodPressRawBufPtr = bloodPressRawBuf;
  WarningAlarmData.pulseRateRawBufPtr = pulseRateRawBuf;
  WarningAlarmData.batteryStatePtr = &batteryState;
  WarningAlarmData.tempIndexPtr = &tempIndex;
  WarningAlarmData.bloodPressIndexPtr = &bloodPressIndex;
  WarningAlarmData.pulseRateIndexPtr = &pulseRateIndex;
  WarningAlarmData.id = 2;
  WarningAlarmData.addFlagPtr = &taskAddFlag[2];
  WarningAlarmData.addComFlagPtr = &taskAddFlag[6];
  WarningAlarm.taskDataPtr = &WarningAlarmData;
  taskArray[2] = &WarningAlarm;

  //Initialized task Status
  Status.myTask = Status_function;
  StatusData.batteryStatePtr = &batteryState;
  StatusData.id = 5;
  StatusData.addFlagPtr = &taskAddFlag[5];
  Status.taskDataPtr = &StatusData;
  taskArray[5] = &Status;

  //Initialized task TFTKeypad
  TFTKeypad.myTask = TFTKeypad_function;
  TFTKeypadData.measurementSelectionPtr = &measurementSelection;
  TFTKeypadData.alarmAcknowledgePtr = &alarmAcknowledge;
  TFTKeypadData.functionSelectPtr = &functionSelect;
  TFTKeypadData.id = 1;
  TFTKeypadData.addFlagPtr = &taskAddFlag[1];
  TFTKeypad.taskDataPtr = &TFTKeypadData;
  taskArray[1] = &TFTKeypad;

  //Initialized task Communications
  Communications.myTask = Communications_function;
  CommunicationsData.tempCorrectedBufPtr = tempCorrectedBuf;
  CommunicationsData.bloodPressCorrectedBufPtr = bloodPressCorrectedBuf;
  CommunicationsData.pulseRateCorrectedBufPtr = pulseRateCorrectedBuf;
  CommunicationsData.batteryStatePtr = &batteryState;
  CommunicationsData.tempIndexPtr = &tempIndex;
  CommunicationsData.bloodPressIndexPtr = &bloodPressIndex;
  CommunicationsData.pulseRateIndexPtr = &pulseRateIndex;
  CommunicationsData.id = 6;
  CommunicationsData.addFlagPtr = &taskAddFlag[6];
  Communications.taskDataPtr = &CommunicationsData;
  taskArray[6] = &Communications;

  //Initialized taskque
  taskAddFlag[0] = true; Insert(taskArray[0]); taskInQue[0] = true;
  taskAddFlag[1] = true; Insert(taskArray[1]); taskInQue[1] = true;
  taskAddFlag[2] = true; Insert(taskArray[2]); taskInQue[2] = true;

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

/*Scheduler*/
void loop() {
  for (int i=0; i<numTask; i++)
    taskTime[i] = 0;
  for (int i=0; i<4; i++){
    if (!mAvailable[i] && millis() - mStart_time[i] >= 5000)
      mAvailable[i] = true;
  }
  if (mAvailable[3]){
    taskAddFlag[5] = true;
    taskAddFlag[2] = true;
    mAvailable[3] = false;
    mStart_time[3] = millis();
  }
  if (measurementSelection && mAvailable[measurementSelection-1]){
    for (int i=0; i<3; i++)
      taskAddFlag[i+2] = true;
    mAvailable[measurementSelection-1] = false;
    mStart_time[measurementSelection-1] = millis();
  }
  for (int i=0; i<numTask; i++){
    if (taskAddFlag[i]==false && taskInQue[i]==true){
      Delete(taskArray[i]);
      taskInQue[i] = false;
    }
    else if (taskAddFlag[i]==true && taskInQue[i]==false){
      Insert(taskArray[i]);
      taskInQue[i] = true;
    }
  }
  currentTask = head;
  while (currentTask != NULL){
    start_time = millis();
    (currentTask->myTask)(currentTask->taskDataPtr); //execute task
    taskTime[*(unsigned char *)(currentTask->taskDataPtr)] = millis() - start_time;
    currentTask = currentTask->next;
  }
  //toggle pin after one cycle of task que
  digitalWrite(taskqueFinishPin, !digitalRead(taskqueFinishPin));
  //show execution time for each task in serial monitor
  message = "";
  for (int i=0; i<numTask; i++)
    message += taskName[i] + ": " + taskTime[i] + " ms\n";
  Serial.write(message.c_str());
}
