#include <Wire.h>
#include <SPI.h>
#include <Adafruit_DotStar.h> //LED STRIP
#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL343.h> //Accelerometer
#include <Adafruit_TCS34725.h> //Flora Color Sensor

unsigned int rgbColor1[3] = {255, 0, 0}; //This box's colors
unsigned int rgbColor3[3] = {0, 0, 0}; //The two boxes' colors combined.

//Basic Colors
//HEX
uint32_t colorHexArray[21] = {0xFF0000,0xFF0032,0xFF0064,0xFF0096,0xFF00FF,0x6400FF,0x0000FF,0x0032FF,0x9600FF,0x00FFFF,0x00FFC8,0x00FF64,0x00FF32,0x00FF00,0x28FF00,0x4BFF00,0x64FF00,0x96FF00,0xC8FF00,0xFFFF00,0x96FF00};
unsigned int hexColorIterator = 0;

int stripLength = 10;

int modeButtPin = 4;

int colorMode = 0; //0 == Color array ; 1 == Random Colors ; 2 == Absorb colors;

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345); // Accelerometer
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //Color sensor


// Here's how to control the LEDs from any two pins:
#define NUMPIXELS 60 // Number of LEDs in strip

// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);


void setup() {
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

  //random seed der giver talstøj til vores randomværdi
  randomSeed(analogRead(0));
  
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
  tcs.setInterrupt(true); //Turn off LED

  ChangeRandomColor(); // Sæt random farve startup
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  int totalAccelleration = abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z);

  //Serial.print("Acceleration : ");
  //Serial.println(totalAccelleration);

  if(colorMode == 0 || colorMode == 1){
    if(totalAccelleration > 40){
      if(colorMode == 1){
        ChangeRandomColor();
      }
      if(colorMode == 0){
        NextColorInFixedArray();
      }
    delay(50);
  }
  }
  if(colorMode == 2){
      tcs.setInterrupt(false);  // turn off LED
      ChaosAbsorbColors();
  }
  
  Serial.println(digitalRead(modeButtPin));
  Serial.println(colorMode); 
    
  if(digitalRead(modeButtPin) == 1){
     /*
     * Check whether the button has been pushed in long enough to absorb colors, or if it should just switch mode.
     */
    int pushDelay = 0;    //Integer to mark for how long the button has been pushed down
    while(pushDelay < 50 && digitalRead(modeButtPin) == 1){ //While the time hasn't passed, and the button hasn't been lifted again.
      pushDelay ++;
      delay(10);
    }

    if(digitalRead(modeButtPin) == 1){ //If the button hasn't been released
      AbsorbColors();   //Go to the function that absorbs colors
      
    }
    
    colorMode = (colorMode + 1) % 3;
    if(colorMode == 2){
      tcs.setInterrupt(false); //Turn on LED
    }else{
      tcs.setInterrupt(true); //Turn off LED
    }
    delay(1000);
  }
}

//opdeler RGB'en i de enkelte farver og sætter farven på strippen
void setColor(uint8_t red,uint8_t green, uint8_t blue){
  for(int i=0; i<stripLength ; i++){
    strip.setPixelColor(i, green,red,blue);
    }
    strip.show();
  }

//Læser og sætter farven konstant
void ChaosAbsorbColors(){
  Serial.println("CHAAAAAAAAAAAAAAAAAAAAAAAAAAAAOOOOOOOOS");
  float red, green, blue;

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue); //Får farverne 

  //Disse IF-statements kalibrerer farverne så de passer bedre på den rigtige farve der bliver aflæst, fremfor at det bare bliver vidt
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
     setColor(red,green,blue);
      Serial.print("R:\t"); Serial.print(int(red)); 
      Serial.print("\tG:\t"); Serial.print(int(green)); 
      Serial.print("\tB:\t"); Serial.print(int(blue));

//  Serial.print("\t");
 // Serial.print((int)red, HEX); Serial.print((int)green, HEX); Serial.print((int)blue, HEX);
    Serial.print("\n");
}
void AbsorbColors(){
  int totalAccelleration;
  //TO-DO: While there is a button press, continue.

  do{
    float red, green, blue; //Prepare RGB values for reading.
    
    tcs.setInterrupt(false); //Turn on RGB sensor

    delay(50); //Give the sensor time to read colors
    
    tcs.getRGB(&red, &green, &blue); //Get RGB values

    setColor(red, green, blue);
    
  }while(digitalRead(modeButtPin) == 1);

  delay(1000); //An arbritrary delay to prevent color change for a second after the button has been released

  do{
    delay(10);

    //Acelleration detection
    sensors_event_t event;
    accel.getEvent(&event);
    totalAccelleration = abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z);

    
  }while(totalAccelleration < 40 && digitalRead(modeButtPin) != 1); //While there's no shake, and the button isn't pushed down
  
}

//Kører kronologisk igennem arrayet af HEX-colors
void NextColorInFixedArray(){

   if(hexColorIterator < 21){
    uint32_t color = colorHexArray[hexColorIterator];
    setFullColor(color);
    hexColorIterator++;
        Serial.println(hexColorIterator);

    delay(500);
   }
   else{
        hexColorIterator = 0;
    uint32_t color = colorHexArray[hexColorIterator];
    setFullColor(color);
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

void setFullColor(uint32_t color){
  for(int i=0; i<stripLength; i++){
    strip.setPixelColor(i, color);
    }
    strip.show(); //refresh
  }

void ChangeRandomColor(){
 int randomHex = random(0,21);
    uint32_t color = colorHexArray[randomHex];
    setFullColor(color);
    delay(500);
   }