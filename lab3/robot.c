/*
fft_adc_serial.pde
guest openmusiclabs.com 7.7.14
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.
*/

#include <Servo.h>
#include <FFT.h> // include the library

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

//LAB 3
#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#define FFT_DATA 7
#define FFT_CTRL 8


int detectingAudio = 1;

const int irBinNum = 44;
const int irThresh = 50;
const int micBinNum = 20;
const int micThresh = 180;

void setup() {
  Serial.begin(9600); // use the serial port
  right_servo.attach(RW);
  left_servo.attach(LW);

  //line sensor
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode(front_wall, INPUT);

  //FFT
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  pinMode(LED_BUILTIN, OUTPUT);

  // pinMode(7, OUTPUT);

  // pinMode(FFT_DATA, INPUT);
  // pinMode(FFT_CTRL, OUTPUT);

  // digitalWrite(FFT_CTRL, LOW);

  servos_stop();
  delay(1000);
  Serial.println("start");


}

void servos_stop(){
  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);
}

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

int polling_ir(){
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    while(!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc (32 prescalar)
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i+1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();

  Serial.println(fft_log_out[irBinNum]);

  if (fft_log_out[irBinNum] > irThresh) {
    Serial.println("ir");
    //digitalWrite(7, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    return 1;
  } else {
    Serial.println("no ir");
    //digitalWrite(7, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    return 0;
  }
}


bool wallDetected(){
  int distance = analogRead(front_wall);
  Serial.println(distance);
  return (distance > 400);
}

void loop() {
  if (wallDetected){
    servos_stop();
  } 
}