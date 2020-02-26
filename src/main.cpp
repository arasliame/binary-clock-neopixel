#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 6

#define COLOR_ORDER GRB
#define CHIPSET WS2811

#define BRIGHTNESS 20

const int kMatrixWidth = 5;
const int kMatrixHeight = 5;


//technically not a matrix but it's coded for one
const bool kMatrixSerpentineLayout = false; 

//helper func to get coordinates of LED
int XY( int x, int y)
{
  int i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      int reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  
  return i;
}

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}

byte const widthMask = (1 << kMatrixWidth) - 1;
byte matrix[kMatrixHeight];
unsigned long const maxCount = pow(2, (kMatrixHeight * kMatrixWidth));

//converts a number into a byte array
//could potentially simplify this whole thing to not use any matrix logic
//but it was originally built with a matrix rather than a strip
void numToBits(unsigned long value, byte arr[]) {
  //could use some error checking
  int row = 0;
  while (value > 0) { 
    arr[row] = byte(value) & widthMask;
    row++;//array row
    value >>= kMatrixWidth;
  }
}

void displayBits(byte arr[]) {
  for (int row = 0; row < kMatrixHeight; row++) {
    for (int col = 0; col < kMatrixWidth; col++) {
      if (bitRead(arr[row],col)) {
        leds[XYsafe(col,row)] = CRGB::DarkGreen;  //update this to have a variable or something for the color
      }
      else leds[XYsafe(col,row)] = CRGB::Black; 
    }
  }
  FastLED.show();
}

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
}

unsigned long count = 200000;
unsigned long delaytime1 = 1000;


void loop() {
  numToBits(count,matrix);
  displayBits(matrix);
  //Serial.print(matrix);
  count--;
  //if (count > 10) {count = 0;}
  //Serial.println(count);
  //testloop();
  delay(delaytime1);
}

//could also add something configurable to toggle the brightness/color
//also want to be able to toggle counting down day/year/month
//need to add rtc