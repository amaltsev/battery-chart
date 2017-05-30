#include "Config.h"
#include "Display.h"

Display::Display() {

}

void Display::setup() {
  matrix.setup();
  matrix.setBrightness(3);
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

  if(debug) {
    Serial.print("V2P("); Serial.print(volts); Serial.print(","); Serial.print(vmin); Serial.print(","); Serial.print(vmax);
    Serial.print("="); Serial.println(pix);
  }

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

