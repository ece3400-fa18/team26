#include <Servo.h>

#define LEFT 70
#define RIGHT 71
#define FORWARD 72
#define BACKWARD 73

// These constants won't change. They're used to give names to the pins used:
//const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int LW = 5; // Servo1
const int RW = 3; // Servo2

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(LW, OUTPUT);
  pinMode(RW, OUTPUT);
}

void loop() {
  analogWrite(LW, 135);
  analogWrite(RW, 135);
//  drive(FORWARD);
//  delay(3000);
//  turn(RIGHT);
//  delay(3000);
}

void halt(){
     analogWrite(LW, 90);
     analogWrite(RW, 90);
}

void turn(int dir){
  if(dir == RIGHT){
    analogWrite(LW, 135);
    analogWrite(RW, 45);
  }
  if(dir == LEFT){
    analogWrite(LW, 45);
     analogWrite(RW, 135);
  }
}

void drive(int dir){
  if(dir == FORWARD){
    analogWrite(LW, 135);
     analogWrite(RW, 135);
  }
  if(dir == BACKWARD){
    analogWrite(LW, 45);
    analogWrite(RW, 45);
  }
}
