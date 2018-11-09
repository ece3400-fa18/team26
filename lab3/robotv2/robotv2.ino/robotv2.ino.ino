/*
fft_adc_serial.pde
guest openmusiclabs.com 7.7.14
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.
*/

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <Servo.h>
#include <FFT.h>

Servo left_servo;
Servo right_servo;

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  95.0
#define SERVO_R_FORWARD_MAX  85.0
#define SERVO_L_INCR_FORWARD 2.0
#define SERVO_R_INCR_FORWARD -2.0

// Thresholds for each sensor to determine when over a line
#define WHITE 700

#define ERROR_RANGE 100

#define left    0
#define forward 1
#define right   2

//pins
const int LW = 5; // Servo1
const int RW = 6; // Servo2
const int right_turn = A5;
const int left_turn = A4;
const int front_wall = A2;

// Control variables for line following
float error           = 0;
float error_magnitude = 0;
int   i = 0;

// Line sensor values
int right_turn_val  = 0;
int left_turn_val   = 0;

//control variables for turning
int turn_count = 0;

const int irBinNum = 44;
const int irThresh = 50;
const int micBinNum = 17;
const int micThresh = 130;

// location and direction values
volatile int unsigned long x = 0; //initial x location
volatile int unsigned long y = 0; //initial y location
int facing = 3; //initially facing NORTH
int direction = 0; //initially going straight
volatile int unsigned long d = 0;
int unsigned long updates[3] = {x,y,d};

void servos_stop(){
  Serial.println("stop");
  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);
  delay(3000);
}

void read_turn(){
  right_turn_val = analogRead(right_turn); //signal from outer right sensor
  left_turn_val = analogRead(left_turn); //signal from outer left sensor 
}

void move(int direction){
  // Turn if requested
    if(direction == right){
      Serial.println("begin turn right");
      updateFacing(&facing, 0);
      left_servo.write(95);
      right_servo.write(95);
      while(analogRead(left_turn) > WHITE);
      while(analogRead(left_turn) < WHITE);
      Serial.println("done turn right");
    }
    
    if(direction == left){
      Serial.println("begin turn left");
      updateFacing(&facing, 1);
      left_servo.write(85);
      right_servo.write(85);
      while(analogRead(right_turn) > WHITE);
      while(analogRead(right_turn) < WHITE);
      Serial.println("done turn left");
    }
}

void find_intersection(){
  //look for intersection
    if (left_turn_val < WHITE && right_turn_val < WHITE){
      while(left_turn_val > WHITE && right_turn_val > WHITE);
      Serial.println("found intersection");
      //intersection cross
//       i = 0;
//       Serial.println("found intersection");
//       while(i < 30){
//         go_straight();
//         i++;
//       }
//       break; //both turn sensors are on white line

      //UPDATE LOCATION HERE
      updateLocation(facing);


      Serial.println("...................INTERSECTION.................");
      Serial.println(x);
      Serial.println(y);
      Serial.println("................................................");
    }
}

void go_straight(){
  find_intersection();
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


bool wallDetected(){
  int distance = analogRead(front_wall);
  //Serial.println(distance);
  if (distance > 305){
    d = 10000000;
    if (facing == 3){
      d = d|1000;
    } else if (facing == 2){
      d = d|100;
    } else if (facing == 1){
      d = d|10;
    } else if (facing == 0){
      d = d|1;
    }
  }
  updates[0] = x;
  updates[1] = y;
  updates[2] = d;
  return (distance > 305);
}

void detectAudio(){
  
  byte prevTIMSK0 = TIMSK0;
  byte prevADCSRA = ADCSRA;
  byte prevADMUX = ADMUX;
  byte prevDIDR0 = DIDR0;

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  while(1){
    Serial.println("while1");
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while (!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf7; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i + 1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();
    Serial.println(fft_log_out[micBinNum]);
   if (fft_log_out[micBinNum] > micThresh){
    Serial.println(fft_log_out[micBinNum]);
    break;
   } 
  }
  TIMSK0 = prevTIMSK0;
  ADCSRA = prevADCSRA;
  ADMUX = prevADMUX;
  DIDR0 = prevDIDR0;
}

void updateLocation(int facing){
  //based on the previous (x,y) location and the direction the robot is facing,
  //update the new (x,y) coordinates
  if (facing == 3){ //facing north
    y = y+1;
  }
  if (facing == 2){ //facing east
    x = x+1;
  }
  if (facing == 1){ //facing south
    y = y-1;
  }
  if (facing ==0){ //facing west
    x = x-1;
  }
}

void updateFacing(int *facing, int turningDirection){
  //based on if the robot is turning left or right,
  //update the direction the robot is facing.
  if (*facing == 3){ //initially north
    if (turningDirection == 1){ //turning left 
      *facing = 0; //now facing west
    } else if (turningDirection == 0) { //turning right
      *facing = 2; //now facing east
      }
  }
    if (*facing == 2){ //initially east
      if (turningDirection == 1){ //turning left 
        *facing = 3; //now facing north
      } else if (turningDirection == 0) { //turning right
        *facing = 1; //now facing south
        }
    }
    if (*facing == 1){ //initially south
      if (turningDirection == 1){ //turning left 
        *facing = 2; //now facing east
      } else if (turningDirection == 0) { //turning right
        *facing = 0; //now facing west
        }
    }
    if (*facing == 0){ //initially west
      if (turningDirection == 1){ //turning left 
        *facing = 1; //now facing south
      } else if (turningDirection == 0) { //turning right
        *facing = 3; //now facing north
        }
     }
}

void setup() {
  Serial.begin(9600); // use the serial port
  detectAudio();
  
  right_servo.attach(RW);
  left_servo.attach(LW);

  //line sensor
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode(front_wall, INPUT);
}

void loop() {
  if (wallDetected()){
    Serial.println("...........update...........");
    Serial.println(updates[0]);
    Serial.println(updates[1]);
    Serial.println(updates[2]);
    Serial.println("............................");
    //Serial.println(turn_count);
    if(turn_count == 0){
      move(left);
    }else if(turn_count == 1){
      move(right);
      move(right);
    }else if(turn_count == 2){
      //reach dead end
      move(right);
      turn_count = 0;
    }
    turn_count++;
  }else{
    go_straight();
    turn_count = 0;
  }
}
