
// location and direction values
int x = 0; //initial x location
int y = 0; //initial y location
int facing = 3; //initially facing NORTH
int direction = 0; //initially going straight
int d = 0;
int updates[3] = {x,y,d};

void updateLocation(int *x, int *y, int facing){
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
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
