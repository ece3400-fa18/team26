//this set register correctly but fft only polls once in awhile
/*
fft_adc_serial.pde
guest openmusiclabs.com 7.7.14
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.
*/

#include <Servo.h>

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

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
int detectingAudio = 1;

const int irBinNum = 67;
const int irThresh = 40;
const int micBinNum = 20;
const int micThresh = 180;

void setup() {
  Serial.begin(115200); // use the serial port
  right_servo.attach(RW);
  left_servo.attach(LW);

  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode(front_wall, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //FFT
  TIMSK0 = 0; // turn off timer0 for lower jitter
  //ADCSRA = 0xe5; // set the adc to free running mode
  //ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  //start
  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);
  delay(1000);
  Serial.println("start");
}

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
  return (distance > 305);
}

void fftDetected(){
  cli();  // UDRE interrupt slows this way down on arduino1.0
    analogWrite(11, 100);
    digitalWrite(LED_BUILTIN, HIGH);
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      int k = analogRead(A0);
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    digitalWrite(LED_BUILTIN, LOW);
    sei();

    delay(500);
}


void loop() {
//  go_straight();
//  if (wallDetected()){
//    //find_intersection();
//    move(right);
//  }

  //fftDetected();
  int temp = analogRead(A0);
  Serial.println(temp);


}
