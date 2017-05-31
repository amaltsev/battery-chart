#include "Config.h"
#include "Voltage.h"

const int16_t ZERO_THRESHOLD = 1;

// Voltage calculation.
//
// We divide the input voltage with 1.5K and 47K resistors, translating
// back into original voltage. Inputs are in millivolts, output is
// in volts.
//
static float calculateVolts(float mv, float mvbase) {
  return (mv-mvbase) * (RESISTOR_UPPER + RESISTOR_LOWER) / RESISTOR_LOWER / 1000;
}

//// Initialization
////
//VoltageBoard::Voltage() {
//  memset(input,0,sizeof(input));
//  memset(volts,0,sizeof(volts));
//  total=0;
//}

// Voltage reading setup
//
void VoltageBoard::setup() {  
  max11632_setup(PIN_CHIP_SELECT_0,PIN_END_OF_CONVERSION_0);
  max11632_setup(PIN_CHIP_SELECT_1,PIN_END_OF_CONVERSION_1);
}

// Reading and analyzing all channels
//
void VoltageBoard::measureAll() {
  
  // MAX-11632 allows to measure and receive multiple channels in one go,
  // but since we're not really strapped for time or energy it is easier
  // to just request each individually.
  //
  for(uint8_t c=0; c<VCHANNELS; c++) {
    int pin_cs=c<16 ? PIN_CHIP_SELECT_0 : PIN_CHIP_SELECT_1;
    int pin_ec=c<16 ? PIN_END_OF_CONVERSION_0 : PIN_END_OF_CONVERSION_1;
    
    int16_t voltage=max11632_read(c & 0x0f, pin_cs, pin_ec, REFERENCE_VOLTAGE);
    
    // Serial.print(c,HEX); Serial.print(":"); Serial.print(voltage);Serial.print(" ");

    // Negative value indicates that no reading was made.
    // How to indicate an error? For now just
    // leaving the previous value untouched.
    //
    // TODO: Count errors and write -1 after some count?
    //
    if(voltage<0)
      voltage=input[c];

    // Some inputs may be unconnected and will read zero (because they have
    // 1.5K resistors to the ground). Just in case there is some noise, value
    // below threshold are also treated as zeros.
    //
    if(voltage<=ZERO_THRESHOLD)
      voltage=0;
  
    if(DEBUG_VOLTAGE && voltage>0)
      Serial << c << "=" << voltage << " ";

    input[c]=voltage;
  }

  if(DEBUG_VOLTAGE) Serial.println("");

  // Our input voltages are measured from serially connected battery banks, so
  // each input is theoretically larger then the previous.
  //
  // Allowing for inputs to be connected in any order, we just look up the
  // next larger value each time.
  //
  uint8_t lastChannel=VCHANNELS;
  int16_t lastInput=0;
  uint8_t active=0;
  float vmin=-1;
  float vmax=-1;
  while(true) {

    // Finding the next input value that is larger then the given
    //
    uint8_t channel=VCHANNELS;
    for(uint8_t c=0; c<VCHANNELS; c++) {
      if(input[c]>lastInput) {
        channel=c;
        break;
      }
    }

    if(channel>=VCHANNELS)
      break;

    float v=volts[channel]=calculateVolts(input[channel],lastInput);
    
    lastChannel=channel;
    lastInput=input[channel];

    ++active;

    if(v>2.8 && v<4.4) {
      if(vmin<0 || vmin>v) vmin=v;
      if(vmax<0 || vmax<v) vmax=v;
    }
  }

  // The total is the maximum value we found
  //
  if(active)
    total=calculateVolts(lastInput,0);
  else
    total=0;

  // Some inputs might get disconnected even though they do correspond to a battery bank,
  // meaning the next measurement would actually be 2+ steps away from the previous,
  // not just one. Finding and recalculating these.
  //
  if(active) {
    float avg=vmin>0 ? (vmin+vmax)/2 : 4.2;

    // LiON voltages are in 3...4.3V range, so anything over 5 is
    // a sure indicator of a doubled up measurement.
    //
    for(uint8_t c=0; c<VCHANNELS; c++) {
      while(volts[c]>5) {
        volts[c]-=avg;
        ++active;
      }
    }
  }

  // Stats for charting
  //
  nActive=active;
  cFirst=0;
  while(cFirst<VCHANNELS && volts[cFirst]==0) ++cFirst;
  cLast=VCHANNELS-1;
  while(cLast>0 && volts[cLast]==0) --cLast;
}
