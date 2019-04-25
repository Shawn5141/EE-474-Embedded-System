
#include <string.h>

void Measure_function(void *data){

	data = (DataStructMeasure*)data;
	String  rawData;
	Serial.write('1');
	
if ( Serial.available()) {
    serialResponse = Serial.readStringUntil('\n');

    // Convert from String Object to String.
    char buf[sizeof(sz)];
    serialResponse.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    int value[4];
    int i = 0;
    while ((str = strtok_r(p, " ", &p)) != NULL) // delimiter is the semicolon
    {  
      Serial.println(toInt(str));
      value[i] = toInt(str);
      i++;
    }
    for(int j=0; j<4; j++) Serial.print(value[j]);
  }



  	*(data->temperatureRawPtr)value[0];
  	*(data->systolicRawPtr)=value[1];
  	*(data->diastolicRawPtr)=value[2];
  	*(data->bpRawPtr)=value[3];




}


void Compute_function(void *data){

	data = (DataStructCompute *)data; 
	*(data->tempCorretedPtr) = 5+0.75*(*(data->tempRawPtr));
	*(data->systolicRawPtr) = 9+2*(*(data->systolicRawPtr));
	*(data->diasCorretedPtr) = 6+1.5*(*(data->diastolicRawPtr)); 
	*(data->prCorrectedPtr) = 8+3*(*(data->bpRawPtr));



}

//For the TFT

// IMPORTANT: ELEGOO_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Elegoo_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

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

void setup(void) {
  Serial.begin(9600);
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

void Display_function(void *data){

   tft.setCursor(0, 30);
   tft.setTextSize(2);
   tft.setTextColor(WHITE,BLACK);
   tft.println("Blood Pressure: ");
   tft.print(" Systolic Pressure: ");
   if (pulseLow==true){
    tft.setTextColor(RED);
    tft.print(mySingle.y);
    tft.println(" mmHg");}
   else{
    tft.setTextColor(GREEN);
    tft.print(mySingle.y);
    tft.println(" mmHg");};
    
   tft.setTextColor(WHITE,BLACK);
   tft.print(" Diastolic Pressure:");
   if (pulseLow==true){
    tft.setTextColor(RED);
    tft.print(mySingle.y);
    tft.println(" mmHg");}
   else{
    tft.setTextColor(GREEN);
    tft.print(mySingle.y);
    tft.println(" mmHg");};
   
   


   tft.setTextColor(WHITE,BLACK);
   tft.print("Temperature:        ");
   if (tempHigh==true){tft.setTextColor(RED);}
   else{tft.setTextColor(GREEN);};
   tft.print(mySingle.y);
   tft.println(" degree C");

  
   tft.setTextColor(WHITE,BLACK);
   tft.print("Pulse Rate:         ");
   if (pulseLow==true){tft.setTextColor(RED);}
   else{tft.setTextColor(GREEN);};
   tft.print(mySingle.y);
   tft.println(" BPM ");

   
   tft.setTextColor(WHITE,BLACK);
   tft.print("Battery status:     ");
   if (batteryState<20){tft.setTextColor(RED);}
   else{tft.setTextColor(GREEN);};
   tft.print(mySingle.y);
   tft.println(" % ");


  tft.println();
  tft.println();

}


void Annuniciate(void *data){

	data = ( DataStructWarningAlarm *)data;
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
	
	if (*(data->pulseStatePtr)<60 || *(data->pulseStatePtr)>100){
		pulseOutOfRange = *(data->pulseStatePtr);
		if(*(data->pulseStatePtr)<60){
			pulseLow=true;
		}else{
			pulseLow=false;
		}

	}


}

void status(void*data){

	data=(DataStructStatus *)data;
	&data--;
}
