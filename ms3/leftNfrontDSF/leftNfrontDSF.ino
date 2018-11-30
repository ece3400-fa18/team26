#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <Servo.h>
#include <FFT.h>

Servo left_servo;
Servo right_servo;

/***********************************************************************
 *        Variable Declarations 
 ***********************************************************************/

// Pin Assignments 
const int LW = 5; // Servo1
const int RW = 6; // Servo2
int right_turn = A5;
int left_turn  = A4;
int front_wall = A2;
int left_wall =  A3;

#define SERVO_BRAKE          90
#define SERVO_L_FORWARD_MAX  95.0
#define SERVO_R_FORWARD_MAX  85.0
#define SERVO_L_INCR_FORWARD 2.0
#define SERVO_R_INCR_FORWARD -2.0

// Thresholds for each sensor to determine when over a line
#define WHITE       700
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

//// Location and Direction values
//volatile int unsigned long x = 0; //initial x 
//volatile int unsigned long y = -1; //initial y 
//int facing = 3; //initially facing NORTH
//int direction = 0; //initially going straight
//volatile int unsigned long d = 0;
//int unsigned long updates[3] = {x,y,d};

//Intersection Flag
int interflag = 0;
int updateFlag = 1;

//DSF values
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
/*path is a 1x81 array of zeros. When a new location is traversed, the corresponding number to that 
 * location replaces the zero. 
 * Example: 5 points have been traversed and no wall encountered, path[81] = {1,10, 19, 28, 37, 0, 0...0}
 * curpath = 4 
 * curpath is the pointer to path (gives the indice that corresponds to the current position, curpos = path[curpath]
 * This was done because dynamic memory allocation is difficult and computationally expensive in Arduino
 * To get the coordinate position for any location, use the following formula:
 * WHERE 1 = (0,0)|| WHERE 1 = (1,1)
 * X = rem(#,9)-1 ||    X = rem(#,9)              
 * Y = floor(#/9) ||    Y = floor(#/9) + 1
 * frontier path is also an array (this was to avoid adding the same value to the frontier multiple times, as stacks can not be searched)
 * curfront is the pointer to the frontier
 * In the above example (assuming that 2, 11, 20, 29, 38 do not contain any walls), the frontier and curfront would be:
 * frontier = {2, 11, 20, 29, 38, 0, 0...0} and curfront = 4
 * numTraversed is the total number of new points traversed by the robot at any given moment, at this point it would be 5
 * 
 * FINDING NEW POSITION
 * for any current position A, to find new position B
 * if (dir == N)
 *     B = A + 9; 
 * else if (dir == S)
 *     B = A - 9;
 * else if (dir == E)
 *     B = A + 1;
 * else if (dir == W)
 *     B = A - 1;
*/
int path[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int curpath = 0; //pointer to path, gives indice that corresponds to currentlocation in path
int lastBranch = 0;//pointer to path, gives indice that corresponds to latest frontier
int numTraversed = 0;
int frontier[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int curfront = 0;//pointer to frontier

/***********************************************************************
 *        Subroutines/Helper Functions
***********************************************************************/
void servos_stop(){
//  Serial.println("stop");
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
      left_servo.write(95);
      right_servo.write(95);
      delay(50);
      while(analogRead(left_turn) > WHITE);
//      while(analogRead(left_turn) < WHITE);
    }
    
    if(direction == left){
      left_servo.write(85);
      right_servo.write(85);
      delay(50);
      while(analogRead(right_turn) > WHITE);
//      while(analogRead(right_turn) < WHITE);
    }
}

void checkIntersection(){
    if (left_turn_val < WHITE && right_turn_val < WHITE){
      interflag = 1;
    }
    else {
      interflag = 0;
      updateFlag = 1;
    }
}


void go_straight(){
  checkIntersection();
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
    //Serial.println(fft_log_out[micBinNum]);
   if (fft_log_out[micBinNum] > micThresh){
    Serial.println("SOUND");
    Serial.println(fft_log_out[micBinNum]);
    break;
   } 
  }
  TIMSK0 = prevTIMSK0;
  ADCSRA = prevADCSRA;
  ADMUX = prevADMUX;
  DIDR0 = prevDIDR0;
}

bool frontdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of front wall
  int distance = analogRead(front_wall);
  if (distance > 305){
    return 0;
  }
  else{
    return 1;
  }
}
bool leftdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of walls on left side
  if (analogRead(left_wall) > 575) {
      return 0;
    }
    else{
      return 1;
    }
}


