#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
int bin = 22;
int mag = 50; 
static int a = 0;

void start() {
digitalWrite(13, HIGH);
} 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // use serial port
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Goal: detect 660 Hz
  //In Arduino the ADC clock is preset to 16 MHz/128 = 125 KHz. However sampling rate can be increased.
  //Each conversion in AVR takes 13 ADC clocks so 125 KHz /13 = 9615 Hz.
  //start is signaled by 660Hz, so analogRead is sufficient to fulfill the Nyquist Criteria
  //convert to frequency domain
 while(1) {
    cli();
    for (int i = 0 ; i < 512 ; i += 2) {
      fft_input[i] = analogRead(A1);
      fft_input[i+1] = 0;
    }
    fft_window();
    fft_reorder();
    fft_run();
    fft_mag_log();
    sei();
        if (fft_log_out[22] > mag) {
          start();
          Serial.println("start");
          a =1;
          }
        while(a){
          Serial.println("working");
         }
   } 
}


