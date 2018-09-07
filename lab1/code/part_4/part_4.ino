int pinPot = A0;    
int val = 0; 
int ledpin = 3;
 

// the setup function runs once when you press reset or power the board
void setup() {
 pinMode(ledpin, OUTPUT);   // sets the pin as output 
 Serial.begin(9600);      // open the serial port at 9600 bps: 
}

// the loop function runs over and over again forever
void loop() {
   val = analogRead(pinPot);   // read the input pin
   Serial.println(val);
   analogWrite(ledpin, val / 4);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
   delay (100);
}