int nextPos(int curpos){
  int newpos = 0;
  if (curpos == 0){
    newpos = 1;
  }
  else if (dir == 1){
    newpos = curpos +9;
  }
  else if(dir == 2){
    newpos = curpos -9;
  }
  else if(dir == 3){
    newpos = curpos +1;
  }
  else if(dir == 4){
    newpos = curpos -1;
  }
  return newpos; 
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
        if(path[i] == pos){
          tf = 0;
        }
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

void newDir(int pos){ //if all directions lead to old or blocked by wall returns -1
  bool F = frontdetectWall();
//  bool R = rightdetectWall();
  bool L = leftdetectWall();
  int n = pos + 9;
  int s = pos - 9;
  int e = pos + 1;
  int w = pos - 1;
  flag = -1;//if all possibilities are either blocked by walls or already traversed
  if (dir == 1){
    if(L && isNew(w,1)){
      turn(left);
      dir = 4;
      flag = 0;
    }
  }
  else if(dir == 2){
    if(L && isNew(e,1)){
      turn(left);
      dir = 3;
      flag = 0;
    }
  }
  else if(dir == 3){
    if(L && isNew(n,1)){
      turn(left);
      dir = 1;
      flag = 0;
    }  
  }
  else if(dir == 4){
    if(L && isNew(s,1)){
      turn(left);
      dir = 2;
      flag = 0;
    }
  }
}

int isPossible(int pos, int NSEW){
  bool ispos = 0; //default is not possible
  bool F = frontdetectWall();
//  bool R = rightdetectWall();
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
//      if(R){
        ispos = 1;
//      }
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
//      if(R){
        ispos = 1;
//      }
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
//      if(R){
        ispos = 1;
//      }
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
//      if(R){
        ispos = 1;
//      }
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
  if (isPossible(pos, 1)){
    n = pos + 9;
  }
  if (isPossible(pos, 2)){
    s = pos - 9;
  }
  if (isPossible(pos, 3)){
    e = pos +1;
  }
  if (isPossible(pos, 4)){
    w = pos - 1;
  }
  int tot[4] = {n,s,e,w};
  for (int i = 0; i <= 3;i++){
    if (tot[i] != 0){
      for (int k=0; k<=curpath; k++) {
        if(path[k] == tot[i]){
          tot[i] = k;//k is index when that value was added
        }
      }
    }
    else if(tot[i] == 0){
      tot[i] = 100;
    }
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

void moveDSF(){
  int curpos = path[curpath];//where you currently
  int branch = path[lastBranch];//the point you will traverse to
  int newpos = frontier[curfront];//the new point
  //out avaible options find best for first move, move to it and set curpath pointer to it
  int nextloc = findOldest(curpos);
  getDirnMove(curpos, nextloc);
  checkIntersection();
  while(!interflag){
    go_straight();
  }
  for (int k=0; k<=curpath; k++) {
      if(path[k] == nextloc){
        curpath = k;//k is index of current point
      }
  }
  //from that point, traverse backwards until branch is reached
  for (int k=curpath; k>=branch+1; k--) {
      getDirnMove(path[k], path[k-1]);
      checkIntersection();
      while(!interflag){
        go_straight();
      }
  }
  //after reaching branch reset curpath using numTraversed -1 
  getDirnMove(newpos, branch);
  curpath = numTraversed-1;
  flag = 0;
}
/***********************************************************************
 *        Intializing Global Variable
***********************************************************************/

/***********************************************************************/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // use the serial port
  detectAudio();
  right_servo.attach(RW);
  left_servo.attach(LW);
  pinMode(right_turn, INPUT);
  pinMode(left_turn, INPUT); 
  pinMode(front_wall, INPUT);
  pinMode(left_wall, INPUT);
}

void loop() {
  //Search Order: N, E, S, W 
  int curpos = path[curpath];//robot starts from 0, first intersection 1  
  int newpos = nextPos(curpos);
  bool a = frontdetectWall();
  bool b = isNew(newpos, 1);
  //if no wall & next location new, advance
  if(a&&b){
    flag = 0;//robot can move forward 
    if(curpos != 0){//as long as this is after the robot has reached the first intersection 
      //check if walls on sides, if not, add to frontier
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
      if((la && lb && lc)){
        lastBranch = lastBranch +1;//if anything is added to frontier, increment
      }
    }
    //go forward in current direction 
    go_straight();
    //when intersection found, add new position to path & update current position
    checkIntersection();
    if(interflag && updateFlag){
      if(path[curpath] == 0){
       path[curpath] = newpos; 
      }
      else {
        curpath = curpath +1;
        path[curpath] = newpos;
      }
      updateFlag = 0;
      numTraversed = numTraversed +1;
    }
  }
  //if wall or next location old, check left and right walls and switch direction
  else if (!(a&b)){
    flag = 1;
  }
  //switch direction
  if(flag != 0){
    newDir(curpos); // turning taken care of inside function 
    if(flag == -1){
      //if all directions lead to old or blocked by wall
      moveDSF();
    }
  }
}
