int pinPot = A0;    
int val = 0; 

// the setup function runs once when you press reset or power the board
void setup() {
 Serial.begin(9600);      // open the serial port at 9600 bps: 
}

// the loop function runs over and over again forever
void loop() {
   val = analogRead(pinPot);   // read the input pin
   Serial.println(val);
   delay (500);
}
