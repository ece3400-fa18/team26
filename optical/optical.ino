#define LOG_OUT 1
#define FFT_N 256

#include <FFT.h>

int threshold = 40;
int time_threshold = 10;
int treasure_7 = 0;
int treasure_12 = 0;
int treasure_17 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  TIMSK0 = 0;  //turn off timer0
  ADCSRA = 0xe5;  //set the adc to free running mode
  ADMUX = 0x40; //use adc0
  DIDR0 = 0x01;
  Serial.println("start treasure hunt");
}

void loop() {
  // put your main code here, to run repeatedly:
  while(1){
    cli(); //UDRE interrupt
    for(int i = 0; i < 512; i+=2){
      while(!(ADCSRA & 0x10)); //waiting for adc to be ready
      ADCSRA = 0xf5; //restart ADC
      byte m = ADCL;
      byte j = ADCH;
      int k = (j << 8) | m;
      k -= 0x0200;
      k <<= 6;
      fft_input[i] = k;
      fft_input[i+1] = 0;
    } 

    fft_window();
    fft_reorder();
    fft_run();
    fft_mag_log();
    sei(); //turns interrupts back on

    
    //for (byte i = 0; i < FFT_N/2; i++){
    //  Serial.println(fft_log_out[i]);
    //}
    //Serial.println("basdfasdfjkahdflkasdfhkaljdfa");
    trasureDetect();
  }

}

void trasureDetect(){
  int max_7_bin = 0;
  for (int i = 46; i < 50; i++){
    int x = (int) fft_log_out[i];
    if(x > max_7_bin){
      max_7_bin = x;
    }
  }

  int max_12_bin = 0;
  for (int i = 80; i < 83; i++){
    int x = (int) fft_log_out[i];
    if(x > max_12_bin){
      max_12_bin = x;
    }
  }

  int max_17_bin = 0;
  for (int i = 113; i < 116; i++){
    int x = (int) fft_log_out[i];
    if(x > max_17_bin){
      max_17_bin = x;
    }
  }

  if (max_17_bin > max_12_bin && max_17_bin > max_7_bin) {
      if (max_17_bin > threshold) 
       treasure_17++;
    }
    else if (max_12_bin > max_7_bin) {
      if (max_12_bin > threshold)  
       treasure_12++;
    }
    else 
      if (max_7_bin > threshold)
        treasure_7++;
    if (treasure_7 > time_threshold) {
      Serial.write("7 kHz treasure!\n");
      treasure_7 = 0;
    }
    else if (treasure_12 > time_threshold) {
      Serial.write ("12 kHz treasure!\n");
      treasure_12 = 0;
    }
    else if (treasure_17 > time_threshold) {
      Serial.write ("17 kHz treasure!\n");
      treasure_17 = 0;
    }
}
