#include <Wire.h>
#include <SPI.h>
#include <Adafruit_DotStar.h> //LED STRIP
#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL343.h> //Accelerometer
#include <Adafruit_TCS34725.h> //Flora Color Sensor

int redLed = 3; //true
int greenLed = 5; //virker meget dominerende
int blueLed = 6;

int ledPins[] = {redLed, greenLed, blueLed}; //The LED pins are thrown in a array for easier coding.
int modeButtPin = 4;

unsigned int rgbColor1[3] = {255, 0, 0}; //This box's colors
unsigned int rgbColor3[3] = {0, 0, 0}; //The two boxes' colors combined.

//Basic Colors
//GRB
unsigned int rgbColorArray[6][3] = {{255, 0, 0},{255, 255, 0},{0, 255, 0},{0, 255, 240},{0, 40, 255},{255, 0, 255}};
int iterator = 0;
//HEX
uint32_t colorHexArray[21] = {0xFF0000,0xFF0032,0xFF0064,0xFF0096,0xFF00FF,0x6400FF,0x0000FF,0x0032FF,0x9600FF,0x00FFFF,0x00FFC8,0x00FF64,0x00FF32,0x00FF00,0x28FF00,0x4BFF00,0x64FF00,0x96FF00,0xC8FF00,0xFFFF00,0x96FF00};
unsigned int hexColorIterator = 0;


int colorMode = 0; //0 == Color array ; 1 == Random Colors ; 2 == Absorb colors;
float greenDownScaleConstant = 1;

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345); // Accelerometer
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //Color sensor


// Here's how to control the LEDs from any two pins:
#define DATAPIN    4
#define CLOCKPIN   5
#define NUMPIXELS 60 // Number of LEDs in strip

// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);


void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);


  //pinMode(iteratorButtPin, INPUT_PULLUP);
  pinMode(modeButtPin, INPUT_PULLUP);

  //LED-strip setup
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
 if(!tcs.begin()){
    Serial.println("Ooops, no TCS34725 detected ... Check your wiring!");
    while(1);
  }  

}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  int totalAccelleration = abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z);

  Serial.print("Acceleration : ");
  Serial.println(totalAccelleration);
  
  if(totalAccelleration > 40){
    Serial.println("Shaking");
          NextColorInFixedArray();
    
    if(colorMode == 0){
      ChangeRandomColor();
    }else if(colorMode == 1){
      NextColorInFixedArray();
    }
    delay(50);
  }
  if(colorMode == 2){
      ChaosAbsorbColors();
  }
  
  Serial.println(digitalRead(modeButtPin));
  Serial.println(colorMode); 
    
  if(digitalRead(modeButtPin) == 1){
    colorMode = (colorMode + 1) % 3;
    if(colorMode == 2){
//      tcs.setInterrupt(false); //Turn on LED
    }else if(colorMode == 0){
//      tcs.setInterrupt(true); //Turn off LED
    }
    delay(1000);
  }

   float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);


   Serial.print("R:\t"); Serial.print(int(red)); 
 Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));

//  Serial.print("\t");
 // Serial.print((int)red, HEX); Serial.print((int)green, HEX); Serial.print((int)blue, HEX);
  Serial.print("\n");

  
}

void SingleColor(){

  float colorConstant = GetColorConstant(rgbColor1[0], rgbColor1[1], rgbColor1[2]);
  
  for(int i = 0; i < 3; i++){
    rgbColor1[i] *= colorConstant;

    if(i == 1)
      analogWrite(ledPins[i], rgbColor1[i] * greenDownScaleConstant);
     else
      analogWrite(ledPins[i], rgbColor1[i]);
  }
  
}

/*void CombineColor(){
  for (int i = 0; i<3; i++){

    rgbColor3[i] = (rgbColor1[i]+rgbColor2[i])/2;
    
  }

  float colorConstant = GetColorConstant(rgbColor3[0], rgbColor3[1], rgbColor3[2]);
  
  Serial.println(colorConstant);
  
  for(int i = 0; i < 3; i++){
    rgbColor3[i] *= colorConstant;
    
    if(i == 1)
      analogWrite(ledPins[i], rgbColor1[i] * greenDownScaleConstant);
     else
      analogWrite(ledPins[i], rgbColor1[i]);
  }

  
  Serial.print("red: ");
  Serial.println(rgbColor3[0]);
  Serial.print("green: ");
  Serial.println(rgbColor3[1]);
  Serial.print("blue: ");
  Serial.println(rgbColor3[2]);
}*/

