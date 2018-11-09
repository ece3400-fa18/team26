#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21
//read: 
#define CC7 0x12//Common Control 7
#define CC3 0x0C//Common Control 3
#define IC 0x11//Internal Clock
#define CC15 0x40//Common Control 15
#define CC14 0x3E//Common Control 14
#define CC17 0x42//Common Control 17
#define MVFP 0x1E//MVFP


///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("test");
  // TODO: READ KEY REGISTERS
  read_key_registers();

  delay(100);
  
  // TODO: WRITE KEY REGISTERS
  write_key_registers();
  set_color_matrix();
  read_key_registers();
  
}

void loop(){

 }


///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
//  read_register_value(CC7); //Common Control 7
  Serial.print("Common Control 7:");
  Serial.print(read_register_value(CC7),HEX);
  Serial.println();
//  read_register_value(CC3); //Common Control 3
  Serial.print("Common Control 3:");
  Serial.print(read_register_value(CC3),HEX);
  Serial.println();
//  read_register_value(IC);//Internal Clock
  Serial.print("Internal Clock");
  Serial.print(read_register_value(IC),HEX);
  Serial.println();
//  read_register_value(CC15);//Common Control 15
  Serial.print("Common Control 15");
  Serial.print(read_register_value(CC15),HEX);
  Serial.println();
//  read_register_value(CC17);//Common Control 17
  Serial.print("Common Control 17");
  Serial.print(read_register_value(CC17),HEX);
  Serial.println();
//  read_register_value(MVFP);//MVFP
  Serial.print("MVFP");
  Serial.print(read_register_value(MVFP),HEX);
  Serial.println();

}

byte read_register_value(int register_address){
//  Serial.println("entering read register");
  byte data = 0;
//  Serial.println("test 5");
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
//  Serial.println("test 6");
  Wire.write(register_address);
//  Serial.println("test 7");
  Wire.endTransmission();
//  Serial.println("test 8");
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
//  Serial.println("test 9");
  Serial.println(Wire.available());
  while(Wire.available()<1);
//  Serial.println("test 10");
  data = Wire.read();
//  Serial.println("test 11");
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
  OV7670_write_register(CC7, 0x80);//setting to 1 resets all registers
  delay(100);
  OV7670_write_register(CC3, 0x08);//setting to 1 enables scale control
  OV7670_write_register(IC, 0xC0);//IC selection
  OV7670_write_register(MVFP, 0x30);//Vertical &mirror flip
  OV7670_write_register(CC7, 0x0E);//Color & rgb bar selection
  OV7670_write_register(CC15, 0xD0);//Color Pixel
  OV7670_write_register(CC17, 0x08);//Color Bar IF DOESN'T WORK TRY 0C
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
