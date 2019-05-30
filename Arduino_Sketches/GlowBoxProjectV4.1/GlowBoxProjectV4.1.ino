#include <Wire.h>
#include <SPI.h>
#include <Adafruit_DotStar.h> //LED STRIP
#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL343.h> //Accelerometer
#include <Adafruit_TCS34725.h> //Flora Color Sensor

unsigned int ownRGBColor[3] = {0, 0, 0}; //This box's colors

//Basic Colors
//HEX
uint32_t colorHexArray[21] = {0xFF0000,0xFF0032,0xFF0064,0xFF0096,0xFF00FF,0x6400FF,0x0000FF,0x0032FF,0x9600FF,0x00FFFF,0x00FFC8,0x00FF64,0x00FF32,0x00FF00,0x28FF00,0x4BFF00,0x64FF00,0x96FF00,0xC8FF00,0xFFFF00,0x96FF00};
uint32_t colorArray[21][3] = {{0,255,0},{0,255,50},{0,255,100},{0,255,150},{0,255,255},{0,100,255},{0,0,255},{50,0,255},{150,0,255},{255,0,255},{255,0,200},{255,0,100},{255,0,50},{255,0,0},{255,40,0},{255,75,0},{255,100,0},{255,150,0},{255,200,0},{255,255,0},{150,255,0}};
unsigned int hexColorIterator = 0;

int stripLength = 40;

int modeButtPin = 3;

int stripInterval = 2;

int colorMode = 0; //0 == Color array ; 1 == Random Colors ; 2 == Absorb colors;

int shakeValue = 40;
int partyShakeValue = 70;


bool colorMode0 = false; //Til at vide om man har skiftet colorModes for at lave en transition når man går i ny mode
bool colorMode1 = false; //Til at vide om man har skiftet colorModes for at lave en transition når man går i ny mode
bool colorMode2 = false; //Til at vide om man har skiftet colorModes for at lave en transition når man går i ny mode

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

  int totalAccelleration = abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z); //Record Current Acceleration

  Serial.print("Acceleration : ");
  Serial.println(totalAccelleration);

  if(colorMode == 0 || colorMode == 1){

    if(colorMode == 0 && colorMode0 == false) //If we are changing to colormode 0
    {
        RandomTransition();
        colorMode0 = true;
        colorMode1 = false;
    }
        
    if(colorMode == 1 && colorMode1 == false) //If we are changing to colormode 1
    {
      FixedTransition();
      colorMode1 = true;
      colorMode2 = false; 
    } 
    if(totalAccelleration > partyShakeValue  && colorMode == 0) //EXCESSIVE SHAKING RESULSTS IN A PARTY
    {
      ChangeEachAdressableRandom();
      ChangeEachAdressableRandom();
    }
    else if(totalAccelleration > shakeValue)  //Regular shaking detection
    {
      if(colorMode == 0)
      {
        ChangeRandomColor();  //Chaos Color Change
      }
      if(colorMode == 1)
      {
        NextColorInFixedArray(); //Fixed Array Color Change
      }
      delay(50);
    }
  }
  if(colorMode == 2) //CHAOS ABSORB
  {
      if(colorMode2 == false) //Transition to the mode
      {
        Serial.println("AAAA GG COLORMODE 2");
        ChangeEachAdressableRandom();
        colorMode2 = true;
        colorMode0 = false;
       }
       
      tcs.setInterrupt(false);  // turn off LED
      ChaosAbsorbColors();
  }
  Serial.print("Button: ");
  Serial.println(digitalRead(modeButtPin));
  Serial.print("Mode: ");
  Serial.println(colorMode); 



   //BUTTON HAS BEEN PRESSED
  if(digitalRead(modeButtPin) == 0){
     /*
     * Check whether the button has been pushed in long enough to absorb colors, or if it should just switch mode.
     */
    int pushDelay = 0;    //Integer to mark for how long the button has been pushed down
    while(pushDelay < 50 && digitalRead(modeButtPin) == 0){ //While the time hasn't passed, and the button hasn't been lifted again.
      pushDelay ++;
      delay(10);
    }

    if(digitalRead(modeButtPin) == 0){ //If the button hasn't been released
      AbsorbColors();   //Go to the function that absorbs colors
    }else{    //If the button is no longer pressed
      delay(400);

      if(digitalRead(modeButtPin) == 0){ //If the button is pressed now, after the delay
        CombineColor();
      }else{
        colorMode = (colorMode + 1) % 3; //If we don't use any of the advanced functions, it iterates through the modes as normal.
      }
    }
    
    if(colorMode == 2){
      tcs.setInterrupt(false); //Turn on LED
    }else{
      tcs.setInterrupt(true); //Turn off LED
    }
    
   // delay(100); //var på 1000 ced ikke om der er en grund til dette?
  }
}



