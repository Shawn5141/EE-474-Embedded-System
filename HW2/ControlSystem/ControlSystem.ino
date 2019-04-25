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
void Measure_function(void *data){}
void Compute_function(void *data){}
void Display_function(void *data){}
void WarningAlarm_function(void *data){}
void Status_function(void *data){}
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
