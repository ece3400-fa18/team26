#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <Servo.h>
//#include <FFT.h>
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
const int pushbutton = 7; //push button

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

// Line sensor values
int right_turn_val  = 0;
int left_turn_val   = 0;

#define front_wall_sensor 2 //if greater than 140
#define left_wall_sensor 4 //if greater than 190 
#define right_wall_sensor 0 // greater than 200
#define third_line_sensor 6 // threshold if less than 100 is white
#define third_line_thresh 100

const byte mux = A2;  // where the multiplexer in/out port is connected


// the multiplexer address select lines (A/B/C)
const byte addressA = 2; // low-order bit S0
const byte addressB = 3; // S1
const byte addressC = 4; // high-order bit S2

// Control variables for turning
const int irBinNum  = 44;
const int irThresh  = 50;
const int micBinNum = 17;
const int micThresh = 135;

//Intersection Flags
bool interflag = 0;
bool updateFlag = 1;
bool errorflag = 0;
bool reached = 0;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x0000000070LL, 0x0000000071LL };
typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
role_e role = role_pong_back;

//DFS values
int flag = 0; //corresponds to how many directions did not lead to a new position, used to decide direction
int dir = 1; //pointer to direction || N = 1, E = 3, S = 2, W = 4
/*
 * 73  74  75  76  77  78  79  80  81
 * 64  65  66  67  68  69  70  71  72  
 * 55  56  57  58  59  60  61  62  63
 * 46  47  48  49  50  51  52  53  54 
 * 37  38  39  40  41  42  43  44  45
 * 28  29  30  31  32  33  34  35  36
 * 19  20  21  22  23  24  25  26  27
 * 10  11  12  13  14  15  16  17  18
 * 1   2   3    4   5   6   7   8   9
*/

int path[81] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int curpath = 0; //pointer to path, gives indice that corresponds to currentlocation in path
int lastBranch = 0;//pointer to path, gives indice that corresponds to latest frontier
int numTraversed = 0;
int frontier[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int curfront = 0;//pointer to frontier
int newpos = 10; 

/***********************************************************************
 *       ROBOT BASIC FUNCTIONS FUNCTIONS
 ***********************************************************************/
int readSensor (const byte which)
  {
  // select correct MUX channel
  digitalWrite (addressA, (which & 1) ? HIGH : LOW);  // low-order bit
  digitalWrite (addressB, (which & 2) ? HIGH : LOW);
  digitalWrite (addressC, (which & 4) ? HIGH : LOW);  // high-order bit
  // now read the sensor
  return analogRead (mux);
  }  // end of readSensor
 
void servos_stop(){
  //Serial.println("stop");
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
    delay(50);
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
  checkIntersection();
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

bool frontdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of front wall
  int distance = readSensor(front_wall_sensor);
  Serial.println(distance);
  if (distance > 140){
    servos_stop();
    return 0;
  }
  else
    return 1;
}

bool leftdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of walls on left side
  int distance = readSensor(left_wall_sensor);
  if (distance > 190)
      return 0;
  else
      return 1;
}

bool rightdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of walls on right side
  int distance = readSensor(right_wall_sensor);
  if ( distance > 200) {
      //servos_stop();
      return 0;
    }
    else{
      return 1;
    }
}

void detectAudio(){
  while(1){
   if (digitalRead(pushbutton) == HIGH){
    break;
   }
  }
}

int convert(char *s) {
  int result = 0;
  while(*s) {
    result <<= 1;
    if(*s++ == '1') result |= 1;
  }
  return result;
}