//opdeler RGB'en i de enkelte farver og sætter farven på strippen
void SetColor(uint8_t red,uint8_t green, uint8_t blue){

  ownRGBColor[0] = red;
  ownRGBColor[1] = green;
  ownRGBColor[2] = blue;

  float intensity = 0.5;
  
  for(int i=0; i<stripLength; i++){
    if((i%stripInterval)==0){
    strip.setPixelColor(i, green*intensity,red*intensity,blue*intensity);
    }
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
     SetColor(red,green,blue);
      Serial.print("R:\t"); Serial.print(int(red)); 
      Serial.print("\tG:\t"); Serial.print(int(green)); 
      Serial.print("\tB:\t"); Serial.print(int(blue));

    Serial.print("\n");
}

void AbsorbColors(){
  int totalAccelleration;
  do{
    float red, green, blue; //Prepare RGB values for reading.
    
    tcs.setInterrupt(false); //Turn on RGB sensor

    delay(50); //Give the sensor time to read colors
    
    tcs.getRGB(&red, &green, &blue); //Get RGB values

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

    SetColor(red, green, blue);
    
  }while(digitalRead(modeButtPin) == 0);

  delay(1000); //An arbritrary delay to prevent color change for a second after the button has been released

  do{
    delay(10);

    //Acelleration detection
    sensors_event_t event;
    accel.getEvent(&event);
    totalAccelleration = abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z);

    
  }while(totalAccelleration < shakeValue && digitalRead(modeButtPin) != 1); //While there's no shake, and the button isn't pushed down

  colorMode = 0;
}

void FixedTransition(){
      SetColor(255,0,0);
      delay(333);
      SetColor(0,255,0);
      delay(333);
      SetColor(0,0,255);
      delay(333);
  }

void RandomTransition(){
  for(int i = 0; i<25; i++){
    ChangeRandomColor();
    delay(50);
    }
  }

void CombineColor(){
  float red, green, blue, ownRed, ownGreen, ownBlue;

  ownRed = ownRGBColor[0];
  ownGreen = ownRGBColor[1];
  ownBlue = ownRGBColor[2];

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);

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

  delay(500);

  SetColor((ownRed + red) / 2, (ownGreen + green) / 2, (ownBlue + blue) / 2);

  int totalAccelleration;

  delay(500);
  
  do{
    delay(10);
    
    sensors_event_t event;
    accel.getEvent(&event);
    totalAccelleration = abs(event.acceleration.x) + abs(event.acceleration.y) + abs(event.acceleration.z);
    
  }while(totalAccelleration < shakeValue && digitalRead(modeButtPin) == 1);

  colorMode = 0;
}

//Kører kronologisk igennem arrayet af RGB-colors
void NextColorInFixedArray(){
  Serial.println("fixedArray Shake");

   if(hexColorIterator < 21){
    SetColor(colorArray[hexColorIterator][0], colorArray[hexColorIterator][1], colorArray[hexColorIterator][2]);
    hexColorIterator++;
        Serial.println(hexColorIterator);

    delay(200);
   }
   else{
        hexColorIterator = 0;
    SetColor(colorArray[hexColorIterator][0], colorArray[hexColorIterator][1], colorArray[hexColorIterator][2]);
    Serial.println("RESET!");
    Serial.println(hexColorIterator);
    delay(200);
    }
}


void setFullColor(uint32_t color){
  for(int i=0; i<stripLength; i++){
    strip.setPixelColor(i, color);
    }
    strip.show(); //refresh
  }

void ChangeRandomColor(){
   Serial.println("Random Shake");
    int randomHex = random(0,21);
    SetColor(colorArray[randomHex][0], colorArray[randomHex][1], colorArray[randomHex][2]);
   }


void ChangeEachAdressableRandom(){
  for(int j=0; j<20; j++){
    for(int i=0; i<stripLength ; i++){
      int randomHex = random(0,21);
        if((i%stripInterval)==0){
           strip.setPixelColor(i, colorArray[randomHex][0], colorArray[randomHex][1], colorArray[randomHex][2]);
           delay(5);
         }
      }
      strip.show();
  }
 }   
