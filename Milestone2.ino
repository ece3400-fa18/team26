//Iteration 0: Depth sensor
//Iteration 1: Follow Right hand rule through maze
//Iteration 2: avoid other robots (do not turn if robot OR wall detected)
//Iteration 3: Blink different colored LEDS when 1) wall encountered 2) Robot encountered 3)Keep on during duration of maze (turn off after maze finished
wall_pin = A0
THRESHOLD;

void turn(){
  servoL.write(90);
  servoR.write(90);
}

bool detectWall() {
    if (analogRead(wall_pin)>THRESHOLD) {
        return true;
    }
    else {
        return false;
    }
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
    // put your main code here, to run repeatedly:
    forward();
    while (detectTurn()==false){
        forward();
    }
    if (detectWall() == true){
        go_past_turn();
        turn();
        if (Wall() == true){
            go_past_turn();
            turn();
        }
    }
    else {
      moveForward();
    }
}
                              
