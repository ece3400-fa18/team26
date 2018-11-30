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
array path[81] = {};
int curpath = 0; //pointer to path, gives indice that corresponds to currentlocation in path
int lastBranch = 0;//pointer to path, gives indice that corresponds to latest frontier
int numTraversed = 0;
array frontier[81] = {};
int curfront = 0;//pointer to frontier

/***********************************************************************
 *        Subroutines/Helper Functions
 ***********************************************************************/
bool frontdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of front wall
}
bool leftdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of walls on left side
}
bool rightdetectWall(){ //if no wall in front of robot, returns true 
  //sends radio of walls on right side
}

int nextPos(int curpos, dir){
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

int sidePosL(int curpos, dir){
  if (dir == 1){
    newpos = curpos -1;
  }
  else if(dir == 2){
    newpos = curpos +1;
  }
  else if(dir == 3){
    newpos = curpos +9;
  }
  else if(dir == 4){
    newpos = curpos -9;
  }
  return pos;
 
}

int sidenextPosR(int curpos, dir){
  if (dir == 1){
    newpos = curpos +1;
  }
  else if(dir == 2){
    newpos = curpos -1;
  }
  else if(dir == 3){
    newpos = curpos -9;
  }
  else if(dir == 4){
    newpos = curpos +9;
  }
  return pos;
}

bool isNew(int pos, bool IsPath){ //if array is path, put 1, if frontier put 0, if new func returns true 
  if(IsPath == 1){
    if(numTraversed == 0){
      tf = 1;
    }
    else {
      for (i=0; i<=curpath; i++) {
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
      for (i=0; i<=curfront; i++) {
        if(frontier[i] == pos){
          tf = 0;
          }
        }
      }
    }
    return tf;
}

void newDir(int pos, dir){ //if all directions lead to old or blocked by wall returns -1
  F = frontdetectWall();
  R = rightdetectWall();
  L = leftdetectWall();
  n = nextPos(pos, 1);
  s = nextPos(pos, 2);
  e = nextPos(pos, 3);
  w = nextPos(pos, 4);
  flag = -1;//if all possibilities are either blocked by walls or already traversed
  if (dir == 1){
    if (R && isNew(e, 1)){
      turn RIGHT
      dir = 3;
      flag = 0;
    }
    else if(L && isNew(w,1)){
      turn LEFT;
      dir = 4;
      flag = 0;
    }
  }
  else if(dir == 2){
    if (R && isNew(w, 1)){
      turn RIGHT
      dir = 4;
      flag = 0;
    }
    else if(L && isNew(e,1)){
      turn LEFT;
      dir = 3;
      flag = 0;
    }
  }
  else if(dir == 3){
    if (R && isNew(s, 1)){
      turn RIGHT
      dir = 2;
      flag = 0;
    }
    else if(L && isNew(n,1){
      turn LEFT;
      dir = 1;
      flag = 0;
    }  
  }
  else if(dir == 4){
    if (R && isNew(n, 1)){
      TURN RIGHT
      dir = 1;
      flag = 0;
    }
    else if(L && isNew(s,1){
      TURN LEFT;
      dir = 2;
      flag = 0;
    }
  }
}

int isPossible(int pos, int NSEW){
  ispos = 0; //default is not possible
  F = frontdetectWall();
  R = rightdetectWall();
  L = leftdetectWall();
  n = nextPos(pos, 1);
  s = nextPos(pos, 2);
  e = nextPos(pos, 3);
  w = nextPos(pos, 4);
  
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
  n = 0;
  s = 0; 
  e = 0; 
  w = 0; 
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
  tot = [n,s,e,w];
  for (i = 0; i < = 3;i++){
    if (tot[i] ~= 0){
      for (k=0; k<=curpath; k++) {
        if(path[k] == tot[i]){
          tot[i] = k;//k is index when that value was added
        }
      }
    }
    elseif(tot[i] == 0){
      tot[i] = 100;
    }
  }

  //get minimum value
  mini = 100;
  for (i = 0; i < = 3;i++){
    if(tot[i] < mini){
      mini = tot[i];
    }
  }

  if(tot[1] = mini){
    newpos = pos + 9;
  }
  else if(tot[2] = mini){
    newpos = pos - 9;
  }
  else if(tot[3] = mini){
    newpos = pos + 1;
  }
  else if(tot[4] = mini){
    newpos = pos - 1;
  }
  return newpos; 
}

void getDirnMove(int posa, int posb, dir){//gets direction between posa and posb, and turns accordingly
  if(posb == posa + 9){//N
    if(dir == 1){
      //no need
    }
    else if(dir == 2){  
      TURN RIGHT;
      TURN RIGHT;
      dir = 1; 
    }
    else if(dir == 3){  
      TURN LEFT;
      dir = 1;  
    }
    else if(dir == 4){ 
      TURN RIGHT;
      dir = 1;  
    }    
  }
  else if(posb == posa - 9){//S
    if(dir == 1){
      TURN RIGHT;
      TURN RIGHT;
      dir = 2;
    }
    else if(dir == 2){  
       //no need 
    }
    else if(dir == 3){ 
      TURN RIGHT;  
      dir = 2;
    }
    else if(dir == 4){ 
      TURN LEFT;
      dir = 2; 
    } 
  }
  else if(posb == posa + 1){//E
    if(dir == 1){
      TURN RIGHT;
      dir = 3;
    }
    else if(dir == 2){ 
      TURN LEFT;
      dir = 3;  
    }
    else if(dir == 3){ 
       //no need  
    }
    else if(dir == 4){ 
      TURN RIGHT;
      TURN RIGHT;
      dir = 3; 
    } 
  }
  else if(posb == posa - 1){//W
    if(dir == 1){
      TURN LEFT;
      dir = 4;
    }
    else if(dir == 2){
      TURN RIGHT;
      dir = 4;   
    }
    else if(dir == 3){
      TURN RIGHT;
      TURN RIGHT;
      dir = 4;   
    }
    else if(dir == 4){ 
       //no need 
    } 
  }
}

void moveDSF(){
  curpos = path[curpath];//where you currently
  branch = path[lastBranch];//the point you will traverse to
  newpos = frontier[curfront];//the new point
  //out avaible options find best for first move, move to it and set curpath pointer to it
  nextloc = findOldest(curpos);
  getDirnMove(curpos, nextloc);
  checkIntersection();
  while(~interflag){
    move(forward);
  }
  for (k=0; k<=curpath; k++) {
      if(path[k] == nextloc){
        curpath = k;//k is index of current point
      }
  }
  //from that point, traverse backwards until branch is reached
  for (k=curpath; k>=branch+1; k--) {
      getDirnMove(path[k], path[k-1]);
      checkIntersection();
      while(~interflag){
        move(forward);
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

}

void loop() {
  //Start from (1,1). Search Order: N, E, S, W 
  curpos = path[curpath];//robot starts from 0  
  newpos = nextPos(curpos);
  a = frontdetectWall();
  b = isNew(newpos, 1);
  //if no wall & next location new, advance
  if(a&&b){
    flag = 0;//robot can move forward 
    if(curpos ~= 0){//as long as this is before the robot has reached the first intersection 
      //check if walls on sides, if not, add to frontier
      posL = sidePosL(curpos);
      la = leftdetectWall();
      lb = isNew(posL,0);//check not already in frontier
      lc = isNew(posL,1);//check not already traversed
      if(la && lb && lc){
        if(frontier[curfront]){
          frontier[curfront] = posL;
        }
        else{
          curfront = curfront +1;
          frontier[curfront] = posL;
        }
      }
      posR = sidePosR(curpos);
      ra = rightdetectWall();
      rb = isNew(posR,0);//check not already in frontier
      rc = isNew(posR,1);//check not already traversed
      if(ra && rb && rc){
        if(frontier[curfront] == 0){
          frontier[curfront] = posR;
        }
        else{
          curfront = curfront +1;
          frontier[curfront] = posR;
        }
      }
      if((la && lb && lc)||(ra && rb && rc)){
        lastBranch = lastBranch +1;//if anything is added to frontier, increment
      }
    }
    //go forward in current direction 
    move(forward);
    //when intersection found, add new position to path => should be in line following function
    checkIntersection();
    if(interflag){
      if(path[curpath] == 0){
       path[curpath] = newpos; 
       numTraversed = numTraversed +1;
      }
      else {
        curpath = curpath +1;
        path[curpath] = newpos;
        numTraversed = numTraversed +1;
      }
    }
  }
  //if wall or next location old, check left and right walls and switch direction
  else if (~(a&b)){
    flag = 1;
  }
  //switch direction
  if(flag ~= 0){
    newDir(curpos); // turning taken care of inside function 
    if(flag == -1){
      //if all directions lead to old or blocked by wall
      moveDSF();
    }
  }
}
