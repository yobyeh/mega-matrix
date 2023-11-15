#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>
#include <avr/pgmspace.h>
#include <SD.h>
#include <Wire.h>

//TODO
//types of signal zigzag
//dynamic file names
//code based colors
//menues
//buttons
//comments
//cleanup


//SD card
File myFile;
File root;
const int chipSelect = 53;
char buffer1[8];


//LCD
// set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);

//buttons
const int btn1Pin = 2;
byte button1State = LOW;
unsigned long lastButtonTime = 0;

//leds
#define LED_PIN     6
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    500
const int numberOfLeds = 500;
#define BRIGHTNESS  150
CRGB leds[NUM_LEDS];
int frame = 0;
bool flip = false;
//signal wiring shape?


//delays
unsigned long currentMillis = 0;
const long period = 300;//-----------
unsigned long previousMillis = 0;
unsigned long previousMillisDisplay = 0;

//animation
int numberOfFrames = 20;//----------
int frameNumber = 1;
int numberOfLoops = 0;
bool frameDisplayed = true;
bool frameBuffered = false;


//program vars
int programNumber = 1;
int numberOfPrograms = 10;//----------
//1-5
int brightnessSetPoint = 4;

//pre declare functions
void displayProgram();
void checkBtns();
void runLeds();
void buildFrame();
void advanceProgram();
void setBrightness();


void setup() {
  delay(1000); // sanity delay
  Serial.begin(9600);

  //SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //open file
  myFile = SD.open("triangle.txt");
  if(myFile){
    Serial.println("opened file");
    Serial.println(myFile.name());
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }

  //buttons
  //pinMode(btn1Pin, INPUT);

  //leds
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  //animations
  

  //start lcd
  //lcd.init();
  //lcd.clear();         
  //lcd.backlight();    // Make sure backlight is on

  // Print a message on both lines of the LCD.
  //lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  //lcd.print("   Program");
  //lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
  //lcd.print("    Start");
  //delay(3000);
  //lcd.clear();
  //displayProgram();


  for(int i = 0; i < numberOfLeds; i++){
    leds[i] = 0x000000;
  }
  FastLED.show();
  Serial.println("clear");
  delay(1000);

}

void loop(){
  //time and delays
  //currentMillis = millis();

  //Serial.println(programNumber);
  //checkBtns();
  buildFrame();
  runLeds();
  //advanceProgram();//---------
 
}

void setBrightness(){
switch (brightnessSetPoint)
{
case 1:
  FastLED.setBrightness(50);
  break;
case 2:
  FastLED.setBrightness(100);
  break;
case 3:
  FastLED.setBrightness(150);
  break;
case 4:
  FastLED.setBrightness(180);
  break;
case 5:
  FastLED.setBrightness(200);
  break;
default:
  FastLED.setBrightness(150);
  break;
}
lcd.setCursor(0,0);
lcd.print("Brightness: ");
lcd.print(brightnessSetPoint);

}

void checkBtns(){
  button1State = digitalRead(btn1Pin);
  //Serial.println("check,,,,,,,,,,,,,,,");
  //debounce
  unsigned long currentTime = millis();

  if (button1State == HIGH){
    if (lastButtonTime < currentTime - 200){
      brightnessSetPoint++;
      if(brightnessSetPoint > 5){
        brightnessSetPoint = 1;
      }
      setBrightness();

      lastButtonTime = currentTime;
    }
  }

}


void displayProgram(){
  
}

void hexStringToLong(String recv){
  
}

void buildFrame(){
  for(int i=0; i<NUM_LEDS; i++){
    if(myFile.peek() == -1){
      break;
    }
    char tempChar = myFile.peek();
    while(tempChar == ',' || tempChar == ' ' || tempChar == '[' || tempChar == ']' ){
      myFile.read();
      tempChar = myFile.peek();
      if(myFile.peek() == -1){
        break;
      }
    }
    myFile.read(buffer1, 8);
    buffer1[8] = '\0'; // Null-terminate the string

    // Convert hex string to a color and store in leds array
    unsigned long hexColor = strtoul(buffer1, NULL, 16);
    leds[i] = hexColor; // Directly assign the converted color to leds array
  }
 
}

void runLeds(){
  
        FastLED.show();
    
  
}

void advanceProgram(){//--------

  
}