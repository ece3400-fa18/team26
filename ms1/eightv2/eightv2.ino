#include <Servo.h>

Servo left_servo;
Servo right_servo;

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  95.0
#define SERVO_R_FORWARD_MAX  85.0
#define SERVO_L_INCR_FORWARD 2.0
#define SERVO_R_INCR_FORWARD -2.0

// Thresholds for each sensor to determine when over a line
#define DARK 900
#define WHITE 700

#define ERROR_RANGE          100

#define left 0
#define forward 1
#define right 2

//pins
const int LW = 5; // Servo1
const int RW = 6; // Servo2
const int right_turn = A5;
const int left_turn = A4;

// Control variables for line following
float error           = 0;
float error_magnitude = 0;
int   i = 0;

volatile long SENSOR0_TIMER;
volatile long SENSOR1_TIMER;

// Line sensor values
int right_turn_val    = 0;
int left_turn_val     = 0;

void read_turn(){
  right_turn_val = analogRead(right_turn); //signal from outer right sensor
  left_turn_val = analogRead(left_turn); //signal from outer left sensor 
}

void move(int direction){
  //look for intersection
  while(1){
    Serial.println("looking");
    read_turn();
    if (left_turn_val < WHITE && right_turn_val < WHITE){
      i = 0;
      Serial.println("found intersection");
      while(i < 30){
        go_straight();
        i++;
      }
      break;
    }
    go_straight();
    delay(10);
  }//close while, found intersection
  
  // Turn if requested, white=700
    if(direction == right){
      Serial.println("TURN RIGHT");
      left_servo.write(95);
      right_servo.write(95);
      while(analogRead(left_turn) > WHITE);
      while(analogRead(left_turn) < WHITE);
      while(analogRead(left_turn) > WHITE);
      Serial.println("done turn right");
    }
    
    if(direction == left){
      Serial.println("TURN LEFT");
      left_servo.write(85);
      right_servo.write(85);
      while(analogRead(right_turn) > WHITE);
      while(analogRead(right_turn) < WHITE);
      while(analogRead(right_turn) > WHITE);
      Serial.println("done turn left");
    }
  
  go_straight();
  delay(50);
  
}

void go_straight(){
  //modified from solution code
  //following straight line
  read_turn();
  error = left_turn_val - right_turn_val; // Positive position to right of line

  // Correct robot's driving direction according to position error
  if (abs(error) <= ERROR_RANGE){
    left_servo.write(SERVO_L_FORWARD_MAX);
    right_servo.write(SERVO_R_FORWARD_MAX);
  }
  
    // Robot is too right
    else if (error > ERROR_RANGE) {
      // Adjust left
      error_magnitude = abs(error)/(float)ERROR_RANGE;
      left_servo.write(SERVO_L_FORWARD_MAX + error_magnitude*SERVO_L_INCR_FORWARD);
      right_servo.write(SERVO_R_FORWARD_MAX - error_magnitude*SERVO_R_INCR_FORWARD);
    }
    // Robot is too left of line
    else if (error < -(ERROR_RANGE)) {
      // Adjust right
      error_magnitude = abs(error)/(float)ERROR_RANGE;
      left_servo.write(SERVO_L_FORWARD_MAX - error_magnitude*SERVO_L_INCR_FORWARD);
      right_servo.write(SERVO_R_FORWARD_MAX + error_magnitude*SERVO_R_INCR_FORWARD);
    }
    delay(10);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  right_servo.attach(RW);
  left_servo.attach(LW);

  //for analog setup
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 

  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);

  delay(1000);
}


void loop() {
//  move(left);
//  delay(50);
//  move(left);
//  delay(50);
//  move(left);
//  delay(50);
//  move(left);
//  delay(50);
//  
//  move(right);
//  delay(50);
//  move(right);
//  delay(50);
//  move(right);
//  delay(50);
//  move(right);
//  delay(50);
  move(right);
  delay(50); 
  
}
