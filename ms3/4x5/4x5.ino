void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // Run through a 4x5 maze
  Serial.println("reset");
  delay(1000);
  Serial.println("0,0,west=false");
  delay(1000);
  Serial.println("1,1,west=true");
  delay(1000);
  Serial.println("0,0,west=true,south=true");
  delay(1000);
  Serial.println("1,0,north=true,west=true");
  delay(1000);
  Serial.println("1,1,west=false");
  delay(1000);
  Serial.println("1,2,west=true,north=true");
  delay(1000);
  Serial.println("1,3,east=true,west=true");
  delay(1000);
  Serial.println("0,3,east=true,south=true");
  delay(1000);
  Serial.println("0,2,south=true,west=true,north=true");
  delay(1000);
  Serial.println("0,3,east=true,south=true");  
  delay(1000);
  Serial.println("1,3,east=true,east=true");
  delay(1000);
  Serial.println("2,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(1000);
  Serial.println("1,3,north=true,east=true");
  delay(5000);

}
