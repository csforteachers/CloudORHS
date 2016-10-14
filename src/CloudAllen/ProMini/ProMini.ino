/**
 *  ProMini.ino
 *
 *  Created on: 21.09.2016
 *  Author: Vasanth Sadhasivan
 */
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
#define PIN 6
#define NUMPIXELS 300
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay for half a second

AltSoftSerial softy;
void setup() {
  Serial.begin(9600);
  softy.begin(9600);
  #if defined(__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
  uint8_t bytes_received;
  bytes_received = receiveMessage(in_msg, TIMEOUT);
  if (bytes_received > 0) {
    Serial.println(in_msg[0]);
    int randomseed = random(1);
    //Conditionals for Cloud Data
    Serial.println("Recieved in_msg: "+in_msg[0]);
    if (in_msg[0] == 0x01) {
      if(randomseed){
        clearClouds();
      }else{
        springDay();
      }
    }else if (in_msg[0] == 0x02) {
      if(randomseed){
        lightningStorm();
      }else{
        rain();
      }
    }else if (in_msg[0] == 0x03) {
      if(randomseed){
        lightningStorm();
      }else{
        rain();
      }
    }else if (in_msg[0] == 0x04) {
      rain();
    }else if (in_msg[0] == 0x05) {
      rain();
    }else if (in_msg[0] == 0x06) {
      snow();
    }else if (in_msg[0] == 0x07) {
      rainbowCycle();
    }else if (in_msg[0] == 0x08) {
      cloudy();
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
  while (!valid) {

    // Wait until we see a Start of Frame (SOF) or time out
    memset(in_buf, 0, IN_BUF_MAX);
    while (!softy.available()) {
      if ((millis() - start_time > timeout) && (timeout > 0)) {
        return 0;
      }
    }
    r = softy.read();
    if (r != SERIAL_SOF) {
      continue;
    }

    // Read buffer
    delay(2); // Magical delay to let the buffer fill up
    buf_idx = 0;
    while (softy.available() > 0) {
      if (buf_idx >= IN_BUF_MAX) {
        buf_idx = IN_BUF_MAX - 1;
      }
      in_buf[buf_idx] = softy.read();
      #if DEBUG
      Serial.print("0x");
      Serial.print(in_buf[buf_idx], HEX);
      Serial.print(" ");
      #endif
      buf_idx++;
      delay(2); // Magical delay to let the buffer fill up
    }
    #if DEBUG
    Serial.println();
    #endif

    if (in_buf[buf_idx - 1] == SERIAL_EOF) {
      valid = true;
    }
  }

  // Copy our message (don't copy checksum or EOF bytes)
  memcpy(msg, in_buf, buf_idx - 1);

  return buf_idx - 1;
}
void clearClouds() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 150));
    pixels.show();
  }
}
void rainbowCycle() {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(1);
  }
}
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//User submitted code

void sunrise() { //Zach & Lasse
  for (int i = 0; i < 100; i++) { //for the first 100 LEDs
    pixels.setPixelColor(i, 150, 20, 0);
  }
  delay(2000);
  pixels.show();
  for (int i = 101; i < 150; i++) { //for LEDs 101 to 150
    pixels.setPixelColor(i, 150, 90, 0);
  }
  delay(2000);
  pixels.show();
  for (int i = 151; i < 250; i++) { //for LEDs 151 to 250
    pixels.setPixelColor(i, 150, 50, 0);
  }
  delay(2000);
  pixels.show();
  for (int i = 251; i < 260; i++) { //for LEDs 251 to 260
    pixels.setPixelColor(i, 150, 50, 0);
  }
  delay(2000);
  pixels.show();
  for (int i = 261; i < 300; i++) {
    pixels.setPixelColor(i, 150, 75, 0);
  }
  delay(2000);
  pixels.show();
}

void springDay() { //Wolfgang & Vaughn
  for (int i = 0; i < 280; i++) {
    pixels.setPixelColor(i, 0, 170, 175);
  }
  for (int i = 281; i < 300; i++) {
    pixels.setPixelColor(i, 255, 255, 0);
  }
  pixels.show();
}
void snow() { //Dustin & Jamal
  int x = 0;
  while (true) {
    for (int i = 0; i < 300; i++) {
      pixels.setPixelColor(i, 17, 72, 143);
    }
    for (int i = 0; i < 12; i++) {
      for (int n = 0; n < 3; n++) {
        pixels.setPixelColor((i * 25 + n + x) % 300, 232, 248, 250);
      }
    }
    pixels.show();
    x++;
    if (x >= 300) {
      x -= 300;
    }
    delay(83);
  }
}

