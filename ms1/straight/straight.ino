/**
 * ECE3400 Fall 2017
 * TA solution for Milestone 1: Line following 
 */

#include <Servo.h>

// Servo speed definitions
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

// Servo objects
Servo left_servo;
Servo right_servo;

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


/**
 * function move() 
 * follows a line
 **/
void move(){
      right_turn_val = analogRead(right_turn); //signal from center right sensor
      left_turn_val = analogRead(left_turn); //signal from center left sensor    


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

void setup() {
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

  // Wait at start for 1 second
  delay(1000);
}

void loop() {
  
  move();
}