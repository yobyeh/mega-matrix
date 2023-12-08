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
char buffer[11];


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


//leds
#define LED_PIN     6
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define NUM_LEDS    700
const int numberOfLeds = 700;
#define WIDTH        14
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
int codePlayFrames = 800;
//fire
#define FIRE_FRAMES_PER_SECOND 10
#define COOLING  90 //50
#define SPARKING 80 //120
//star
int count = 0; 
int countStart = 0;
bool newStar = false;
//unsigned long starWaitTime = 0;
//unsigned long currentStarWait = 0;

//int numberOfFrames = 20;//----------
//int frameNumber = 1;
//int numberOfLoops = 0;
//bool frameDisplayed = true;
//bool frameBuffered = false;


//program vars
int currentProgram = 1;
bool nextProgramFlag = false;
bool playCode = true;

//pre declare functions
void checkBtns();
void ledShow();
void buildFrame();
void setBrightness(int);
void menuCheck();
void menuInterrupt();
void ledTest();
void playFireFrame();
void Fire2012(int);
void ledClear();
int parseRGB(int);
CRGB myHeatColor();
void lcdWrite(String, String, int); //writes 2 lines then delay till clear. 0 to leave on screen
void lcdClear();
void displayMenu(int);
void loadNextfile();
void printDirectory(File, int);
void shootingStarAnimation(int, int, int, int, int, int);
void runStars();
void bouncingColoredBalls(int BallCount, byte colors[][3]);

//void playProgram(int);

void setup() {
  delay(5000); // sanity delay
  Serial.begin(9600);
  currentMillis = millis();

  //SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  root = SD.open("/programs");
  if (!root) {
  Serial.println("Failed to open /programs directory");
  return;
  } else {
  Serial.println("/programs directory opened");
  }
  myFile = root.openNextFile();
  if (!myFile) {
  Serial.println("Failed to open the first file in the directory");
  } else {
  Serial.println("First file opened successfully");
  Serial.print("File name: ");
  Serial.println(myFile.name());
  }


  //buttons
  //pinMode(btn1Pin, INPUT);
  pinMode(menuPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(menuPin), menuInterrupt, RISING);

  //leds
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  //start lcd
  lcd.init();
  lcd.clear();         
  lcd.backlight();    // Make sure backlight is on

  lcdWrite("Mega", "Matrix", 1000);
  //ledTest();
  ledClear();
}

void loop(){
  //time and delays
  //currentMillis = millis();
  
  //Serial.println(programNumber);
  //checkBtns();
  menuCheck();
  if(nextProgramFlag){
    myFile = root.openNextFile();
    if(!myFile){
      playCode = true;
      setBrightness(2);
      root.rewindDirectory();
      myFile = root.openNextFile();
    }
    nextProgramFlag = false;
    delay(1000);
  }

  if(playCode){
    for(int i=0; i<codePlayFrames; i++){
      //playFireFrame();
       // A simple White Shooting Star
      //runStars();
      byte colors[3][3] = { {0xff, 0,0},
                        {0xff, 0xff, 0xff},
                        {0, 0xdd, 0} };

      bouncingColoredBalls(3, colors);
      
      // Try changing the arguments. Uncomment this line of code to try out a fully randomized animation example:
      //shootingStarAnimation(random(0, 255), random(0, 255), random(0, 255), random(10, 60), random(5, 40), random(2000, 8000), 1);
    }
    playCode = false;
    setBrightness(3);
  }else{
    buildFrame();
  }
 
}

void loadNextFile(){
  //if()


  myFile = SD.open("tree2.txt");
  if(myFile){
    Serial.println("opened file");
    Serial.println(myFile.name());
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }

}


//dealy of 0 to keep on screen
void lcdWrite(String line1, String line2, int showDelay){
  lcdClear();
  int textLength;
  const char* cLine1 = line1.c_str();
  const char* cLine2 = line2.c_str();
  int length1 = strlen(cLine1);
  int length2 = strlen(cLine2);
  if(length1 > length2){
    textLength = length1;
  }else{
    textLength = length2;
  }
  for(int i = 0; i < textLength; i++){
    if(i < length1){
      lcd.setCursor(i,0);
      lcd.print(cLine1[i]);
    }
    if(i < length2){
      lcd.setCursor(i,1);
      lcd.print(cLine2[i]);
    }
    delay(60);
  }
  if(showDelay > 0){
    delay(showDelay);
    lcdClear();
  }
}

void lcdClear(){
  for(int i = 15; i > -1; i--){
    lcd.setCursor(i,0); 
    lcd.print(" ");
    lcd.setCursor(i,1);
    lcd.print(" ");
    delay(60);
  }
}

void buildFrame(){
    char tempChar;
    for(int i=0; i<NUM_LEDS; i++){
      do {
        if(myFile.peek() == -1){
          nextProgramFlag = true;
          return;
        } 
        tempChar = myFile.read();
      } while(tempChar == ',' || tempChar == ' ' || tempChar == '[' || tempChar == ']' || tempChar == '"');
    myFile.read(buffer, 11);
    int red = parseRGB(0);
    int green = parseRGB(4);
    int blue = parseRGB(8);
    leds[i] = CRGB(red, green, blue);
  }
  ledShow();
}

int parseRGB(int startIndex){
  char colorValue[4];
  strncpy(colorValue, buffer + startIndex, 3);
  return atoi(colorValue);
}

void ledShow(){
        FastLED.show();
}

