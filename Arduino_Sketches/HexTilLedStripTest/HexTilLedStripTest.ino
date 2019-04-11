#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    4
#define CLOCKPIN   5

// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);


//int colorArray[21][3] = {{0,255,0},{0,255,50},{0,255,100},{0,255,150},{0,255,255},{0,100,255}k,{0,0,255},{50,0,255},{150,0,255},{255,0,255},{255,0,200},{255,0,100},{255,0,50},{255,0,0},{255,40,0},{255,75,0},{255,100,0},{255,150,0},{255,200,0},{255,255,0},{150,255,0}};
uint32_t colorHexArray[21] = {0xFF0000,0xFF0032,0xFF0064,0xFF0096,0xFF00FF,0x6400FF,0x0000FF,0x0032FF,0x9600FF,0x00FFFF,0x00FFC8,0x00FF64,0x00FF32,0x00FF00,0x28FF00,0x4BFF00,0x64FF00,0x96FF00,0xC8FF00,0xFFFF00,0x96FF00};

//HUSK DET ER GRB
const int buttonPin = 2;

int buttonState = 1;

void setup() {
  
  pinMode(buttonPin, INPUT_PULLUP);

  
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  Serial.begin(9600);

}
//________________________________________________________________


void loop() {

  if (digitalRead(buttonPin) == 0){
      buttonStateSetterChecker();
    }
  

  if (buttonState == 1){
    fixedColorArray();
    }
  else if (buttonState == 2){
    chaosColorArray();
  }

 

  
  

  
}

void buttonStateSetterChecker(){
  
  }

void chaosColorArray(){
   for (int i = 0 ; i < 21 ; i++){
    int randnum = rand() % 23 + 0;
    uint32_t color = colorHexArray[randnum];
    setColor(color);
    delay(100);
  }
}

void fixedColorArray(){
  for (int i = 0 ; i < 21 ; i++){
    uint32_t color = colorHexArray[i];
    setColor(color);
    delay(1000);
  }
}

void setColor(uint32_t color){
  for(int i=0; i<10; i++){
    strip.setPixelColor(i, color);
    }
    strip.show(); //refresh
  }
