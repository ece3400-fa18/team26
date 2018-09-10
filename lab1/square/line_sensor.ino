#include <Servo.h>

#define LEFT 70
#define RIGHT 71
#define FORWARD 72
#define BACKWARD 73

// These constants won't change. They're used to give names to the pins used:
const int line_L = A0;  //line sensor on left side
const int line_R = A1;
const int LW = 5; // Servo1
const int RW = 3; // Servo2
int val;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(LW, OUTPUT);
  pinMode(RW, OUTPUT);
  pinMode(line_L, INPUT);
  pinMode(line_R, INPUT);
}

void loop() {
  val = analogRead(line); 
  delay (100); 
  analogWrite(LW, 135);
  analogWrite(RW, 135);
  drive(FORWARD);
  if (line_R <= 400){
    turn(RIGHT); 
    drive(FORWARD);
  }
  else if (line_L <= 400){
    turn (LEFT);
    drive(FORWARD);
  }
  if (line_R >= 1000 || line_L >= 1000){
    drive(BACKWARD)
    delay (500)
    turn(RIGHT); 
    drive(FORWARD);
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
