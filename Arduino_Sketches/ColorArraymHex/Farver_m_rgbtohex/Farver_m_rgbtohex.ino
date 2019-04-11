int redLed = 3; //true
int greenLed = 5; //virker meget dominerende
int blueLed = 6;

int ledPins[] = {redLed, greenLed, blueLed};

//unsigned int rgbColor[3] = {0,0,255};

int colorArray[21][3] = {{0,255,0},{0,255,50},{0,255,100},{0,255,150},{0,255,255},{0,100,255},{0,0,255},{50,0,255},{150,0,255},{255,0,255},{255,0,200},{255,0,100},{255,0,50},{255,0,0},{255,40,0},{255,75,0},{255,100,0},{255,150,0},{255,200,0},{255,255,0},{150,255,0}};
int colorHexArray[21] = {}
//HUSK DET ER GRB

//USES COMMON CATHODE : : RESISITORS 3X200OHM

/*
--Greens to Blue--
Very green: {0,255,0}
Green turkish: {0,255,50}
Turkish: {0,255,100}
Light Blue turkish: {0,255,150}
Light Blue: {0,255,255}
Blue: {0,100,255}
Dark purple -> blue {0,0,255}

--Green to red--
Green: [0,255,0}
Green with hint of yellow: {150,255,0}
A green yellow: {255,255,0}
dark yellow: {255,200,0}
yellow {255,150,0}
light orange {255,100,0}
orange {255,75,0}
red orange {255,40,0}
red {255,0,0}

--Red to blue--
Fuscia (dark pink): {255,0,50}
Pink/lyserød: {255,0,100}
//Lavender/lavendel: {255,0,100}
Lilla: {255,0,200}
Violet: {255,0,255}
Orchid (Lys lilla): {150,0,255}
blålilla {50,0,255}
blå{0,0,255}
*/





void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  
  // put your setup code here, to run once:
  

}

void loop() {
  for (int i = 0; i<21; i++){
    for (int j = 0; j<3; j++){      
        analogWrite(ledPins[j], colorArray[i][j]);    
      }
  delay(1000);
  }
}
