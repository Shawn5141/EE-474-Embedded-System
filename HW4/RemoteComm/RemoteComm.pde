import controlP5.*; //import ControlP5 library
import processing.serial.*;

Serial port;

ControlP5 cp5; //create ControlP5 object
PFont font;

void setup(){ //same as arduino program

  size(320, 650);    //window size, (width, height)

  port = new Serial(this, "/dev/ttyACM0", 2000000);  //i have connected arduino to com3, it would be different in linux and mac os
  
  //lets add buton to empty window
  
  cp5 = new ControlP5(this);
  font = createFont("Calibri Light", 20);    // custom fonts for buttons and title
  
  cp5.addButton("reset")     //"red" is the name of button
    .setPosition(80, 50)  //x and y coordinates of upper left corner of button
    .setSize(160, 70)      //(width, height)
    .setFont(font)
  ;   

  cp5.addButton("blood_pressure")     //"yellow" is the name of button
    .setPosition(80, 150)  //x and y coordinates of upper left corner of button
    .setSize(160, 70)      //(width, height)
    .setFont(font)
  ;

  cp5.addButton("temperature")     //"blue" is the name of button
    .setPosition(80, 250)  //x and y coordinates of upper left corner of button
    .setSize(160, 70)      //(width, height)
    .setFont(font)
  ;
  
  cp5.addButton("pulse_rate")     //"alloff" is the name of button
    .setPosition(80, 350)  //x and y coordinates of upper left corner of button
    .setSize(160, 70)      //(width, height)
    .setFont(font)
  ;
  
  cp5.addButton("respiration_rate")     //"alloff" is the name of button
    .setPosition(80, 450)  //x and y coordinates of upper left corner of button
    .setSize(160, 70)      //(width, height)
    .setFont(font)
  ;
  
  cp5.addButton("acknowledge")     //"alloff" is the name of button
    .setPosition(80, 550)  //x and y coordinates of upper left corner of button
    .setSize(160, 70)      //(width, height)
    .setFont(font)
  ;
}

void draw(){  //same as loop in arduino

  background(150, 0 , 150); // background color of window (r, g, b) or (0 to 255)
  
  //lets give title to our window
  fill(0, 255, 0);               //text color (r, g, b)
  textFont(font);
  text("Medical Device", 90, 30);  // ("text", x coordinate, y coordinat)
  if (port.available()>0){
    print(port.readString());
  }
}

//lets add some functions to our buttons
//so whe you press any button, it sends perticular char over serial port

void reset(){
  port.write('0');
}

void blood_pressure(){
  port.write('1');
}

void temperature(){
  port.write('2');
}

void pulse_rate(){
  port.write('3');
}

void respiration_rate(){
  port.write('4');
}

void acknowledge(){
  port.write('5');
}
