#include "GoBLE.h"  
#include <Servo.h>  
#include <Metro.h>  
#include "DFMobile.h"  

Metro measureDistance = Metro(50);  
Metro sweepServo = Metro(20);  
DFMobile Robot (7,6,4,5);  
int LeftWheelSpeed;  
int RightWheelSpeed;  
int speedPin_M1 = 5;
int speedPin_M2 = 6;
int directionPin_M1 = 4;  
int directionPin_M2 = 7;  

unsigned long actualDistance = 0;  
Servo myservo;  
int pos = 60;  
int sweepFlag = 1;  

int URPWM = 3;  
int URTRIG = 10;  
uint8_t EnPwmCmd[4] = {0x44, 0x02, 0xbb, 0x01};  
bool modeAutomatique = false;  
int vitesse = 100;  
int joystickX, joystickY;  
int buttonState[7];  
int ledPin = 13;  

void setup() {  
  myservo.attach(9);  
  Robot.Direction (LOW,LOW);  
  pinMode(ledPin,OUTPUT);  
  Serial.begin(115200);  
  Goble.begin();  

  pinMode(speedPin_M1, OUTPUT);  
  pinMode(directionPin_M1, OUTPUT);  
  pinMode(speedPin_M2, OUTPUT);  
  pinMode(directionPin_M2, OUTPUT);  
  SensorSetup();  
  delay(1000);  
}  

void loop() {  
  if (Goble.available()) {  
//int joystickY = Goble.readJoystickY();  
//vitesse = map(joystickY, 0, 255, 50, 255);  
  if (Goble.readSwitchSelect() == PRESSED) {  
  modeAutomatique = false;  
  carStop();  
  delay(100);  
  }  

  if (Goble.readSwitchStart() == PRESSED) {  
    modeAutomatique = true;  
    carStop();  
    delay(100);  
    }  
  }  
if (modeAutomatique) {  
modeAuto();  
} else {  
modeManuel();  
}  
}  

void modeManuel() {  
joystickX = Goble.readJoystickX();  
joystickY = Goble.readJoystickY();  
buttonState[SWITCH_UP] = Goble.readSwitchUp();  
buttonState[SWITCH_LEFT] = Goble.readSwitchLeft();  
buttonState[SWITCH_RIGHT] = Goble.readSwitchRight();  
buttonState[SWITCH_DOWN] = Goble.readSwitchDown();  
buttonState[SWITCH_SELECT] = Goble.readSwitchSelect();  
buttonState[SWITCH_START] = Goble.readSwitchStart();  
int SpeedX=2*joystickX-256;  
int SpeedY=2*joystickY-256;  
if (SpeedX>200 || SpeedX<-200){  
LeftWheelSpeed=SpeedX;  
RightWheelSpeed=SpeedX;  
Robot.Speed (LeftWheelSpeed,RightWheelSpeed);  
}  
else if (SpeedY>200 || SpeedY<-200){  
LeftWheelSpeed=SpeedY-80;  
RightWheelSpeed=-SpeedY-80;  
Robot.Speed(LeftWheelSpeed,RightWheelSpeed);  
}  
else if (SpeedX==0 && SpeedY==0){  
Robot.Speed(0,0);  
}  
if (buttonState[1] == PRESSED){  
digitalWrite(ledPin,HIGH);  
}  
if (buttonState[1] == RELEASED){  
digitalWrite(ledPin,LOW);  
}  
}  

void modeAuto() {  
if (measureDistance.check()) {  
actualDistance = MeasureDistance();  
delay(100);  
}  
if (sweepServo.check()) {  
servoSweep();  
}  
if (Goble.available() && Goble.readSwitchSelect() == PRESSED) {  
modeAutomatique = false;  
carStop();  
return;  
}  

if (actualDistance <= 25) {             //alter this value to change sensor's sensitivity  
    carStop();
    delay (1000);
    carAdvance(90, 90);                 //alter these values to change forward speed
    delay(300);

    myservo.write(45);
    int DistanceDroite = MeasureDistance();
    delay(1000);

    myservo.write(135);
    int DistanceGauche = MeasureDistance();
    delay(1000);

    if (DistanceDroite >= DistanceGauche) {
      carTurnLeft(150, 150);             //alter these values to change turn right speed
      delay(500);  
    } else {
      carTurnRight(150, 150);              //alter these values to change turn left speed
      delay(500);
    }
    myservo.write(90);
  } else {
    carAdvance(90,90);                 //alter these values to change forward speed
    delay(300);
  }

}  

void SensorSetup() {
  pinMode(URTRIG, OUTPUT);                    // A low pull on pin COMP/TRIG
  digitalWrite(URTRIG, HIGH);                 // Set to HIGH
  pinMode(URPWM, INPUT);                      // Sending Enable PWM mode command
  for (int i = 0; i < 4; i++) {
    Serial.write(EnPwmCmd[i]);
  }
}

int MeasureDistance() { // a low pull on pin COMP/TRIG  triggering a sensor reading
  digitalWrite(URTRIG, LOW);
  digitalWrite(URTRIG, HIGH);               // reading Pin PWM will output pulses
  unsigned long distance = pulseIn(URPWM, LOW);
  if (distance == 1000) {          // the reading is invalid.
    Serial.print("Invalid");
  } else {
    distance = distance / 50;       // every 50us low level stands for 1cm
  }
  return distance;
}

void carStop() {                //  Motor Stop
  digitalWrite(speedPin_M2, 0);
  digitalWrite(directionPin_M1, LOW);
  digitalWrite(speedPin_M1, 0);
  digitalWrite(directionPin_M2, LOW);
}

void carBack(int leftSpeed, int rightSpeed) {       //Move backward
  analogWrite (speedPin_M2, leftSpeed);             //PWM Speed Control
  digitalWrite(directionPin_M1, LOW);              //set LOW to reverse or HIGH to advance
  analogWrite (speedPin_M1, rightSpeed);
  digitalWrite(directionPin_M2, HIGH);
}

void carAdvance(int leftSpeed, int rightSpeed) {     //Move forward
  analogWrite (speedPin_M2, leftSpeed);
  digitalWrite(directionPin_M1, HIGH);
  analogWrite (speedPin_M1, rightSpeed);
  digitalWrite(directionPin_M2, LOW);
}

void carTurnLeft(int leftSpeed, int rightSpeed) {           //Turn Left
  analogWrite (speedPin_M2, leftSpeed);
  digitalWrite(directionPin_M1, HIGH);
  analogWrite (speedPin_M1, rightSpeed);
  digitalWrite(directionPin_M2, HIGH);
}
void carTurnRight(int leftSpeed, int rightSpeed) {         //Turn Right
  analogWrite (speedPin_M2, leftSpeed);
  digitalWrite(directionPin_M1, LOW);
  analogWrite (speedPin_M1, rightSpeed);
  digitalWrite(directionPin_M2, LOW);
}

void servoSweep() {
  if (sweepFlag) {
    if (pos >= 30 && pos <= 120) {
      pos = pos + 15;                              // in steps of 1 degree
      myservo.write(pos);                         // tell servo to go to position in variable 'pos'
    }
    if (pos > 119)  sweepFlag = false;                    // assign the variable again
  }
  else {
    if (pos >= 30 && pos <= 120) {
      pos = pos - 15;
      myservo.write(pos);
    }
    if (pos < 61)  sweepFlag = true;
  }
}
