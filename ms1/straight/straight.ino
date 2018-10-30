/**
 * ECE3400 Fall 2018
 */

#include <Servo.h>

// Servo speed definitions
#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  95
#define SERVO_R_FORWARD_MAX  85
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

const int right_turn = A5;
const int right_pid = A5;
const int left_pid = A4;
const int left_turn = A4;

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
    // Read analog values from two center sensors
    right_pid_val = analogRead(right_pid); // signal from center right sensor
    left_pid_val = analogRead(left_pid);   // signal from center left sensor  
    // right_pid_val = digitalRead(right_pid); // signal from center right sensor
    // left_pid_val = digitalRead(left_pid);   // signal from center left sensor  

          
    error = left_pid_val - right_pid_val; // Positive position to right of line

    Serial.println(right_pid_val);
    Serial.println(left_pid_val);
    // Correct robot's driving direction according to position error
    if (abs(error) <= ERROR_RANGE){
      left_servo.write(SERVO_L_FORWARD_MAX);
      right_servo.write(SERVO_R_FORWARD_MAX);
    }
    
    // Robot is too right
    else if (error > ERROR_RANGE) {
      // Adjust left
      Serial.println("too right");
      error_magnitude = abs(error)/(float)ERROR_RANGE;
      left_servo.write(SERVO_L_FORWARD_MAX + error_magnitude*SERVO_L_INCR_FORWARD);
      right_servo.write(SERVO_R_FORWARD_MAX - error_magnitude*SERVO_R_INCR_FORWARD);
    }
    // Robot is too left of line
    else if (error < -(ERROR_RANGE)) {
      // Adjust right
      Serial.println("too left");
      error_magnitude = abs(error)/(float)ERROR_RANGE;
      left_servo.write(SERVO_L_FORWARD_MAX - error_magnitude*SERVO_L_INCR_FORWARD);
      right_servo.write(SERVO_R_FORWARD_MAX + error_magnitude*SERVO_R_INCR_FORWARD);
    }
    delay(10);
}

void setup() {
  Serial.begin(9600);
  right_servo.attach(6);
  left_servo.attach(5);
  // Set up the select pins as outputs:

  pinMode(right_turn, INPUT);
  pinMode(right_pid, INPUT);
  pinMode(left_pid, INPUT);
  pinMode(left_turn, INPUT);

  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);

  delay(1000);
}

void loop() {
  move();
}
