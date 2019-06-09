import controlP5.*;
import processing.serial.*;

Serial port;
ControlP5 cp5;
boolean initialize = false;
byte offset = 0;
String message=" ";
boolean E = false;
String measure = "";
String warning = "";
boolean [] flash;
boolean [] show;
int [] time;

void setup() {
  size(700,500);
  
  port = new Serial(this, "/dev/ttyACM0", 9600);
  
  PFont font = createFont("arial",20);
  
  cp5 = new ControlP5(this);
  
  cp5.addTextfield("input")
     .setPosition(20,100)
     .setSize(200,40)
     .setFont(font)
     .setFocus(true)
     .setColor(color(255,0,0))
     ;
     
  textFont(font);
  flash = new boolean[7];
  show = new boolean[7];
  time = new int[7];
}

void draw() {
  if (initialize){
    if (port.available()>0){
      message = port.readStringUntil('!');
      if (message != null){
      message = message.replace("!", "");
      if (message.equals("E"))
        E = true;
      else if (message.charAt(0) == 'w'){
        message = message.replace("w", "");
        warning = message;
      }
      else if (message.charAt(0) == 'M'){
        message = message.replace("M", "");
        String [] f = message.split("#");
        measure = f[0];
        String [] farray = f[1].split(" ");
        for (int i=0; i<farray.length; i++){
          //print(farray[i]);
          flash[i] = farray[i].equals("1");
        }
      }}
    }
  }
  refresh();
}

public void input(String theText) {
  // automatically receives results from controller input
  if (!initialize){
    if (theText.equals("I")){
      initialize = true;
    }
    return;
  }
  E = false;
  println("a textfield event for controller 'input' : "+theText);
  port.write(theText);
}

public void refresh(){
  background(0);
  if (initialize)
    text("Connected!", 20, 50);
  if (E)
    text("E", 20, 75);
  text("Warning data:", 20, 200);
  text(warning, 20, 225);
  String [] m = measure.split("\n");
  for (int i=0; i<m.length; i++){
    if (!flash[i]){
      show[i] = true;
    }
    else if (millis() - time[i] >= 500){
      show[i] = !show[i];
      time[i] = millis();
    }
    if (show[i])
      text(m[i], 350, 50 + i * 25);
  }
}