void radioSetup(void)
{
  //
  // Print preamble
  //
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_MIN);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

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

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void transmit(int x, int y, char *d)
{
  role = role_ping_out;
  // Ping out role
  if (role == role_ping_out)
  {
    radio.stopListening();
    
    //sending message
    unsigned char message[3] = {x,y,convert(d)};
    
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

void sendWalls(void){
  transmit(0,0,"10001001");
  for (int i=1; i<8; i++)
    transmit(i,0,"10000001");
  transmit(8,0,"10000011");
  for (int i=1; i<8; i++)
    transmit(8,i,"10000010");
  transmit(8,8,"10000110");
  for (int i=1; i<8; i++)
    transmit(i,8,"10000100");
  transmit(0,8,"10001100");
  for (int i=1; i<8; i++)
    transmit(0,i,"10001000");
}

/***********************************************************************
 *       DFS HELPER FUNCTIONS
 ***********************************************************************/

int getX(int pos){
  int Y = pos/9; 
  int X = pos - (Y*9);
  return X;
}

int getY(int pos){
  int Y = pos/9;
  return Y;
}
 
void nextPos(int curpos){
  if (curpos == 0) newpos = 10;
  else if (dir == 1)newpos = curpos +9;
  else if (dir == 2)newpos = curpos -9;
  else if (dir == 3)newpos = curpos +1;
  else if (dir == 4)newpos = curpos -1;
}

int sidePosL(int curpos){
  int pos = 0;
  if (dir == 1){
    pos = curpos -1;
  }
  else if(dir == 2){
    pos = curpos +1;
  }
  else if(dir == 3){
    pos = curpos +9;
  }
  else if(dir == 4){
    pos = curpos -9;
  }
  return pos;
 
}

int sidePosR(int curpos){
  int pos = 0;
  if (dir == 1){
    pos = curpos +1;
  }
  else if(dir == 2){
    pos = curpos -1;
  }
  else if(dir == 3){
    pos = curpos -9;
  }
  else if(dir == 4){
    pos = curpos +9;
  }
  return pos;
}

bool isNew(int pos, bool IsPath){ //if array is path, put 1, if frontier put 0, if pos is new func returns true 
  bool tf = 1;
  if(IsPath == 1){
    if(numTraversed == 0){
      tf = 1;
    }
    else {
      for (int i=0; i<=curpath; i++) {
        if(path[i] == pos) tf = 0;
      }
    }
  }
  else if(IsPath == 0){
    if(frontier[curfront] == 0){
      tf = 1;
    }
    else{
      for (int i=0; i<=curfront; i++) {
        if(frontier[i] == pos){
          tf = 0;
          }
        }
      }
    }
    return tf;
}

void updateFrontierWPath(){
  int latest = path[curpath];
  bool tf = isNew(latest, 0);
  if(~tf){
    int index = 0;
    for(int i = 0; i <= curfront; i++){
      if(frontier[i] == latest){
        int index = i;
      }
    }
    if(index == curfront){
      frontier[curfront] = 0;
      curfront = curfront -1;
    }
    else{
       for(int k = index; k <= curfront - 1; k++){
          int a = frontier[k+1];
          frontier[k] = a;
       }
       frontier[curfront] = 0;
       curfront = curfront - 1;
    }
  }
}

void newDir(int pos){ //if all directions lead to old or blocked by wall returns -1
  bool F = frontdetectWall();
  bool R = rightdetectWall();
  bool L = leftdetectWall();

  if (~F){ // check for IR
    //update radio 
    int X = getX(pos);
    int Y = getX(pos);
    transmit(X,Y,"0");
  }
  if(~R){ // check for IR
    //update radio
    int X = getX(pos);
    int Y = getX(pos);
    transmit(X,Y,"0");
  }
  if(~L){ // check for IR
    //update radio
    int X = getX(pos);
    int Y = getX(pos);
    transmit(X,Y,"0");
  }
  int n = pos + 9;
  int s = pos - 9;
  int e = pos + 1;
  int w = pos - 1;
  flag = -1;//if all possibilities are either blocked by walls or already traversed
  if (dir == 1){
    if(F && isNew(n,1)){
      dir = 1;
      flag = 0;
    }
    else if (R && isNew(e, 1)){
      turn(right);
      dir = 3;
      flag = 0;
    }
    else if(L && isNew(w,1)){
      turn(left);
      dir = 4;
      flag = 0;
    }
  }
  else if(dir == 2){
   if(F && isNew(s,1)){
      dir = 2;
      flag = 0;
    }
    else if (R && isNew(w, 1)){
      turn(right);
      dir = 4;
      flag = 0;
    }
    else if(L && isNew(e,1)){
      turn(left);
      dir = 3;
      flag = 0;
    }
  }
  else if(dir == 3){
    if(F && isNew(e,1)){
      dir = 3;
      flag = 0;
    }
    else if (R && isNew(s, 1)){
      turn(right);
      dir = 2;
      flag = 0;
    }
    else if(L && isNew(n,1)){
      turn(left);
      dir = 1;
      flag = 0;
    }  
  }
  else if(dir == 4){
    if(F && isNew(w,1)){
      dir = 4;
      flag = 0;
    }
    else if (R && isNew(n, 1)){
      turn(right);
      dir = 1;
      flag = 0;
    }
    else if(L && isNew(s,1)){
      turn(left);
      dir = 2;
      flag = 0;
    }
  }
}

int isPossible(int pos, int NSEW){
  bool ispos = 0; //default is not possible
  bool F = frontdetectWall();
  bool R = rightdetectWall();
  bool L = leftdetectWall();
  int n = pos + 9;
  int s = pos - 9;
  int e = pos + 1;
  int w = pos - 1;
  
  if (dir == 1){
    if (NSEW == 1){
      if(F){
        ispos = 1;
      }
    }
    else if(NSEW == 2){
      ispos = 1;
    }
    else if(NSEW == 3){
      if(R){
        ispos = 1;
      }
    }
    else if(NSEW == 4){
      if(L){
        ispos = 1;
      }
    }
  }
  else if(dir == 2){
    if (NSEW == 2){
      if(F){
        ispos = 1;
      }
    }
    else if(NSEW == 1){
      ispos = 1;
    }
    else if(NSEW == 4){
      if(R){
        ispos = 1;
      }
    }
    else if(NSEW == 3){
      if(L){
        ispos = 1;
      }
    }
  }
  else if(dir == 3){
    if (NSEW == 3){
      if(F){
        ispos = 1;
      }
    }
    else if(NSEW == 4){
      ispos = 1;
    }
    else if(NSEW == 2){
      if(R){
        ispos = 1;
      }
    }
    else if(NSEW == 1){
      if(L){
        ispos = 1;
      }
    }
  }
  else if(dir == 4){
    if (NSEW == 4){
      if(F){
        ispos = 1;
      }
    }
    else if(NSEW == 3){
      ispos = 1;
    }
    else if(NSEW == 1){
      if(R){
        ispos = 1;
      }
    }
    else if(NSEW == 2){
      if(L){
        ispos = 1;
      }
    }
  }
  return ispos;
}

int findOldest(int pos){//returns oldest position from avaible options
  int n = 0;
  int s = 0; 
  int e = 0; 
  int w = 0; 
  if (isPossible(pos, 1)) n = pos + 9;
  if (isPossible(pos, 2)) s = pos - 9;
  if (isPossible(pos, 3)) e = pos + 1;
  if (isPossible(pos, 4)) w = pos - 1;
  int tot[4] = {n,s,e,w};
  for (int i = 0; i <= 3;i++){
    if (tot[i] != 0){
      for (int k=0; k<=curpath; k++) {
        if(path[k] == tot[i]){
          tot[i] = k;//k is index when that value was added
        }
      }
    }
    else if(tot[i] == 0)
      tot[i] = 100;
  }

  //get minimum value
  int mini = 100;
  for (int i = 0; i <= 3;i++){
    if(tot[i] < mini){
      mini = tot[i];
    }
  }
  int oldest = pos + 9;
  if(tot[1] = mini){
    oldest = pos + 9;
  }
  else if(tot[2] = mini){
    oldest = pos - 9;
  }
  else if(tot[3] = mini){
    oldest = pos + 1;
  }
  else if(tot[4] = mini){
    oldest = pos - 1;
  }
  return oldest; 
}

void getDirnMove(int posa, int posb){//gets direction between posa and posb, and turns accordingly
  if(posb == posa + 9){//N
    if(dir == 1){
      //no need
    }
    else if(dir == 2){  
      turn(right);
      turn(right);
      dir = 1; 
    }
    else if(dir == 3){  
      turn(left);
      dir = 1;  
    }
    else if(dir == 4){ 
      turn(right);
      dir = 1;  
    }    
  }
  else if(posb == posa - 9){//S
    if(dir == 1){
      turn(right);
      turn(right);
      dir = 2;
    }
    else if(dir == 2){  
       //no need 
    }
    else if(dir == 3){ 
      turn(right);  
      dir = 2;
    }
    else if(dir == 4){ 
      turn(left);
      dir = 2; 
    } 
  }
  else if(posb == posa + 1){//E
    if(dir == 1){
      turn(right);
      dir = 3;
    }
    else if(dir == 2){ 
      turn(left);
      dir = 3;  
    }
    else if(dir == 3){ 
       //no need  
    }
    else if(dir == 4){ 
      turn(right);
      turn(right);
      dir = 3; 
    } 
  }
  else if(posb == posa - 1){//W
    if(dir == 1){
      turn(left);
      dir = 4;
    }
    else if(dir == 2){
      turn(right);
      dir = 4;   
    }
    else if(dir == 3){
      turn(right);
      turn(right);
      dir = 4;   
    }
    else if(dir == 4){ 
       //no need 
    } 
  }
}

void goUntilIntersec(){
  int stopnow = 0;
  checkIntersection();
  if(!reached){
    go_straight(); 
     if(interflag){
      stopnow = 1;
     }
  }
  if(stopnow){
    reached = 1;
    right_servo.write(SERVO_BRAKE);
    left_servo.write(SERVO_BRAKE);
    delay(50);
  }
}


void moveDFS(){
  int curpos = path[curpath];//where you currently
  int branch = path[lastBranch];//the point you will traverse to
  int newfront = frontier[curfront];//the new point
  //out avaible options find best for first move, move to it and set curpath pointer to it
  int nextloc = findOldest(curpos);
  getDirnMove(curpos, nextloc);
  goUntilIntersec();
  for (int k=0; k<=curpath; k++) {
      if(path[k] == nextloc){
        curpath = k;//k is index of current point
      }
  }
  //from that point, traverse backwards until branch is reached
  for (int i=curpath; i>=branch+1; i--) {
      getDirnMove(path[i], path[i-1]);
      if(i != branch){
        reached = 0;
      }
      goUntilIntersec();
  }
  //after reaching branch reset curpath using numTraversed -1 
  getDirnMove(newfront, branch);
  curpath = numTraversed-1;
  flag = 0;
}


/***********************************************************************
 *       DFS MAIN FUNCTIONS
 ***********************************************************************/

void updatePath(){
  int curpos = path[curpath];//robot starts from over intersection 1  
  nextPos(curpos);
  bool a = isNew(newpos, 1);
  if(curpos == 0){
    path[curpath] = newpos;
    numTraversed = numTraversed +1;
  }
  else{
    if(a){
      curpath = curpath +1;
      path[curpath] = newpos;
      numTraversed = numTraversed +1;
    }
  }
}

void updateFrontier(){
  int curpos = path[curpath];
  //if just traversed point was in frontier, update
  updateFrontierWPath();
  //add to frontier: Left
  int posL = sidePosL(curpos);
  bool la = leftdetectWall();
  bool lb = isNew(posL,0);//check not already in frontier
  bool lc = isNew(posL,1);//check not already traversed
  if(la && lb && lc){
    if(frontier[curfront]){
      frontier[curfront] = posL;
    }
    else{
      curfront = curfront +1;
      frontier[curfront] = posL;
    }
  }
  //add to frontier: Right
  int posR = sidePosR(curpos);
  bool ra = rightdetectWall();
  bool rb = isNew(posR,0);//check not already in frontier
  bool rc = isNew(posR,1);//check not already traversed
  if(ra && rb && rc){
    if(frontier[curfront] == 0){
      frontier[curfront] = posR;
    }
    else{
      curfront = curfront +1;
      frontier[curfront] = posR;
    }
  }
  //update lastBranch
  if((la && lb && lc)||(ra && rb && rc)){
    lastBranch = lastBranch +1;//if anything is added to frontier, increment
  } 
}

void decideNextDir(){
  int curpos = path[curpath];
  newDir(curpos);
  if(flag == -1){
    moveDFS();
  }
}


/***********************************************************************
 *       MAIN SETUP AND LOOP
 ***********************************************************************/
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // use the serial port
  detectAudio();
  radioSetup();
  right_servo.attach(RW);
  left_servo.attach(LW);
  pinMode(pushbutton, INPUT);
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode (addressA, OUTPUT); 
  pinMode (addressB, OUTPUT); 
  pinMode (addressC, OUTPUT); 
  sendWalls();
}

void loop() {
   go_straight();
   checkIntersection();
   if(interflag && updateFlag && ~errorflag){
      updatePath();
      updateFrontier();
      decideNextDir();
      updateFlag = 0;
    }
}
