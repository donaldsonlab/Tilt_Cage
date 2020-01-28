#include <Servo.h>

// Pin Names
const int userButton = A0; // user stop button
const int leftButton = A2;
const int rightButton = A3;
const int doorCloseLeft = A4; //Switch for stopping left door
const int doorCloseRight = A6; //Switch for stopping right door
const int tiltSwitch = A5;

////Transmit Pins
//const int dispenseOut = 23; //high when pellet should be dispensed, connects to 3 on other Arduino

////Receive Pins
//const int dispenseIn = 24; //high when pellet is dispensed, connects to 7 on other Arduino
//const int retrieveIn = 25; //high when pellet is retrieved, connects to 8 on other Arduino

//Tilt Motor
const int tiltSlp = 11; // reset and sleep driver pin
const int tiltStp = 12; // stepper motor pwm
const int tiltDir = 13; // direction driver pin

// Servos
Servo leftServo;
Servo rightServo;
Servo leftSwitch;
Servo rightSwitch;

//Global Variables
int userRead = 1;
int userReadPrev;
int doorCloseLeftRead;
int doorCloseRightRead;

//////////////// USER DEFINED VARIABLES /////////////////
int up = 0; //Start the cage flat
int tiltSwitchRead;
int retractL = 106; //Servo positions to retract levers
int retractR = 42;
int extrudeL = 48; //Servo positions to extrude levers
int extrudeR = 100;
/////////////////////////////////////////////////////////

void setup(){
  Serial.begin(9600);

  //Button
  pinMode(userButton,INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);

  //Switch Setup
  pinMode(doorCloseLeft,INPUT_PULLUP);
  pinMode(doorCloseRight,INPUT_PULLUP);
  pinMode(tiltSwitch, INPUT_PULLUP);
   
  //Door Lever Driver Setup
  pinMode(tiltSlp, OUTPUT);
  digitalWrite(tiltSlp,LOW);
  pinMode(tiltStp, OUTPUT);
  digitalWrite(tiltStp,LOW);
  pinMode(tiltDir, OUTPUT);
  digitalWrite(tiltDir,LOW);

  //Servo Setup
  leftServo.attach(10);
  rightServo.attach(8);
  leftSwitch.attach(7);
  rightSwitch.attach(6);
  
  leftServo.write(95); //Initialize doors stopped
  rightServo.write(90);
//  leftSwitch.write(extrudeL); //Initialize switches extruded
//  rightSwitch.write(extrudeR);
  leftSwitch.write(retractL); //Initialize switches retracted
  rightSwitch.write(retractR);

  //Start the doors closed
  userRead = digitalRead(userButton);
  doorCloseLeftRead = digitalRead(doorCloseLeft);
  doorCloseRightRead = digitalRead(doorCloseRight);
  if(doorCloseLeftRead == 1){
    closeLeft();
  }
  if(doorCloseRightRead == 1){
    closeRight();
  }
  delay(1000);
}
  
void loop(){
//  int leftRead = digitalRead(leftButton);
//  int rightRead = digitalRead(rightButton);

//  retractSwitches();
//  delay(2000);

  openLeft();
//  delay(500);
//  openRight();
  delay(30000);
  tiltUp();
  delay(15000);
  closeLeft();
//  delay(1000);
//  closeRight();
  delay(5000);
  tiltDown();
  delay(15000);
  
//  extrudeSwitches();
//  delay(2000);
} // void loop end


void tiltUp(){
  if (up == 0){ // Only execute if cage is down
    Serial.println("UP");
    //Tilt cage up
    tiltSwitchRead = digitalRead(tiltSwitch);
    digitalWrite(tiltDir,LOW);
    digitalWrite(tiltSlp,HIGH);
    while(tiltSwitchRead == 1){
      digitalWrite(tiltStp,HIGH);
      delay(1);
      digitalWrite(tiltStp,LOW);
      delay(1);
      tiltSwitchRead = digitalRead(tiltSwitch);
    }
    digitalWrite(tiltSlp, LOW);
    up = 1;  //Cage is up
  }
}

void tiltDown(){
  if (up == 1){ // Only execute if cage is up
    //Tilt cage down
    Serial.println("DOWN");
    digitalWrite(tiltDir,HIGH);
    digitalWrite(tiltSlp,HIGH);
    for(int k=1; k<=5350; k++){ // 4800 nominal
          digitalWrite(tiltStp,HIGH);
          delay(1);
          digitalWrite(tiltStp,LOW);
          delay(1);
          }
    digitalWrite(tiltSlp, LOW);
    up = 0;  //Cage is down
  }
}

void closeLeft(){
  Serial.println("CLOSE L");
  doorCloseLeftRead = digitalRead(doorCloseLeft);
  leftServo.write(180);
  while (doorCloseLeftRead == 1){ // while the switch is unpressed
    doorCloseLeftRead = digitalRead(doorCloseLeft);
    userReadPrev = userRead;
    delay(10);
    userRead = digitalRead(userButton);
    if((userRead == 0) &&(userReadPrev == 1)){ // if the user presses the button, stop the door
      Serial.println("USER PRESS");
      leftServo.write(95);
      delay(100);
      userStop();
      delay(10);
      leftServo.write(180);
    }
  }
  leftServo.write(95);
}

void openLeft(){
  Serial.println("OPEN");
  leftServo.write(0);
  delay(3000);
  leftServo.write(95);
}


void closeRight(){
  Serial.println("CLOSE R");
  doorCloseRightRead = digitalRead(doorCloseRight);
  rightServo.write(0);
  while (doorCloseRightRead == 1){ //while the switch is unpressed
    doorCloseRightRead = digitalRead(doorCloseRight);
    userReadPrev = userRead;
    delay(10);
    userRead = digitalRead(userButton);
    if((userRead == 0) && (userReadPrev == 1)){ // if the user presses the button, stop the door
      Serial.println("USER PRESS");
      rightServo.write(90);
      delay(100);
      userStop();
      delay(10);
      rightServo.write(0);
    }
  }
  rightServo.write(90);
}

void openRight(){
  Serial.println("OPEN R");
  rightServo.write(180);
  delay(3000);
  rightServo.write(90);
}

void retractSwitches(){
  leftSwitch.write(retractL);
  rightSwitch.write(retractR);
}
void extrudeSwitches() {
  leftSwitch.write(extrudeL);
  rightSwitch.write(extrudeR);
}

void userStop(){ // Keep doors stopped until user presses the button again
  userRead = digitalRead(userButton);
  while(userRead == 0){
    userRead = digitalRead(userButton);
  }
  delay(10);
  userRead = digitalRead(userButton);
  while(userRead == 1){
    userRead = digitalRead(userButton);
//    Serial.println(userRead);
  }
  Serial.println(userRead);
}
