#include <Adafruit_NeoPixel.h>
#include <AltSoftSerial.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <SoftwareSerial.h>

const int IN_BUF_MAX = 3;
const byte SERIAL_SOF = 0xA5;
const byte SERIAL_EOF = 0x5A;
const int TIMEOUT = 100;
byte in_msg[IN_BUF_MAX];
#define PIN            6
#define NUMPIXELS      300
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay for half a second

AltSoftSerial softy;
void setup() {
    Serial.begin(9600);
    softy.begin(9600);
    #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif
    pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
  uint8_t bytes_received;
  bytes_received = receiveMessage(in_msg, TIMEOUT);
  if(bytes_received >0){
    Serial.println(in_msg[0]);
    //Conditionals for Cloud Data
    if(in_msg[0]==0x01){
      clearClouds();
    }
    else if(in_msg[0]==0x02){
      rainbowCycle();
    }
    else{
      grey();
    }
  }
  
}

/****************************************************************
 * Receiver
 ***************************************************************/
uint8_t receiveMessage(byte msg[], unsigned long timeout) {
  
  boolean valid;
  uint8_t buf_idx;
  unsigned long start_time;
  byte in_buf[IN_BUF_MAX];
  byte r;
  
  // Wait until we get a valid message or time out
  start_time = millis();
  valid = false;
  while ( !valid ) {
    
    // Wait until we see a Start of Frame (SOF) or time out
    memset(in_buf, 0, IN_BUF_MAX);
    while ( !softy.available() ) {
      if ( (millis() - start_time > timeout) && (timeout > 0) ) {
        return 0;
      }
    }
    r = softy.read();
    if ( r != SERIAL_SOF ) {
      continue;
    }

    // Read buffer
    delay(2);  // Magical delay to let the buffer fill up
    buf_idx = 0;
    while ( softy.available() > 0 ) {
      if ( buf_idx >= IN_BUF_MAX ) {
        buf_idx = IN_BUF_MAX - 1;
      }
      in_buf[buf_idx] = softy.read();
#if DEBUG
      Serial.print("0x");
      Serial.print(in_buf[buf_idx], HEX);
      Serial.print(" ");
#endif
      buf_idx++;
      delay(2);  // Magical delay to let the buffer fill up
    }
#if DEBUG
    Serial.println();
#endif

    if ( in_buf[buf_idx - 1] == SERIAL_EOF ) {
      valid = true;
    }
  }
  
  // Copy our message (don't copy checksum or EOF bytes)
  memcpy(msg, in_buf, buf_idx - 1);
  
  return buf_idx - 1;
}
void clearClouds(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,150));
    pixels.show();
  }
}
void grey(){
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(20,20,20));
    pixels.show();
  }
}
void rainbowCycle() {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(1);
  }
}
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