void ledClear(){
  for(int i = 0; i < numberOfLeds; i++){
    leds[i] = 0x000000;
  }
  FastLED.show();
  //lcdWrite("Clear", "LEDs", 1500);
}

void ledTest(){
  lcdWrite("RGB", "Check", 2000);
  for (int i = 0; i < 1; i++) {
    lcdWrite("Black", "", 0);
    for(int i = 0; i < numberOfLeds; i++){
      leds[i] = 0x000000;
    }
    FastLED.show();
    Serial.println("clear");
    delay(1000);
    lcdWrite("Red", "", 0);
    for(int i = 0; i < numberOfLeds; i++){
      //fire brick
      leds[i] = 0xB22222;
    }
    FastLED.show();
    delay(3000);
    lcdWrite("Green", "", 0);
    for(int i = 0; i < numberOfLeds; i++){
      //lime green
      leds[i] = 0x32CD32;
    }
    FastLED.show();
    delay(3000);
    lcdWrite("Blue", "", 0);
    for(int i = 0; i < numberOfLeds; i++){
      //medium blue
      leds[i] = 0x0000CD;
    }
    FastLED.show();
    delay(3000);
    lcdWrite("Black", "", 0);
    for(int i = 0; i < numberOfLeds; i++){
      leds[i] = 0x000000;
    }
    //FastLED.show();
    Serial.println("clear");
    delay(3000);
    lcdClear();
  }
}

void menuInterrupt(){
  menuRequest = true;
}

void menuCheck(){
  int mainMenuLocation = 1;
  int mainMenuSelection = 0;
  while(menuRequest == true){
    lcdWrite("Menu", "", 1000);
    //displayMenu(mainMenuLocation);
    //int buttonType = buttonCheck();
    //if(buttonType != 0){
      //buttonLogic(buttonType, mainMenuLocation, mainMenuSelection);
    //}
    
    menuRequest = false;
  }
}

struct MenuItem
{
  String name1;
  String name2;
};

MenuItem mainMenu[] = {
  {"Stataic", "Displays"},
  {"Select", "Program"},
  {"Set", "Brightness"},
  {"Test", "LEDs"}
};


void displayMenu(int mainMenuLocation){
  
  switch (mainMenuLocation)
  {
  case 1:
      lcdWrite("Static", "Displays", 0);
        break;
  case 2:
      lcdWrite("Sleect", "Program", 0);
        break;
  case 3:
      lcdWrite("Set", "Brightness", 0);
        break;
  case 4:
      lcdWrite("Test", "Leds", 0);
        break;            
  default:
    break;
  }

}

void buttonLogic(int whatButton, int currentMainMenue, int newMainMenu){
  switch (whatButton)
  {
  case 1:
    //enter
    //menuLogic(currentMainMenue);
    break;
  case 2:
    //up
    newMainMenu --;
    break;
  case 3:
    //down
    newMainMenu ++;
    break;
  default:
    break;
  }
}

void brightnessMenu(){

}

void staticMenu(){
    // Action for Menu Item 1
}

void programMenu(){
    // Action for Menu Item 2
}

//1-5
void setBrightness(int brightness){
  switch (brightness)
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
}


//balls
void bouncingColoredBalls(int BallCount, byte colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 300;
 
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
 
  for (int i = 0 ; i < BallCount ; i++) {  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
 
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * ((NUM_LEDS - 1)) / StartHeight);
    }
 
    for (int i = 0 ; i < BallCount ; i++) {
      //leds[i] = CRGB(red, green, blue);
      leds[Position[i]] = CRGB(colors[i][0],colors[i][1],colors[i][2]);
    }
   
    FastLED.show();
    ledClear();
  }
}

//shooting star
void runStars(){
  shootingStarAnimation(255, 255, 255, random(60, 80), random(5, 40), 1);
}

void shootingStarAnimation(int red, int green, int blue, int tail_length, int delay_duration, int direction){
  /*
   * red - 0 to 255 red color value
   * green - 0 to 255 green color value
   * blue - 0 to 255 blue color value
   * tail_length - value which represents number of pixels used in the tail following the shooting star
   * delay_duration - value to set animation speed. Higher value results in slower animation speed.
   * interval - time between each shooting star (in miliseconds)
   * direction - value which changes the way that the pixels travel (uses -1 for reverse, any other number for forward)
  */
  //unsigned long currentMillis = millis();   // Get the time
  if (countStart + 50 <= count) {
    //previousMillis = currentMillis;         // Save the last time the LEDs were updated
    count = 0 + (random(0, 14)*50);         // Reset the count to 0 after each interval
    countStart = count;
  }
  if (direction == -1) {        // Reverse direction option for LEDs
    if (count < NUM_LEDS) {
      leds[NUM_LEDS - (count % (NUM_LEDS+1))].setRGB(red, green, blue);    // Set LEDs with the color value
      count++;
    }
  }
  else {
    if (count < NUM_LEDS) {     // Forward direction option for LEDs
      leds[count % (NUM_LEDS+1)].setRGB(red, green, blue);    // Set LEDs with the color value
      count++;
    }
  }
  fadeToBlackBy(leds, NUM_LEDS, tail_length);                 // Fade the tail LEDs to black
  FastLED.show();
  delay(delay_duration);                                      // Delay to set the speed of the animation
}



//fire
void playFireFrame(){
  // Add entropy to random number generator; we use a lot of it.
  
  for(int w = 0; w < WIDTH; w++){
    random16_add_entropy( random());
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
