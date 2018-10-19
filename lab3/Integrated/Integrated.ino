//Integrated Start with 660, IR Stop, Wall and Line Sensing
#define LOG_OUT 1 
#define FFT_N 256 

#include <FFT.h>
#include <Servo.h>

Servo left_servo;
Servo right_servo;

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  95.0
#define SERVO_R_FORWARD_MAX  85.0
#define SERVO_L_INCR_FORWARD 2.0
#define SERVO_R_INCR_FORWARD -2.0

// Thresholds for each sensor to determine when over a line
#define DARK  900
#define WHITE 700

#define ERROR_RANGE 100

#define left    0
#define forward 1
#define right   2

//pins
const int LW = 9; // Servo1
const int RW = 10; // Servo2
const int right_turn = A5;
const int left_turn = A4;
const int front_wall = A2;
const int started = 0; 

// Control variables for line following
float error           = 0;
float error_magnitude = 0;
int   i = 0;

volatile long SENSOR0_TIMER;
volatile long SENSOR1_TIMER;

// Line sensor values
int right_turn_val  = 0;
int left_turn_val   = 0;

void read_turn(){
  right_turn_val = analogRead(right_turn); //signal from outer right sensor
  left_turn_val = analogRead(left_turn); //signal from outer left sensor 
}

void move(int direction){
  // Turn if requested
    if(direction == right){
      left_servo.write(95);
      right_servo.write(95);
      while(analogRead(left_turn) > WHITE);
      while(analogRead(left_turn) < WHITE);
      Serial.println("done turn right");
    }
    
    if(direction == left){
      left_servo.write(85);
      right_servo.write(85);
      while(analogRead(right_turn) > WHITE);
      while(analogRead(right_turn) < WHITE);
      Serial.println("done turn left");
    }
  
  go_straight();
  delay(50);
  
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
      Serial.println("delay");
      break; //both turn sensors are on white line
    }
    go_straight();
    delay(10);
  }//close while; found intersection
  Serial.println("back to main");
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
    left_servo.write(SERVO_L_FORWARD_MAX - 2);//error_magnitude*SERVO_L_INCR_FORWARD*0.2);
    right_servo.write(SERVO_R_FORWARD_MAX - 2);//error_magnitude*SERVO_R_INCR_FORWARD*0.2);
  }
  // Robot is too left of line
  else if (error < -(ERROR_RANGE)) {
    // Adjust right
    error_magnitude = abs(error)/(float)ERROR_RANGE;
    left_servo.write(SERVO_L_FORWARD_MAX +2);
    right_servo.write(SERVO_R_FORWARD_MAX + 2);
  }
  delay(10);
}

bool wallDetected(){
  int distance = analogRead(front_wall);
  Serial.println(distance);
  return (distance > 400);
}

void servos_stop(){
  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);
}

int irBin = 45;
int irThresh = 40; 
int startBin = 22;
int startThresh = 80;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  TIMSK0 = 0;  //turn off timer0
  ADCSRA = 0xe5;  //set the adc to free running mode
  ADMUX = 0x40; //use adc0
  DIDR0 = 0x01;
    
  right_servo.attach(RW);
  left_servo.attach(LW);

  //for analog setup
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode(front_wall, INPUT);

  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);

  delay(1000);
}

void loop() {
  while(1) { 
    cli();  
    for (int i = 0 ; i < 512 ; i += 2) { 
      while(!(ADCSRA & 0x10)); 
      ADCSRA = 0xf5; 
      ADCSRA = 0xf6; 
      byte m = ADCL; 
      byte j = ADCH;
      int k = (j << 8) | m; 
      k -= 0x0200; 
      k <<= 6; 
      fft_input[i] = k; 
      fft_input[i+1] = 0; 
    }
    fft_window(); 
    fft_reorder(); 
    fft_run(); 
    fft_mag_log(); 
    sei();

    if (fft_log_out[startBin] > startThresh) {
        started = 1;
          }
        while(started){
          if (fft_log_out[irBin] > irThresh) {
            servos_stop();
          } else {
            go_straight();    
            if (wallDetected() == true){
              move(left);
            if(wallDetected() == true){
              move(left);
            }
            else go_straight;
            }
        }   
     }
  }
}
