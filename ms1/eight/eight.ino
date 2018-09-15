#include <Servo.h>

Servo left_servo;
Servo right_servo;

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  100.0
#define SERVO_R_FORWARD_MAX  80.0
#define SERVO_L_INCR_FORWARD -2.0
#define SERVO_R_INCR_FORWARD 2.0

// Thresholds for each sensor to determine when over a line
#define RIGHT_PID_THRESH     900
#define LEFT_PID_THRESH      900
#define RIGHT_TURN_THRESH    700
#define LEFT_TURN_THRESH     700

#define ERROR_RANGE          100

#define left 0
#define forward 1
#define right 2

const int free_range = 50;

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


void read_pid(){
  right_pid_val = analogRead(right_pid);
  left_pid_val = analogRead(left_pid);
}

void read_turn(){
  right_turn_val = analogRead(right_turn); //signal from outer right sensor
  left_turn_val = analogRead(left_turn); //signal from outer left sensor 
}

void move(int direction){
  // Turn if requested
  if (direction != 1){
    left_servo.write(servo_turn_value[direction]);
    right_servo.write(servo_turn_value[direction]);
    delay(servo_turn_delays[direction]); //force turn over line

    // While center sensors are not over another line, continue turning
    read_pid();
    while((right_pid_val> RIGHT_PID_THRESH) && (left_pid_val> LEFT_PID_THRESH)){
      read_pid();
    } //end while, done rutning
  } 

  //looking for intersection
  right_turn_val = 0;
  left_turn_val = 0;
  counter = free_range;
  
  while(right_turn_val < RIGHT_TURN_THRESH || left_turn_val < LEFT_TURN_THRESH){
    if (counter > 0){ //force forward
      right_turn_val =0; 
      left_turn_val = 0;  
      counter = counter - 1;
    }
    else {
      read_turn();
    }

    go_straight();
    delay(10);
  }//close while; found intersection

  //Serial.println("found intersection");
  left_servo.write(SERVO_BRAKE);
  right_servo.write(SERVO_BRAKE);
}

void go_straight(){
  //following straight line
  read_pid();
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
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  right_servo.attach(RW);
  left_servo.attach(LW);

  /*
  To be tested;
  */
  //for analog setup
  pinMode(right_turn, INPUT);
  pinMode(right_pid, INPUT);
  pinMode(left_pid, INPUT);
  pinMode(left_turn, INPUT);

  //for digital setup turn on
  digitalWrite(right_pid, HIGH);
  digitalWrite(left_pid, HIGH);
  digitalWrite(right_tunr, HIGH);
  digitalWrite(left_turn, HIGH);

  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);

  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  // move in figure-8
  move(forward);   // forward one block
  delay(free_range);
  move(left);
  delay(free_range);
  move(left);
  delay(free_range);
  move(left);
  delay(free_range);

  move(forward);   // forward one block
  delay(free_range);
  move(right);
  delay(free_range);
  move(right);
  delay(free_range);
  move(right);
  delay(free_range);
}
