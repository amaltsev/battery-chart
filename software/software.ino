#include "MultiMatrix.h"

#define DEBUG 1

// My matrices are wired with 0x70 on the right
//
MultiMatrix matrix = MultiMatrix(4,(uint8_t[]){0x73,0x72,0x71,0x70});

void setup() {
  if(DEBUG) Serial.begin(9600);
  
  matrix.setup();
  matrix.setBrightness(3);
  matrix.setRotation(3);
}

void loop() {
  for(uint8_t r=0; r<4; ++r) {
    matrix.setRotation(r);
    
    matrix.clear();
    matrix.drawPixel(0,0,LED_ON);
    matrix.drawPixel(0,3,LED_ON);
    matrix.drawPixel(2,0,LED_ON);
    matrix.drawPixel(31,15,LED_ON);
    
  //  matrix.setTextSize(1);
  //  matrix.setTextWrap(false);
  //  matrix.setTextColor(LED_ON);
  //  matrix.setCursor(0,6);
  //  matrix.print("Hello World");
  
    matrix.writeDisplay();
  
    delay(1000);
  
    for(int i=0; i<32; i++) {
      matrix.clear();
      matrix.drawLine(0,0,i,15,LED_ON);
      matrix.writeDisplay();
      delay(100);
    }
    
    matrix.clear();
    matrix.drawLine(0,0, 31,15, LED_ON);
    matrix.writeDisplay();
    delay(1000);
  
    matrix.clear();
    matrix.drawRect(0,0, 31,15, LED_ON);
    matrix.fillRect(2,2, 4,4, LED_ON);
    matrix.writeDisplay();
    delay(1000);
  
    matrix.clear();
    matrix.drawCircle(10,8, 5, LED_ON);
    matrix.writeDisplay();
    delay(1000);
  
    matrix.setTextSize(2);
    matrix.setTextWrap(false);
    matrix.setTextColor(LED_ON);
  
    for (int16_t x=32; x>=-210; --x) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print("OKA Battery Meter");
      matrix.writeDisplay();
      delay(25);
    }
  
    delay(1000);
  }
}
