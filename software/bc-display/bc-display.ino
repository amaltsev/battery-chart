#include "Config.h"
#include "Options.h"
#include "Display.h"
#include "Voltage.h"

// Globally visible debug flag.
//
bool debug=true;

// Voltage meters
//
VoltageBoard vboard;
VoltageDemo  vdemo;

// Options switches
//
Options opts;

// Display chart
//
Display display;

void setup() {
  if(debug)
    Serial.begin(9600);

  // Randomizing with noise from an unconnected pin
  //
  randomSeed(analogRead(A6));

  display.setup();

  vboard.setup();
  
  vdemo.setup();

  opts.setup();
}

void loop() {
  opts.update();

  // calibrate();

  Voltage &vdata=opts.demoMode ? (Voltage&)vdemo : (Voltage&)vboard;

  vdata.measureAll();

  vdata.dump();

  display.loop(vdata);
  
  delay(500);
}

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
