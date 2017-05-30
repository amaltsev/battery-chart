#include "Config.h"
#include "Display.h"

static const int ANIMATION_DELAY=10;
static const int TEXT_VIEW_STEPS=1000/ANIMATION_DELAY;

static uint8_t pxWarn1;
static uint8_t pxWarn2;

// Voltage into chart pixel Y coordinate
//
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

//  if(DEBUG_DISPLAY) {
//    Serial.print("V2P("); Serial.print(volts); Serial.print(","); Serial.print(vmin); Serial.print(","); Serial.print(vmax);
//    Serial.print("="); Serial.println(pix);
//  }

  return pix;
}

// Constructor
Display::Display() {

}

// Setup
//
void Display::setup() {
  matrix.setup();
  matrix.setBrightness(0);
  matrix.setRotation(1);
  matrix.clear();
  matrix.writeDisplay();

  // Voltage warnings recalculated into pixels
  //
  pxWarn1=pixelValue(CHART_VOLT_WARN1,CHART_VOLT_MIN,CHART_VOLT_MAX);
  pxWarn2=pixelValue(CHART_VOLT_WARN2,CHART_VOLT_MIN,CHART_VOLT_MAX);
}

// Loop (both chart and intro)
//
void Display::loop(Voltage &vdata) {

  // Rotation
  //
  matrix.setRotation(opts.rotateScreen ? 3 : 1);

  // Brightness
  //
  ///// FIXME

  // Blink phase, just counting here, analyzing when blinking.
  //
  ++blinkPhase;
  
  // Greeting if this the first run
  //
  if(!greetingShown) {
    greetingShown=true;
    greeting();

    // Growing the chart into view for a nice effect
    //
    for(uint8_t cap=0; cap<16; ++cap) {
      pixelCap=cap;
      chart(vdata);
      delay(ANIMATION_DELAY);
    }

    pixelCap=255;
  }

  // Chart
  //
  chart(vdata);
}

// Chart display
//
void Display::chart(Voltage &vdata) {
  matrix.clear();
  
  bool w1Line=false;
  bool w2Line=false;
  bool w1Total=false;
  bool w2Total=false;
    
  for(uint8_t i=vdata.getChannelFirst(); i<=vdata.getChannelLast(); ++i) {
    float volts=vdata.getLine(i);

    // The 16 pixels we have display values from MIN (2.8V) to
    // MAX (4.3V) in 0.1V steps.
    //
    uint8_t vpix=pixelValue(volts,CHART_VOLT_MIN,CHART_VOLT_MAX);

    drawColumn(i,vpix);

    if(vpix<=pxWarn1) w1Line=true;
    if(vpix<=pxWarn2) w2Line=true;
  }
  
  // If the rightmost columns are empty (they normally are) -- showing
  // the total in their place.
  //
  if(vdata.getChannelLast() < VCHANNELS-2 && vdata.getActiveCount()>0) {
    uint8_t active=vdata.getActiveCount();
    
    uint8_t vpix=pixelValue(vdata.getTotal(),CHART_VOLT_MIN * active, CHART_VOLT_MAX * active);
    
    drawColumn(VCHANNELS-1,vpix);

    if(vpix<=pxWarn1) w1Total=true;
    if(vpix<=pxWarn2) w2Total=true;
  }

  // If low voltage showing a blinking indicator of that. One
  // in the total column and one to the right of the line columns.
  //
  if(opts.blinkLow && pixelCap>15) {
    if(w1Line || w2Line)
      drawWarning(vdata.getChannelLast(),w1Line,w2Line);

    if(w1Total || w2Total)
      drawWarning(VCHANNELS-1,w1Total,w2Total);
  }

  matrix.writeDisplay();
}

void Display::drawColumn(uint8_t column, uint8_t value) {
  if(value>pixelCap)
    value=pixelCap;
    
  if(opts.fillChart) {
    matrix.drawLine(column,15,column,15-value,LED_ON);
  }
  else {
    matrix.drawPixel(column,15-value,LED_ON);
    if(opts.drawBottom)
      matrix.drawPixel(column,15,LED_ON);
  }
}

void Display::drawWarning(uint8_t column, bool w1, bool w2) {  
  bool draw=w2 ? (blinkPhase & 1)==0 : (blinkPhase & 2)==0;
  
  if(draw) {  
    matrix.fillRect(column-1,0,3,5,LED_OFF);
    matrix.drawPixel(column,0,LED_ON);
    matrix.drawPixel(column,1,LED_ON);
    matrix.drawPixel(column,3,LED_ON);
  }
}

void Display::greeting() {
  if(!opts.showGreeting)
    return;

  // First dots growing into lines
  //
  for(uint8_t i=0; i<8; ++i) {
    matrix.clear();
    matrix.drawLine(15,7-i,15,8+i,LED_ON);
    matrix.drawLine(16,7-i,16,8+i,LED_ON);
    matrix.writeDisplay();
    delay(ANIMATION_DELAY);
  }

  // Greeting and curtains
  //
  for(uint16_t i=1; i<TEXT_VIEW_STEPS+16; ++i) {
    uint8_t offset=i<TEXT_VIEW_STEPS ? 0 : i-TEXT_VIEW_STEPS;

    matrix.clear();
    
    //  matrix.setFont(&FreeSans9pt7b);
    matrix.setTextSize(1);
    matrix.setTextWrap(false);
    matrix.setTextColor(LED_ON);
    matrix.setCursor(8,0-offset);
    matrix.print("OKA");
    matrix.setCursor(0,8-offset);
    matrix.print("ELECTRO");

    if(i<15) {
      matrix.fillRect(0,     0,15-i,16,LED_OFF);
      matrix.fillRect(16+i+1,0,15-i,16,LED_OFF);
    }
    
    if(i<16) {
      matrix.drawLine(15-i,0,15-i,15,LED_ON);
      matrix.drawLine(16+i,0,16+i,15,LED_ON);
    }
 
    matrix.writeDisplay();

    delay(ANIMATION_DELAY);
  }
}
