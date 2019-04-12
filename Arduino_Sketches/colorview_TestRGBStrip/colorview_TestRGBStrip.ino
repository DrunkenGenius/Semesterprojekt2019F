#include <SPI.h>
#include <Adafruit_DotStar.h>

#define NUMPIXELS 60

//Create the Strip

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);


#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)

// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode false

// our RGB -> eye-recognized gamma color
byte gammatable[256];


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);
  //Serial.println("Color View Test!");

strip.begin(); //Initializes the strip
  strip.show();   //Turns off Led's within NUMPIXELS
  
  if (tcs.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
    //Serial.println(gammatable[i]);
  }
}

// The commented out code in loop is example of getRawData with clear value.
// Processing example colorview.pde can work with this kind of data too, but It requires manual conversion to 
// [0-255] RGB value. You can still uncomments parts of colorview.pde and play with clear value.
void loop() {
  float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  
  tcs.setInterrupt(true);  // turn off LED
  int tempColor = red * 1.2;
  if(red>green && red > blue){
    if(red*1.5>255){
      green -= 40;
        blue -= 40;
      red=255;
      }else{
        green *= 0.7;
        blue *= 0.7;
        red *= 1.5;
        }
      }

   if(green>red && green > blue){
    if((green * 1.5)>255){
      red *= 0.7;
        blue *= 0.7;
      green=255;
      }else{
        red *= 0.7;
        blue *= 0.7;
        green *= 1.5;
        }
         }

         if(blue>red && blue > green){
    if((blue * 1.5)>255){
      green *= 0.7;
        blue *= 0.7;
      blue=255;
      }else{
        red *= 0.7;
        green *= 0.7;
        blue *= 1.5;
        }
         }


  

   Serial.print("R:\t"); Serial.print(int(red)); 
 Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));

//  Serial.print("\t");
 // Serial.print((int)red, HEX); Serial.print((int)green, HEX); Serial.print((int)blue, HEX);
  Serial.print("\n");


    
    
  


  setColor(red,green,blue);

//  uint16_t red, green, blue, clear;
//  
//  tcs.setInterrupt(false);  // turn on LED
//
//  delay(60);  // takes 50ms to read
//
//  tcs.getRawData(&red, &green, &blue, &clear);
//  
//  tcs.setInterrupt(true);  // turn off LED
//
//  Serial.print("C:\t"); Serial.print(int(clear)); 
//  Serial.print("R:\t"); Serial.print(int(red)); 
//  Serial.print("\tG:\t"); Serial.print(int(green)); 
//  Serial.print("\tB:\t"); Serial.print(int(blue));
//  Serial.println();
}

void setColor(uint8_t red,uint8_t green, uint8_t blue){
  for(int i=0; i<5 ; i++){
    strip.setPixelColor(i, green,red,blue);
    }
    strip.show();
  }
