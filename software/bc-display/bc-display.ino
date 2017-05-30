#include <Adafruit_GFX.h>
//#include <gfxfont.h>
//#include <Fonts/FreeSans9pt7b.h>

#include "Config.h"
#include "Options.h"
#include "MultiMatrix.h"
#include "Voltage.h"

bool debug=true;

// TODO: Move to options
//
bool optshowintro=true;

// My matrices are wired with 0x70 on the right in the normal
// connector-top orientation. Listing them backwards here.
//
MultiMatrix matrix = MultiMatrix(4,(uint8_t[]){0x73,0x72,0x71,0x70});

// Voltage meters
//
VoltageBoard vboard;
// VoltageDemo  vdemo;

// Options switches
//
Options opts;

void setup() {
  if(debug) Serial.begin(9600);

  // LED matrix setup
  matrix.setup();
  matrix.setBrightness(3);
  matrix.setRotation(1);

  // Voltage reading setup
  //
  max11632_setup(PIN_CHIP_SELECT_0,PIN_END_OF_CONVERSION_0);
  max11632_setup(PIN_CHIP_SELECT_1,PIN_END_OF_CONVERSION_1);

  // Options reader
  //
  opts.setup();
}

void loop() {
  opts.update();

  // Screen rotation
  //
  matrix.setRotation(opts.rotateScreen ? 3 : 1);

  // Flash screen greeting
  //
  if(opts.showGreeting && optshowintro)
    intro();

  // calibrate();

  vboard.measureAll();

  Serial.print("Tot: "); Serial.print(vboard.getTotal()); Serial.print(" - ");
  
  for(uint8_t c=0; c<31; c++) {
    float v=vboard.getLine(c);
    if(v) {
      Serial.print(c); Serial.print("="); Serial.print(v);Serial.print(" ");
    }
  }

  Serial.println("");
  
  delay(5000);
}

void intro() {
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
  
  matrix.clear();
  matrix.writeDisplay();
 
//  optshowintro=false;
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

// Voltage calibration output. I connected all 32 inputs to the same 5V source and read values
// as divider by 1.5K/45K dividers. Each and every read is identical, so no calibration is
// actually needed.
//
// The value read is 0.153 consistently over about a hundred runs at room temperature.
//
// The theoretical value should have been 0.154 = 4.98 * 1.5 / (1.5 + 47), but consistency
// is more important than the actual value, so letting it be. Besides, it's entirely possible
// that the input value is not actually 4.98 as measured by my multimeter.
//
static void calibrate() {
  for(uint8_t c=0; c<31; c++) {
    int pin_cs=c<16 ? PIN_CHIP_SELECT_0 : PIN_CHIP_SELECT_1;
    int pin_ec=c<16 ? PIN_END_OF_CONVERSION_0 : PIN_END_OF_CONVERSION_1;
    int16_t voltage=max11632_read(c & 0x0f, pin_cs, pin_ec, REFERENCE_VOLTAGE);    
    Serial.print(c,HEX); Serial.print(":"); Serial.print(voltage);Serial.print(" ");
  }

  Serial.println("");
}
