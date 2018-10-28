/* integrate this with start
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


int detectingAudio = 1;

const int irBinNum = 44;
const int irThresh = 50;
const int micBinNum = 17;
const int micThresh = 130;


void servos_stop(){
  Serial.println("stop");
  analogWrite(LW, SERVO_BRAKE);
  analogWrite(RW, SERVO_BRAKE);
  delay(3000);
}

void read_turn(){
  right_turn_val = analogRead(right_turn); //signal from outer right sensor
  left_turn_val = analogRead(left_turn); //signal from outer left sensor 
}

void move(int direction){
  // Turn if requested
    if(direction == right){
      analogWrite(LW, 95);
      analogWrite(RW, 95);
      while(analogRead(left_turn) > WHITE);
      while(analogRead(left_turn) < WHITE);
      Serial.println("done turn right");
    }
    
    if(direction == left){
      analogWrite(LW, 85);
      analogWrite(RW, 85);
      while(analogRead(right_turn) > WHITE);
      while(analogRead(right_turn) < WHITE);
      Serial.println("done turn left");
    }
}

void find_intersection(){
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
      break; //both turn sensors are on white line
    }
  }//close while; found intersection
}

void go_straight(){
  Serial.println("go");
  //modified from solution code
  //following straight line
  read_turn();
  error = left_turn_val - right_turn_val; // Positive position to right of line

  // Correct robot's driving direction according to position error
  if (abs(error) <= ERROR_RANGE){
    analogWrite(LW, SERVO_L_FORWARD_MAX);
    analogWrite(RW, SERVO_R_FORWARD_MAX);
  }
  // Robot is too right
  else if (error > ERROR_RANGE) {
    // Adjust left
    error_magnitude = abs(error)/(float)ERROR_RANGE;
    analogWrite(LW, SERVO_L_FORWARD_MAX - 2);//error_magnitude*SERVO_L_INCR_FORWARD*0.2);
    analogWrite(RW, SERVO_R_FORWARD_MAX - 2);//error_magnitude*SERVO_R_INCR_FORWARD*0.2);
  }
  // Robot is too left of line
  else if (error < -(ERROR_RANGE)) {
    // Adjust right
    error_magnitude = abs(error)/(float)ERROR_RANGE;
    analogWrite(LW, SERVO_L_FORWARD_MAX +2);
    analogWrite(RW, SERVO_R_FORWARD_MAX + 2);
  }

  delay(10);
}


bool wallDetected(){
  int distance = analogRead(front_wall);
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
  while(detectingAudio == 1){
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
   if (fft_log_out[micBinNum] > micThresh) 
      detectingAudio = 0;
  }
  TIMSK0 = prevTIMSK0;
  ADCSRA = prevADCSRA;
  ADMUX = prevADMUX;
  DIDR0 = prevDIDR0;
}

void setup() {
  Serial.begin(9600); // use the serial port
  detectAudio();
  
  pinMode(LW, OUTPUT);
  pinMode(RW, OUTPUT);

  //line sensor
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode(front_wall, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //start sequence
  Serial.println("start");
}

void loop() {
  go_straight();
  if (wallDetected()){
    Serial.println("wall detect");
    move(right);
  }
  
}