float GetColorConstant(int red, int gre, int blu){
  int rgbCol[3] = {red, gre, blu};
  float colorConstant;
  if(rgbCol[0] >= rgbCol[1] && rgbCol[0] >= rgbCol[2]){
    colorConstant = (float)255 / (float)rgbCol[0];
  } else if(rgbCol[1] >= rgbCol[0] && rgbCol[1] >= rgbCol[2]){
    colorConstant = (float)255 / (float)rgbCol[1];
  } else if(rgbCol[2] >= rgbCol[1] && rgbCol[2] >= rgbCol[0]){
    colorConstant = (float)255 / (float)rgbCol[2];
  }
  return colorConstant;
}

void AbsorbColors(){

  
}

void ChaosAbsorbColors(){
  float red, green, blue;

//  tcs.getRGB(&red, &green, &blue);

  int oldColors[] = {rgbColor1[0], rgbColor1[1], rgbColor1[2]};
  int newColors[] = {red, green, blue};

  for(int i = 0; i < 100; i++){
    for(int i2 = 0; i2 < 3; i2++){
      rgbColor1[i2] = oldColors[i2] + (float)(newColors[i2] - oldColors[i2]) * 0.01 * i;
    }
    SingleColor();
    delay(2.5);
  }
  
  for(int i = 0; i < 3; i++){
    rgbColor1[i] = newColors[i];
    Serial.print("  ");
    Serial.print(rgbColor1[i]);
  }
  Serial.println();
}

void NextColorInFixedArray(){

   if(hexColorIterator < 21){
    uint32_t color = colorHexArray[hexColorIterator];
    setColor(color);
    hexColorIterator++;
        Serial.println(hexColorIterator);

    delay(500);
   }
   else{
        hexColorIterator = 0;
    uint32_t color = colorHexArray[hexColorIterator];
    setColor(color);
    Serial.println("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEESET!");
    Serial.println(hexColorIterator);
    delay(500);
    }
  
  /*
  iterator++;
  if(iterator >= 6){
    iterator = 0;
  }

  int oldColors[] = {rgbColor1[0], rgbColor1[1], rgbColor1[2]};
  int newColors[] = {rgbColorArray[iterator][0], rgbColorArray[iterator][1], rgbColorArray[iterator][2]};

  for(int i = 0; i < 100; i++){
    for(int i2 = 0; i2 < 3; i2++){
      rgbColor1[i2] = oldColors[i2] + (float)(newColors[i2] - oldColors[i2]) * 0.01 * i;
    }
    SingleColor();
    delay(2.5);
  }
  
  for(int i = 0; i < 3; i++){
    rgbColor1[i] = newColors[i];
    Serial.print("  ");
    Serial.print(rgbColor1[i]);
  }
  Serial.println();
  */
}

void setColor(uint32_t color){
  for(int i=0; i<10; i++){
    strip.setPixelColor(i, color);
    }
    strip.show(); //refresh
  }

void ChangeRandomColor(){
  int oldColors[] = {rgbColor1[0], rgbColor1[1], rgbColor1[2]};
  int newColors[3];

  for(int i = 0; i < 3; i++){
    newColors[i] = random(255);
  }

  if(newColors[0] < newColors[1] && newColors[0] < newColors[2]){
    newColors[0] /= 2;
  } else if(newColors[1] < newColors[0] && newColors[1] < newColors[2]){
    newColors[1] /= 2;
  } else if(newColors[2] < newColors[1] && newColors[2] < newColors[0]){
    newColors[2] /= 2;
  }


  for(int i = 0; i < 100; i++){
    for(int i2 = 0; i2 < 3; i2++){
      rgbColor1[i2] = oldColors[i2] + (float)(newColors[i2] - oldColors[i2]) * 0.01 * i;
    }
    SingleColor();
    delay(2.5);
  }
  
  for(int i = 0; i < 3; i++){
    rgbColor1[i] = newColors[i];
  }
  //delay(500);
}
