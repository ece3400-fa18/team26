/*
 * Todo: Tune Servo
 */

#include <Servo.h>

#define LEFT 70
#define RIGHT 71
#define FORWARD 72
#define BACKWARD 73

// These constants won't change. They're used to give names to the pins used:
//const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int LW = 9; // Servo1
const int RW = 10; // Servo2

const int Lline = A0;
const int Rline = A1;

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


//turn right 90 degree
//black 1000
// gray 450
// white 100 
void turnGrid(){
  //if both line sensors detect a white
  //then turn until the left sensor sees white in the third time
  int Lline_val = analogRead(line1);
  int Rline_val = analogRead(line2);
  if (Lline_val <-100 && Rline_val <= 100){
    //left sensor will see white->gray->white->gray->white (stop)
    analogWrite(LW, 135);
    analogWrite(RW, 90);
    while(Lline_val <=100);
    while(Lline_val <=500 && Lline_val >100);
    while(Lline_val <= 100);
    while(Lline_val <=500 && Lline_val >100);
    if(Lline_val <= 100){
      drive(FORWARD);
    }
  }
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
