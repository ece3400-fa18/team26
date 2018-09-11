#include<Servo.h>

Servo servo_L;
Servo servo_R;

int pos = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo_L.attach(9);
  servo_R.attach(10);
}

void loop() {
  // put your main code here, to run repeatedly:
//  servo_L.write(180);
//  servo_R.write(0);
//  delay(3000);
//  servo_L.write(180);
//  servo_R.write(90);
//  delay(3000);

  //wanting to test individual servo...
  servo_R.write(0);
  delay(3000);
  servo_R.write(45);
  delay(3000);
}
