#include "Config.h"
#include "Display.h"

static const int ANIMATION_DELAY=20;
static const int TEXT_VIEW_DELAY=3000;

static uint8_t pxWarn1;
static uint8_t pxWarn2;

// Voltage into chart pixel Y coordinate with a hysteresis depending on the
// given current value of that column.
//
static uint8_t pixelCalculate(float volts, float vmin, float vmax, uint8_t pixcur) {
  uint8_t pix;
  uint8_t pixnormal;
  
  if(volts < vmin)
    pix=0;
  else if(volts > vmax)
    pix=15;
  else {
    float t=(volts-vmin)*15/(vmax-vmin);
    
    pix=pixnormal=(uint8_t)(t + 0.5);

    // Hysteresis. If the new pixel value is larger, then the voltage value
    // has to be at least .75 of a step higher.
    //
    if(pix>pixcur)
      pix=(uint8_t)(t + 0.5 - HYSTERESIS_HSTEP);
      
    else if(pix<pixcur)
      pix=(uint8_t)(t + 0.5 + HYSTERESIS_HSTEP);
  
      //    if(DEBUG_DISPLAY && pix!=pixnormal)
      //      Serial << "-- V2P(" << volts << "," << vmin << "," << vmax << "," << pixcur << ")=" << pix << " / " << pixnormal << " t=" << t << "\n";
  }
  
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

  // Initial pixel values
  //
  memset(pixelValues,0,sizeof(pixelValues));

  // Voltage warnings recalculated into pixels
  //
  pxWarn1=pixelCalculate(CHART_VOLT_WARN1,CHART_VOLT_MIN,CHART_VOLT_MAX,15);
  pxWarn2=pixelCalculate(CHART_VOLT_WARN2,CHART_VOLT_MIN,CHART_VOLT_MAX,15);
}

// Loop (both chart and intro)
//
void Display::loop(Voltage &vdata) {

  // Rotation
  //
  matrix.setRotation(opts.rotateScreen ? 3 : 1);

  // Brightness
  //
  if(opts.enableBright) {
    int rbr=analogRead(PIN_BRIGHTNESS);   // about 0..850

    if(brindex<0) {
      for(uint8_t i=0; i<BRIGHTNESS_SAMPLES; ++i)
        brsamples[i]=rbr;
      brindex=0;
    }
    else {
      brsamples[brindex]=rbr;
      ++brindex;
      if(brindex>=BRIGHTNESS_SAMPLES)
        brindex=0;
    }

    // Averaging all samples
    //
    uint32_t tbr=0;
    for(uint8_t i=0; i<BRIGHTNESS_SAMPLES; ++i)
      tbr+=brsamples[i];

    int abr=tbr/BRIGHTNESS_SAMPLES;
    
    if(abr<200)
      brightness=0;
    else if(abr<300)
      brightness=1;
    else if(abr<400)
      brightness=2;
    else if(abr<600)
      brightness=3;
    else
      brightness=4;
    
    if(DEBUG_DISPLAY)
      Serial << "Brightness: " << rbr << " -> " << abr << " -> " << brightness << "\n";
  }
  else {
    brightness=2;
  }
  
  matrix.setBrightness(brightness);

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
    uint8_t vpix=pixelCalculate(volts,CHART_VOLT_MIN,CHART_VOLT_MAX,pixelValues[i]);

    drawColumn(i,vpix);

    pixelValues[i]=vpix;

    if(vpix<=pxWarn1) w1Line=true;
    if(vpix<=pxWarn2) w2Line=true;
  }

  // If the rightmost columns are empty (they normally are) -- showing
  // the total in their place.
  //
  if(vdata.getChannelLast() < VCHANNELS-2 && vdata.getActiveCount()>0) {
    uint8_t active=vdata.getActiveCount();

    uint8_t vpix=pixelCalculate(vdata.getTotal(),CHART_VOLT_MIN * active, CHART_VOLT_MAX * active, pixelValues[VCHANNELS-1]);

    drawColumn(VCHANNELS-1,vpix);

    pixelValues[VCHANNELS-1]=vpix;

    if(vpix<=pxWarn1) w1Total=true;
    if(vpix<=pxWarn2) w2Total=true;
  }

  // If low voltage showing a blinking indicator of that. One
  // in the total column and one to the right of the line columns.
  //
  if(opts.blinkLow && pixelCap>15) {
    if(w1Line || w2Line) {
      uint8_t c=vdata.getChannelLast()+2;
      if(c>=VCHANNELS-2) c-=2;
      drawWarning(c,w1Line,w2Line);
    }

    if(w1Total || w2Total)
      drawWarning(VCHANNELS-1,w1Total,w2Total);
  }

  matrix.writeDisplay();
}

void Display::drawColumn(uint8_t column, uint8_t value) {
  if(value>pixelCap)
    value=pixelCap;

  if(opts.fillChart && brightness>0) {
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

// Converted with http://javl.github.io/image2cpp/
//
const unsigned char grt32x16 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x01, 0xe6, 0x63, 0xc0, 0x03, 0x36, 0x66, 0xc0, 0x03, 0x36, 0xcc, 0xc0,
	0x03, 0x37, 0x8c, 0xc0, 0x03, 0x36, 0xcf, 0xc0, 0x03, 0x36, 0x6c, 0xc0, 0x01, 0xe6, 0x6c, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x1d, 0x3b, 0xbb, 0xb8, 0x11, 0x22, 0x12, 0xa8, 0x1d, 0x3a, 0x13, 0x28,
	0x11, 0x22, 0x12, 0xa8, 0x11, 0x22, 0x12, 0xa8, 0x1d, 0xbb, 0x92, 0xb8, 0x00, 0x00, 0x00, 0x00,
};

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
  for(uint16_t i=1; i<=16; ++i) {
    matrix.clear();

    // OKA ELECTRO bitmap
    //
    matrix.drawBitmap(0,0,grt32x16,32,16,LED_ON);

    // Curtains
    //
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

  delay(TEXT_VIEW_DELAY);

  // Slide off
  //
  for(uint16_t i=1; i<16; ++i) {
    matrix.clear();

    // OKA ELECTRO bitmap
    //
    matrix.drawBitmap(0,0-i,grt32x16,32,16,LED_ON);

    matrix.writeDisplay();

    delay(ANIMATION_DELAY);
  }
}
