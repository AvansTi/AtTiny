#include <Adafruit_NeoPixel.h>
#include "TinyWireS.h"                  // wrapper class for I2C slave routines

#define PIN 4
Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, PIN, NEO_GRB + NEO_KHZ800);

#define I2C_SLAVE_ADDR  0x26            // i2c slave address (38)


#define CMD_SETPIXEL      1
#define CMD_SHOW          2
#define CMD_RAINBOWPIXEL  3
#define CMD_STATUSON      4
#define CMD_STATUSOFF     5

#define CMD_MAX           7

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  for (int i = -64; i < 255 + 32; i++)
  {
    for (int l = 0; l < strip.numPixels(); l++)
    {
      int dist = min(255, abs(i - (255 / 6) * l) * 4);
      strip.setPixelColor(l, strip.Color(255 - dist, (255 - dist) / 3, 0));
    }
    strip.show();
    delay(2);
  }
  TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode
//  Blink(3, 4);                 // show we transmitted
}


byte command[4];
int index = 0;
unsigned long lastrecv = millis();

bool initializing = true;
int initializeCounter = 0;



void loop() {
  // put your main code here, to run repeatedly:
  byte byteRcvd = 0;
  if (TinyWireS.available()) {          // got I2C input!
    if (initializing)
    {
      for (int l = 0; l < strip.numPixels(); l++)
        strip.setPixelColor(l, strip.Color(0, 0, 0));
      strip.show();
    }
    initializing = false;


    unsigned long time = millis();
    if (time - lastrecv > 200)
      index = 0;
    lastrecv = time;
    command[index++] = TinyWireS.receive();     // get the byte from master
    int opcode = command[0] & 0x7;
    if (opcode == CMD_SETPIXEL)
    {
      if(index == 4)
      {
        int pixel = command[0] >> 3;
        strip.setPixelColor(pixel, strip.Color(command[1], command[2], command[3]));
        index = 0;   
      }
    }
    else if (opcode == CMD_SHOW)
    {
    
      strip.show();
      index = 0;
    }
    else if (opcode == CMD_STATUSON)
    {
      digitalWrite(3, HIGH);
      index = 0;
    }
    else if (opcode == CMD_STATUSOFF)
    {
      digitalWrite(3, LOW);
      index = 0;
    }
  }
  if (initializing)
  {
    for (int i = 0; i < 6; i++)
      strip.setPixelColor(i, strip.Color(initializeCounter % 32, initializeCounter % 32, initializeCounter % 32));
    strip.show();

    initializeCounter++;
    delay(50);
  }



}



void Blink(byte led, byte times) {
  for (byte i = 0; i < times; i++) {
    digitalWrite(led, HIGH);
    delay (250);
    digitalWrite(led, LOW);
    delay (175);
  }
}
