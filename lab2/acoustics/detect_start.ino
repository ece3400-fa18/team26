#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
const int microp = A0; //microphone
int freq_m = 0; //raw value read from microphone
int bin = 5; //CHANGE VALUES
int mag = 0; //CHANGE VALUES 
int uncert = 0; //the allowed amount of uncertanity (frequency)
int start_b  = 0;

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
  freq_m = analogRead(microp); //reads raw data from microphone
  //convert to frequency domain
    while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      fft_input[i] =  freq_m; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    Serial.println("print");
    while(1);
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();
    for (byte i = 0 ; i < FFT_N/2 ; i++) { 
      Serial.println('bin number' + i + 'output' + fft_log_out[i]); // send out the data
    }
  }
}

