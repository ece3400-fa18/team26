#include <Wire.h>

#define OV7670_I2C_ADDRESS /*TODO: write this in hex (eg. 0xAB) */
#define CC7 0x9//Common Control 7
#define CC3 0x6//Common Control 3
#define IC 0x8//Internal Clock
#define CC15 0x20//Common Control 15
#define CC17 0x4//Common Control 17
#define MVFP 0xF//MVFP
#define GAIN 0//GAIN

///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // TODO: READ KEY REGISTERS
  read_key_registers();
  delay(100);
  
  // TODO: WRITE KEY REGISTERS
  write_key_registers();
  
  read_key_registers();
}

void loop(){
 }


///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
  read_register_value(CC7) //Common Control 7
  Serial.println("Common Control 7")
  Serial.println(read_register_value(CC7))
  read_register_value(CC3) //Common Control 3
  Serial.println("Common Control 3")
  Serial.println(read_register_value(CC3))
  read_register_value(IC)//Internal Clock
  Serial.println("Internal Clock")
  Serial.println(read_register_value(IC))
  read_register_value(CC15)//Common Control 15
  Serial.println("Common Control 15")
  Serial.println(read_register_value(CC15))
  read_register_value(CC17)//Common Control 17
  Serial.println("Common Control 17")
  Serial.println(read_register_value(CC17))
  read_register_value(MVFP)//MVFP
  Serial.println("MVFP")
  Serial.println(read_register_value(MVFP))
  read_register_value(GAIN)//GAIN
  Serial.println("GAIN")
  Serial.println(read_register_value(GAIN))
}

byte read_register_value(int register_address){
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
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
  //OV7670_write_register(CC7, 7)//setting to 1 resets all registers
  OV7670_write_register(CC3, 3)//setting to 1 enables scale control
  OV7670_write_register(CC7, 2)//RGB selection
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
