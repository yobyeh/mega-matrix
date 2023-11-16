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

//menue interupt
volatile bool menuRequest = false;
const int menuPin = 2;
//menue vars
int menuLevel = 0;

//leds
#define LED_PIN     6
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    500
const int numberOfLeds = 500;
#define WIDTH        10
#define SINGLE_STRIP 50
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
#define FIRE_FRAMES_PER_SECOND 8
#define COOLING  55
#define SPARKING 120
//int numberOfFrames = 20;//----------
//int frameNumber = 1;
//int numberOfLoops = 0;
//bool frameDisplayed = true;
//bool frameBuffered = false;


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
void menuCheck();
void menuInterrupt();
void ledTest();
void playFireFrame();
void Fire2012(int);
CRGB myHeatColor();


void setup() {
  delay(1000); // sanity delay
  Serial.begin(9600);
  currentMillis = millis();

  //SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //open file
  myFile = SD.open("pac.txt");
  if(myFile){
    Serial.println("opened file");
    Serial.println(myFile.name());
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }

  //buttons
  //pinMode(btn1Pin, INPUT);
  pinMode(menuPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(menuPin), menuInterrupt, RISING);

  //leds
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  //animations
  

  //start lcd
  lcd.init();
  lcd.clear();         
  lcd.backlight();    // Make sure backlight is on

  // Print a message on both lines of the LCD.
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("Mega");
  lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
  lcd.print("Matrix");
  delay(3000);
  lcd.clear();

  //RGB test
  //ledTest();


  //displayProgram();

  
  
}

void loop(){
  //time and delays
  //currentMillis = millis();

  //Serial.println(programNumber);
  //checkBtns();
  menuCheck();
  //playFireFrame();
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

void ledTest(){
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("RGB Check");
  delay(2500);
  lcd.clear();
  for (int i = 0; i < 1; i++) {
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Off");
    for(int i = 0; i < numberOfLeds; i++){
      leds[i] = 0x000000;
    }
    FastLED.show();
    Serial.println("clear");
    delay(1000);

    lcd.clear();
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Red");
    for(int i = 0; i < numberOfLeds; i++){
      //fire brick
      leds[i] = 0xB22222;
    }
    FastLED.show();
    delay(3000);

    lcd.clear();
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Green");
    for(int i = 0; i < numberOfLeds; i++){
      //lime green
      leds[i] = 0x32CD32;
    }
    FastLED.show();
    delay(3000);

    lcd.clear();
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Blue");
    for(int i = 0; i < numberOfLeds; i++){
      //medium blue
      leds[i] = 0x0000CD;
    }
    FastLED.show();
    delay(3000);

    lcd.clear();
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Off");
    for(int i = 0; i < numberOfLeds; i++){
      leds[i] = 0x000000;
    }
    FastLED.show();
    Serial.println("clear");
    delay(3000);
    lcd.clear();
  }
}

void advanceProgram(){//--------

  
}

void menuInterrupt(){
  menuRequest = true;
}

void menuCheck(){
  if(menuRequest == true){
    Serial.println("menu");
    
  // Print a message on LCD.
  lcd.clear();
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("Menu");
  lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
  lcd.print("");
  delay(3000);
  lcd.clear();


    menuRequest = false;
  }
}


//for fire

void playFireFrame(){
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());
  for(int w = 0; w < WIDTH; w++){

    Fire2012(w); // run simulation frame

  }

  

  FastLED.show(); // display this frame
  delay(1000 / FIRE_FRAMES_PER_SECOND);
}

void Fire2012(int columnNumber){

// Array of temperature readings at each simulation cell
  static byte heat[SINGLE_STRIP];
 
  // Step 1.  Cool down every cell a little
    for( int i = 0; i < SINGLE_STRIP; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / SINGLE_STRIP) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for(int k = SINGLE_STRIP - 1; k > 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 3]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
 
    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < SINGLE_STRIP; j++) {
      int tempIndex;
      if (columnNumber % 2 == 0) {
        tempIndex = j + (SINGLE_STRIP * columnNumber);
      } 
      else {
        tempIndex = (SINGLE_STRIP * (columnNumber + 1)) - 1 - j;
      }
      leds[tempIndex] = HeatColor( heat[j]);  
    }
}

CRGB myHeatColor( uint8_t temperature)
{
  CRGB heatcolor;
  
  // Scale 'heat' down from 0-255 to 0-191,
  // which can then be easily divided into three
  // equal 'thirds' of 64 units each.
  uint8_t t192 = scale8_video( temperature, 192);
 
  // calculate a value that ramps up from
  // zero to 255 in each 'third' of the scale.
  uint8_t heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // now figure out which third of the spectrum we're in:
  if( t192 & 0x80) {
    // we're in the hottest third
    heatcolor.r = 255; // full red
    heatcolor.g = 255; // full green
    heatcolor.b = heatramp; // ramp up blue
    
  } else if( t192 & 0x40 ) {
    // we're in the middle third
    heatcolor.r = 255; // full red
    heatcolor.g = heatramp; // ramp up green
    heatcolor.b = 0; // no blue
    
  } else {
    // we're in the coolest third
    heatcolor.r = heatramp; // ramp up red
    heatcolor.g = 0; // no green
    heatcolor.b = 0; // no blue
  }
  
  return heatcolor;
}
