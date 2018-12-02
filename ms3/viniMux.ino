const byte sensor = A2;  // where the multiplexer in/out port is connected
int val = 0; 
int i = 0; 

// the multiplexer address select lines (A/B/C)
const byte addressA = 2; // low-order bit S0
const byte addressB = 3; // S1
const byte addressC = 4; // high-order bit S2

int readSensor (const byte which)
  {
  // select correct MUX channel
  digitalWrite (addressA, LOW);  // low-order bit
  digitalWrite (addressB, LOW);
  digitalWrite (addressC, HIGH);  // high-order bit
  //digitalWrite (addressA, (which & 1) ? HIGH : LOW);  // low-order bit
  //digitalWrite (addressB, (which & 2) ? HIGH : LOW);
  //digitalWrite (addressC, (which & 4) ? HIGH : LOW);  // high-order bit
  // now read the sensor
  return analogRead (sensor);
  }  // end of readSensor

void setup ()
  {
  Serial.begin (9600);
  Serial.println ("Starting multiplexer test ...");
  pinMode (addressA, OUTPUT); 
  pinMode (addressB, OUTPUT); 
  pinMode (addressC, OUTPUT); 
  }  // end of setup
  
void loop ()
  {
  Serial.print ("Sensor ");
  //Serial.print (i);
  Serial.print (" reads: ");
  //val = analogRead(sensor);     // read the input pin
  //Serial.println(val);
  Serial.println (readSensor (i));
  delay (1000);
  }  // end of loop