void sunset() { //Mihika & Rishika
  for (int k = 90; k < 220; k++) {
    for (int i = 0; i < 300; i++) {
      if (k < 180) {

        pixels.setPixelColor(i, k, 180 - k, 0);
      } else {
        pixels.setPixelColor(i, k, 0, 0);
      }
    }
    delay(50);
    pixels.show();
  }
}
void rainbowSky() { //Will and Ben
  for (int timer = 0; timer < 2; timer++) {

    for (int i = 0; i < 300; i++) {
      if (i >= 0 && i < 50)
        pixels.setPixelColor(i, 255, 0, 0);
      if (i >= 50 && i < 100)
        pixels.setPixelColor(i, 255, 165, 0);
      if (i >= 100 && i < 150)
        pixels.setPixelColor(i, 255, 255, 0);
      if (i >= 150 && i < 200)
        pixels.setPixelColor(i, 0, 255, 0);
      if (i >= 200 && i < 250)
        pixels.setPixelColor(i, 0, 0, 255);
      if (i >= 250 && i < 300)
        pixels.setPixelColor(i, 255, 0, 255);
    }

    pixels.show();

    for (int i = 0; i < 300; i++) {
      pixels.setPixelColor(i, 0, 0, 0);
      pixels.show();
    }

    for (int i = 0; i < 300; i++) {

      if (i >= 0 && i < 50)
        pixels.setPixelColor(i, 255, 0, 0);
      if (i >= 50 && i < 100)
        pixels.setPixelColor(i, 255, 165, 0);
      if (i >= 100 && i < 150)
        pixels.setPixelColor(i, 255, 255, 0);
      if (i >= 150 && i < 200)
        pixels.setPixelColor(i, 0, 255, 0);
      if (i >= 200 && i < 250)
        pixels.setPixelColor(i, 0, 0, 255);
      if (i >= 250 && i < 300)
        pixels.setPixelColor(i, 255, 0, 255);

      pixels.show();
    }
  }
}
void lightningStorm() { //Chris & Ryan
  int num = 4; //Change Later if need be
  for (int i = 0; i < num; i++) {
    //Creates random number to see what bolt happens, 0 = scroll flash, 1-3 is flash, 4 = dual flash
    int randNum = random(4);
    switch (randNum) {
    case 0:
      scrollFlash();
      break;
    case 1:
      flash(random(999) + 500);
      break;
    case 2:
      flash(random(999) + 500);
      break;
    case 3:
      flash(random(999) + 500);
      break;
    case 4:
      dualFlash(random(999) + 500);
      break;
    }
  }
}

//Scroll flash
//Should have lightning go throughout the cloud (in theory, no idea if it will look good or not)
void scrollFlash() {
    //scrolling
    for (int i = 0; i < 30; i++) {
      for (int j = 0; j < 10; j++) {
        pixels.setPixelColor(i * 10 + j, 255, 255, 255);
      }
      pixels.show();
    }

}
//Creats a single flash somehwere in the cloud with an int duration (in miliseconds)
void flash(int duration) {
  int randNum = random(2); //Random number 0-3 for use in deciding bolt location within cloud
  int min, max = 0; //min = minumum light value that the bolt will strike, max = maximum light value bolt will end
  //Determines whether the bolt will be in front, mid, or back of light string
  switch (randNum) {
  case 0: //Bolt in front
    min = random(24); //Starts between 0-24
    max = random(49) + 75; //Ends between 100 - 124
    break;
  case 1: //Bolt in middle
    min = random(49) + 125; //starts between 125 - 174
    max = random(49) + 175; //ends between 175 - 224
    break;
  case 2:
    min = random(49) + 200; //starts between 200 - 249
    max = random(49) + 250; //ends between 250 - 299
    break;
  }
  //activates chosen lights
  for (int i = min; i < max; i++) {
    pixels.setPixelColor(i, 255, 255, 255);
  }
  pixels.show();
  delay(50); //Hopefully creates a quick flicker effect
  lightningReset();
  delay(50);
  for (int i = min; i < max; i++) {
    pixels.setPixelColor(i, 255, 255, 255);
  }
  delay(duration);
  lightningReset(); //Resets lights to nothing at end of program
}

//Creates 2 flashes of lightning, one in the front, one in the back
void dualFlash(int duration) {
  int min1, min2, max1, max2 = 0;
  min1 = random(49);
  max1 = random(49) + 100;
  min2 = random(49) + 150; //Numbers same as flash program, but 2 instead of 1
  max2 = random(49) + 250;
  //Displays first bolt
  for (int i = min1; i < max1; i++) {
    pixels.setPixelColor(i, 255, 255, 255);
  }
  pixels.show();
  delay(50); //"Flicker effect"
  lightningReset();
  for (int i = min1; i < max1; i++) {
    pixels.setPixelColor(i, 255, 255, 255);
  }
  delay(100); //waits 100 miliseconds until 2nd bolt
  //shows second bolt
  for (int i = min2; i < max2; i++) {
    pixels.setPixelColor(i, 255, 255, 255);
  }
  pixels.show();
  delay(50); //"Flicker effect"
  for (int i = min2; i < max2; i++) {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();
  delay(50);
  for (int i = min2; i < max2; i++) {
    pixels.setPixelColor(i, 255, 255, 255);
  }
  pixels.show();
  delay(duration); //shows bolt for duration
  lightningReset(); //resets lights to nothing at end of program
}

//Quick method to reset lights to nothing
void lightningReset() {
  for (int i = 0; i < 300; i++) {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();
}

void rain() { //Bernard & Lan
  for (int x = 0; x < 15; x++) {
    for (int a = 0; a < 300; a++) {
      pixels.setPixelColor(a, 5, 5, 5); //set LEDs a gray color
    }
    for (int b = 0; b < 20; b++) {
      pixels.setPixelColor(15 * b + x, 235, 235, 255); //set rain color
    }
    delay(20);
    pixels.show();
  }
}

void cloudy() { //Bernard & Lan
  for(int loopcount=0; loopcount<5; loopcount++){
    int i,a;
    for(i =100; i<255; i++){
      for (a = 0; a < 300; a++) {
        pixels.setPixelColor(a, i, i, i); //set LEDs a gray color
      }
      delay(5);
      pixels.show();
    }

    for(i =254; i>99; i--){
      for (a = 0; a < 300; a++) {
        pixels.setPixelColor(a, i, i, i); //set LEDs a gray color
      }
      delay(5);
      pixels.show();
    }
  }
}
