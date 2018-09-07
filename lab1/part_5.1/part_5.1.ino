#include <Servo.h>

Servo servo_1;

int pos = 0;
 
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  servo_1.attach(3);
  //servo_1.write(90);  // set servo to mid-point: 
}

// the loop function runs over and over again forever
void loop() {
 //servo_1.attached();
  for (pos= 0; pos <= 180; pos+=1){
     servo_1.write(pos);
     Serial.println(pos);
     delay (100);
  }
  for ( pos = 180; pos >= 0; pos-=1){
      servo_1.write(pos);
       Serial.println(pos);
      delay (100);
   }
}
