#include "Config.h"
#include "Display.h"

Display::Display() {

}

void Display::setup() {
  matrix.setup();
  matrix.setBrightness(0);
  matrix.setRotation(1);
  matrix.clear();
  matrix.writeDisplay();
}

static uint8_t pixelValue(float volts, float vmin, float vmax) { 
  uint8_t pix;
  if(volts < vmin)
    pix=0;
  else if(volts > vmax)
    pix=15;
  else {
    float t=(volts-vmin)*15/(vmax-vmin);
    pix=(uint8_t)(t + 0.5);
  }

//  if(debug) {
//    Serial.print("V2P("); Serial.print(volts); Serial.print(","); Serial.print(vmin); Serial.print(","); Serial.print(vmax);
//    Serial.print("="); Serial.println(pix);
//  }

  return pix;
}

void Display::loop(Voltage &vdata) {

  // Greeting if this the first run
  //
  if(!greetingShown) {
    greetingShown=true;
    greeting();
  }

  // Rotation
  //
  matrix.setRotation(opts.rotateScreen ? 3 : 1);

  // Brightness
  //
  ///// FIXME

  // Chart
  //
  chart(vdata);
}

void Display::chart(Voltage &vdata) {
  matrix.clear();
  
  for(uint8_t i=vdata.getChannelFirst(); i<=vdata.getChannelLast(); ++i) {
    float volts=vdata.getLine(i);

    // The 16 pixels we have display values from MIN (2.8V) to
    // MAX (4.3V) in 0.1V steps.
    //
    uint8_t vpix=pixelValue(volts,CHART_VOLT_MIN,CHART_VOLT_MAX);

    drawColumn(i,vpix);
  }
  
  // If the rightmost columns are empty (they normally are) -- showing
  // the total in their place.
  //
  if(vdata.getChannelLast() < 30) {
    uint8_t active=vdata.getActiveCount();
    uint8_t vpix=pixelValue(vdata.getTotal(),CHART_VOLT_MIN * active, CHART_VOLT_MAX * active);
    drawColumn(31,vpix);
  }

  matrix.writeDisplay();
}

void Display::drawColumn(uint8_t column, uint8_t value) {
  if(opts.fillChart) {
    matrix.drawLine(column,15,column,15-value,LED_ON);
  }
  else {
    matrix.drawPixel(column,15-value,LED_ON);
    if(opts.drawBottom)
      matrix.drawPixel(column,15,LED_ON);
  }
}

void Display::greeting() {
  if(!opts.showGreeting)
    return;
    
  matrix.clear();
//  matrix.setFont(&FreeSans9pt7b);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  matrix.setTextColor(LED_ON);
  matrix.setCursor(8,0);
  matrix.print("OKA");
  matrix.setCursor(0,8);
  matrix.print("ELECTRO");
  matrix.writeDisplay();
  delay(1000);
}



//void testMatrix() {
//  for(uint8_t r=0; r<4; ++r) {
//    matrix.setRotation(r);
//    
//    matrix.clear();
//    matrix.drawPixel(0,0,LED_ON);
//    matrix.drawPixel(0,3,LED_ON);
//    matrix.drawPixel(2,0,LED_ON);
//    matrix.drawPixel(31,15,LED_ON);
//    
//  //  matrix.setTextSize(1);
//  //  matrix.setTextWrap(false);
//  //  matrix.setTextColor(LED_ON);
//  //  matrix.setCursor(0,6);
//  //  matrix.print("Hello World");
//  
//    matrix.writeDisplay();
//  
//    delay(1000);
//  
//    for(int i=0; i<32; i++) {
//      matrix.clear();
//      matrix.drawLine(0,0,i,15,LED_ON);
//      matrix.writeDisplay();
//      delay(100);
//    }
//    
//    matrix.clear();
//    matrix.drawLine(0,0, 31,15, LED_ON);
//    matrix.writeDisplay();
//    delay(1000);
//  
//    matrix.clear();
//    matrix.drawRect(0,0, 31,15, LED_ON);
//    matrix.fillRect(2,2, 4,4, LED_ON);
//    matrix.writeDisplay();
//    delay(1000);
//  
//    matrix.clear();
//    matrix.drawCircle(10,8, 5, LED_ON);
//    matrix.writeDisplay();
//    delay(1000);
//  
//    matrix.setTextSize(2);
//    matrix.setTextWrap(false);
//    matrix.setTextColor(LED_ON);
//  
//    for (int16_t x=32; x>=-210; --x) {
//      matrix.clear();
//      matrix.setCursor(x,0);
//      matrix.print("OKA Battery Meter");
//      matrix.writeDisplay();
//      delay(25);
//    }
//  
//    delay(1000);
//  }
//}
