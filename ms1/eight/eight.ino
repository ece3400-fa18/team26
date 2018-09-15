#include <Servo.h>

Servo left_servo;
Servo right_servo;

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  100.0
#define SERVO_R_FORWARD_MAX  80.0
#define SERVO_L_INCR_FORWARD -2.0
#define SERVO_R_INCR_FORWARD 2.0

// Thresholds for each sensor to determine when over a line
#define DARK 900
#define WHITE 200

#define ERROR_RANGE          100

#define left 0
#define forward 1
#define right 2

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
int   i = 0;

// Line sensor values
int right_pid_val     = 0; 
int left_pid_val      = 0;
int right_turn_val    = 0;
int left_turn_val     = 0;

// Servo turn values
float servo_turn_value[]   = {SERVO_R_FORWARD_MAX, 0 ,SERVO_L_FORWARD_MAX};


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
  if (direction != forward){
    left_servo.write(servo_turn_value[direction]);
    right_servo.write(servo_turn_value[direction]);
    delay(300); //force turn over line

    while(1){
      read_pid();
      if (left_pid_val < WHITE && right_pid_val < WHITE){
        break; //found white line
      }
    } //end while, done rutning
  } 

  //look for intersection
  while(1){
    read_turn();
    if (left_turn_val < WHITE && right_turn_val < WHITE){
      break; //both turn sensors are on white line
    }
    go_straight();
    delay(50);
  }//close while; found intersection

}

void go_straight(){
  //modified from solution code
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

  //for analog setup
  pinMode(right_turn, INPUT);
  pinMode(right_pid, INPUT);
  pinMode(left_pid, INPUT);
  pinMode(left_turn, INPUT);

  //for digital setup turn on
  digitalWrite(right_pid, HIGH);
  digitalWrite(left_pid, HIGH);
  digitalWrite(right_turn, HIGH);
  digitalWrite(left_turn, HIGH);

  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);

  delay(1000);
  move(forward);
}


void loop() {
  while(i <4){
    move(left);
    i++;
  } 
  while(i < 8){
    move(right);
    i++;
    if (i == 7){
      i = 0;
    }
  }
}
