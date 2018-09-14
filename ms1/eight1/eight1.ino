#include <Servo.h>

Servo left_servo;
Servo right_servo;

////directions
//#define right_turn 4
//#define right_pid  3
//#define left_pid   2
//#define left_turn  1

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  100.0
#define SERVO_R_FORWARD_MAX  80.0
#define SERVO_L_INCR_FORWARD 2.0
#define SERVO_R_INCR_FORWARD -2.0

// Thresholds for each sensor to determine when over a line
#define RIGHT_PID_THRESH     900
#define LEFT_PID_THRESH      900
#define RIGHT_TURN_THRESH    700
#define LEFT_TURN_THRESH     700

#define ERROR_RANGE          100

//pins
const int LW = 9; // Servo1
const int RW = 10; // Servo2
const int right_turn = A3;
const int right_pid = A2;
const int left_pid = A1;
const int left_turn = A0;

// Control variables for line following
float error           = 0;
float error_magnitude = 0;
int   counter         = 0;

// Line sensor values
int right_pid_val     = 0; 
int left_pid_val      = 0;
int right_turn_val    = 0;
int left_turn_val     = 0;

// Servo turn values
float servo_turn_value[]   = {SERVO_R_FORWARD_MAX, 0 ,SERVO_L_FORWARD_MAX, SERVO_L_FORWARD_MAX};
int   servo_turn_delays [] = {300, 0, 300, 900};

void move(int direction){
  // Turn if requested
  if (direction != 1){
    // Start turning until off the current line
    left_servo.write(servo_turn_value[direction]);
    right_servo.write(servo_turn_value[direction]);
    delay(servo_turn_delays[direction]);

    // Read values from center line sensors
    right_pid_val = analogRead(right_pid);
    left_pid_val = analogRead(left_pid);

    // While center sensors are not over another line, continue turning
    while((right_pid_val< RIGHT_PID_THRESH) || (left_pid_val< LEFT_PID_THRESH)){
      right_pid_val = analogRead(right_pid);
      left_pid_val = analogRead(left_pid);
    }
  } // done turning

  // Reset intersection variables  
    right_turn_val = 0;
    left_turn_val = 0;
    counter = 50;
    
  // After turn is complete, drive forward to the next intersection, making corrections as necessary
  // while-loop contains all line-following code
  while(right_turn_val < RIGHT_TURN_THRESH || left_turn_val < LEFT_TURN_THRESH){

    // Wait 50 while-loops for robot to be clear of starting intersection
    // Don't start searching for an intersection until the intersection sensors are clear of black line
    if (counter > 0){
      right_turn_val = 0; //set so that while loop continues
      left_turn_val = 0;  //set so that while loop continues 
      counter = counter - 1;
    }
    else {
      right_turn_val = analogRead(right_turn); //signal from center right sensor
      left_turn_val = analogRead(left_turn); //signal from center left sensor    
    }

    // Read analog values from two center sensors
    right_pid_val = analogRead(right_pid); // signal from center right sensor
    left_pid_val = analogRead(left_pid);   // signal from center left sensor  

    // Print statements for debugging
      //Serial.println(right_pid_val);
      //Serial.println(left_pid_val);  
      //Serial.println();  
      
    error = left_pid_val - right_pid_val; // Positive position to right of line

    // Correct robot's driving direction according to position error
    if (abs(error) <= ERROR_RANGE){
      left_servo.write(SERVO_L_FORWARD_MAX);
      right_servo.write(SERVO_R_FORWARD_MAX);
    }
    
    // Robot is too right
    else if (error > ERROR_RANGE) {
      // Adjust left
      error_magnitude = abs(error)/(float)ERROR_RANGE;
      left_servo.write(SERVO_L_FORWARD_MAX - error_magnitude*SERVO_L_INCR_FORWARD);
      right_servo.write(SERVO_R_FORWARD_MAX + error_magnitude*SERVO_R_INCR_FORWARD);
    }
    // Robot is too left of line
    else if (error < -(ERROR_RANGE)) {
      // Adjust right
      error_magnitude = abs(error)/(float)ERROR_RANGE;
      left_servo.write(SERVO_L_FORWARD_MAX + error_magnitude*SERVO_L_INCR_FORWARD);
      right_servo.write(SERVO_R_FORWARD_MAX - error_magnitude*SERVO_R_INCR_FORWARD);
    }
    delay(10);
  }
  left_servo.write(SERVO_BRAKE);
  right_servo.write(SERVO_BRAKE);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  right_servo.attach(RW);
  left_servo.attach(LW);
  // Set up the select pins as outputs:

  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, HIGH);
  pinMode(A3, OUTPUT);
  digitalWrite(A3, HIGH);

  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);

  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  // move in figure-8
  move(1);   // forward one block
  delay(50);
  move(2);   // right one block
  delay(50);
  move(2);
  delay(50);
  move(2);
  delay(50);

  move(1);   
  delay(50);
  move(0);   // left one block
  delay(50);
  move(0);
  delay(50);
  move(0);
  delay(50);
}
