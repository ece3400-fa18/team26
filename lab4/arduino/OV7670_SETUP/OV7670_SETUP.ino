#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21
//read: 
#define CC7 0x12  //Common Control 7
#define CC3 0x0C  //Common Control 3
#define IC   0x11   //Internal Clock
#define CC15 0x40 //Common Control 15
#define CC14 0x3E //Common Control 14 * 
#define CC17 0x42 //Common Control 17
#define MVFP 0x14 //MVFP 1E


///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  // TODO: READ KEY REGISTERS
  //OV7670_write_register(CC7, 0x80);//setting to 1 resets all registers
  read_key_registers();

  delay(100);
  
  // TODO: WRITE KEY REGISTERS
  write_key_registers();

  set_color_matrix();
  read_key_registers();

  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
}

int MSB = 0;
int LSB = 0;
int B = 0;

void loop(){
  MSB = digitalRead(8);
  B = digitalRead(9);
  LSB = digitalRead(10);

  if(MSB == LOW && B == LOW && LSB == LOW){ //nothing
    Serial.println("no color");
  }
  else if(MSB == LOW && B == LOW && LSB == HIGH){
    Serial.println("BLUE TRIANGLE");
  }
  else if(MSB == LOW && B == HIGH && LSB == LOW){
    Serial.println("BLUE DIAMOND");
  }
  else if(MSB == LOW && B == HIGH && LSB == HIGH){
    Serial.println("BLUE SQUARE");
  }
  else if(MSB == HIGH && B == LOW && LSB == LOW){
    Serial.println("RED TRIANGLE");
  }
  else if(MSB == HIGH && B == LOW && LSB == HIGH){
    Serial.println("RED DIAMOND");
  }
  else if(MSB == HIGH && B == HIGH && LSB == LOW){
    Serial.println("RED SQUARE");
  }
  Serial.println( "loop" );
}


///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
//  read_register_value(CC7); //Common Control 7
  Serial.println("Common Control 7");
  Serial.println(read_register_value(CC7),HEX);
//  read_register_value(CC3); //Common Control 3
  Serial.println("Common Control 3");
  Serial.println(read_register_value(CC3),HEX);
//  read_register_value(IC);//Internal Clock
  Serial.println("Internal Clock");
  Serial.println(read_register_value(IC),HEX);
//  read_register_value(CC15);//Common Control 15
  Serial.println("Common Control 15");
  Serial.println(read_register_value(CC15),HEX);
//  read_register_value(CC17);//Common Control 17
  Serial.println("Common Control 17");
  Serial.println(read_register_value(CC17),HEX);
//  read_register_value(MVFP);//MVFP
  Serial.println("MVFP");
  Serial.println(read_register_value(MVFP),HEX);
}

byte read_register_value(int register_address){
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
  //Serial.println(Wire.available());
  while(Wire.available()<1);
  data = Wire.read();
  return data;
}

String OV7670_write(int start, const byte *pData, int size){
    int n,error;
    Wire.beginTransmission(OV7670_I2C_ADDRESS);
    n = Wire.write(start);
    if(n != 1){
      return "I2C ERROR WRITING START ADDRESS";   
    }
    n = Wire.write(pData, size);
    if(n != size){
      return "I2C ERROR WRITING DATA";
    }
    error = Wire.endTransmission(true);
    if(error != 0){
      return String(error);
    }
    return "no errors :)";
 }

String OV7670_write_register(int reg_address, byte data){
  return OV7670_write(reg_address, &data, 1);
 }




String write_key_registers(){
  OV7670_write_register(CC7, 0x80);   //setting to 1 resets all registers
  delay(100);

  OV7670_write_register(IC, 0xC0);    //IC selection
  OV7670_write_register(CC3, 0x08);  //Vertical & mirror flip
  OV7670_write_register(CC7, 0x0C);   //Color & RGB bar selection 0E
  OV7670_write_register(CC15, 0xD0);  //Color Pixel
  OV7670_write_register(CC17, 0x00);//Color Bar 08
  OV7670_write_register(MVFP, 0x0B);
 }

void set_color_matrix(){
    OV7670_write_register(0x4f, 0x80);
    OV7670_write_register(0x50, 0x80);
    OV7670_write_register(0x51, 0x00);
    OV7670_write_register(0x52, 0x22);
    OV7670_write_register(0x53, 0x5e);
    OV7670_write_register(0x54, 0x80);
    OV7670_write_register(0x56, 0x40);
    OV7670_write_register(0x58, 0x9e);
    OV7670_write_register(0x59, 0x88);
    OV7670_write_register(0x5a, 0x88);
    OV7670_write_register(0x5b, 0x44);
    OV7670_write_register(0x5c, 0x67);
    OV7670_write_register(0x5d, 0x49);
    OV7670_write_register(0x5e, 0x0e);
    OV7670_write_register(0x69, 0x00);
    OV7670_write_register(0x6a, 0x40);
    OV7670_write_register(0x6b, 0x0a);
    OV7670_write_register(0x6c, 0x0a);
    OV7670_write_register(0x6d, 0x55);
    OV7670_write_register(0x6e, 0x11);
    OV7670_write_register(0x6f, 0x9f);
    OV7670_write_register(0xb0, 0x84);
}
