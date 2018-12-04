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
#define third_line_sensor 6 
#define third_line_thresh 100

#include <Servo.h>
#include <FFT.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

Servo left_servo;
Servo right_servo;
RF24 radio(9,10);

/***********************************************************************
 *        Variable Declarations 
 ***********************************************************************/

// Pin Assignments 
const int LW = 5; // Servo1
const int RW = 6; // Servo2
const int right_turn  =  A5;
const int left_turn   =  A4;
const int front_wall  =  A2;
const int left_wall   =  A3;

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  95.0
#define SERVO_R_FORWARD_MAX  85.0
#define SERVO_L_INCR_FORWARD 2.0
#define SERVO_R_INCR_FORWARD -2.0

// Thresholds for each sensor to determine when over a line
#define WHITE       600
#define ERROR_RANGE 100
#define left        0
#define forward     1
#define right       2

// Control Variables for line following
float error           = 0;
float error_magnitude = 0;
int   i = 0;

// Line sensor values
int right_turn_val  = 0;
int left_turn_val   = 0;

// Control variables for turning
int turn_count = 0;
const int irBinNum  = 44;
const int irThresh  = 50;
const int micBinNum = 17;
const int micThresh = 135;

//Intersection Flag
bool interflag = 0;
bool updateFlag = 1;
bool errorflag = 0;
int intersec = 0;

const byte mux = A2;
// the multiplexer address select lines (A/B/C)
const byte addressA = 2; // low-order bit S0
const byte addressB = 3; // S1
const byte addressC = 4; // high-order bit S2


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x0000000070LL, 0x0000000071LL };
typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
role_e role = role_pong_back;


/***********************************************************************
 *        Subroutines/Helper Functions
 ***********************************************************************/

int readSensor (const byte which){
  // select correct MUX channel
  digitalWrite (addressA, (which & 1) ? HIGH : LOW);  // low-order bit
  digitalWrite (addressB, (which & 2) ? HIGH : LOW);
  digitalWrite (addressC, (which & 4) ? HIGH : LOW);  // high-order bit
  return analogRead (mux);
} 
 
void servos_stop(){
  Serial.println("stop");
  right_servo.write(SERVO_BRAKE);
  left_servo.write(SERVO_BRAKE);
  delay(3000);
}

void read_turn(){
  right_turn_val = analogRead(right_turn); //signal from outer right sensor
  left_turn_val = analogRead(left_turn);  //signal from outer left sensor 
}

void turn(int direction){
  // Turn if requested
    if(direction == right){
      delay(12);
      left_servo.write(95);
      right_servo.write(95);
      //while(analogRead(left_turn) > WHITE);//UNECESSARY
      while(analogRead(left_turn) < WHITE);
    }
    
    if(direction == left){
      delay(12);
      left_servo.write(85);
      right_servo.write(85);
      //while(analogRead(right_turn) > WHITE);//UNECESSARY
      while(analogRead(right_turn) < WHITE);
    }

}

void checkIntersection(){
    int third = readSensor(third_line_sensor);
    read_turn();
    if (left_turn_val < WHITE && right_turn_val < WHITE && third < third_line_thresh){
      interflag = 1;
    }
    else {
      interflag = 0;
      updateFlag = 1;
    }
    delay(10);
}



void go_straight(){
  read_turn();
  error = left_turn_val - right_turn_val; // Positive position to right of line

  // Correct robot's driving direction according to position error
  if (abs(error) <= ERROR_RANGE){
    left_servo.write(SERVO_L_FORWARD_MAX);
    right_servo.write(SERVO_R_FORWARD_MAX);
    errorflag = 0; 
  }
  // Robot is too right
  else if (error > ERROR_RANGE) {
    errorflag = 1;
    // Adjust left
    error_magnitude = abs(error)/(float)ERROR_RANGE;
    left_servo.write(SERVO_L_FORWARD_MAX + error_magnitude*SERVO_L_INCR_FORWARD);
    right_servo.write(SERVO_R_FORWARD_MAX - error_magnitude*SERVO_R_INCR_FORWARD);
  }
  // Robot is too left of line
  else if (error < -(ERROR_RANGE)) {
    // Adjust right
    errorflag = 1;
    error_magnitude = abs(error)/(float)ERROR_RANGE;
    left_servo.write(SERVO_L_FORWARD_MAX - error_magnitude*SERVO_L_INCR_FORWARD);
    right_servo.write(SERVO_R_FORWARD_MAX + error_magnitude*SERVO_R_INCR_FORWARD);
  }
  delay(10);
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
   if (fft_log_out[micBinNum] > micThresh){
    Serial.println("SOUND");
    break;
   } 
  }
  TIMSK0 = prevTIMSK0;
  ADCSRA = prevADCSRA;
  ADMUX = prevADMUX;
  DIDR0 = prevDIDR0;
}

int convert(char *s) {
  int result = 0;
  while(*s) {
    result <<= 1;
    if(*s++ == '1') result |= 1;
  }
  return result;
}


void radioSetup(void){
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);

  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }
  radio.startListening();
  radio.printDetails();
}


void transmit(void){
  role = role_ping_out;
    
  // Ping out role
  if (role == role_ping_out)
  {
    radio.stopListening();
    
    //sending message
    char d[129];
    itoa(10010,d,2);
    unsigned char message[3] = {0,0,convert(d)};
    unsigned char tx = message[0];
    unsigned char ty = message[1];
    unsigned char td = message[2];
    Serial.println(tx);
    Serial.println(ty);
    Serial.println(td);
    bool ok = radio.write(&message, sizeof(message));

    if (ok) printf("ok...");
    else printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
      printf("Failed, response timed out.\n\r");
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_message[3];
      radio.read( &got_message, sizeof(got_message));
      printf("Got response %lu, round-trip delay: %lu\n\r",got_message,millis());
    }
    delay(1000);
  }

  if ( Serial.available() )
  {
    char c = 'T';
    role = role_pong_back;
    
    if ( c == 'T' && role == role_pong_back )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");
      role = role_ping_out;
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    }
  }
}



/***********************************************************************
 *        Main Set Up and Loop
 ***********************************************************************/
void setup() {
  Serial.begin(9600);
  detectAudio();
  radioSetup();
  right_servo.attach(RW);
  left_servo.attach(LW);
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode(front_wall, INPUT);
  pinMode(left_wall, INPUT);
  pinMode (addressA, OUTPUT); 
  pinMode (addressB, OUTPUT); 
  pinMode (addressC, OUTPUT); 
}


void loop() {
    go_straight();
    checkIntersection();
    if(interflag && updateFlag && ~errorflag){
      intersec = intersec + 1; 
      updateFlag = 0;
      if(intersec == 1||intersec == 6 || intersec == 7 || intersec == 8){
        turn(right);
      }
      else if (intersec == 2||intersec == 3 || intersec == 4 || intersec == 5) {
        turn(left);
      }
    }
}
