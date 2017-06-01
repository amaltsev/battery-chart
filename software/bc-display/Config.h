#ifndef Oka_Config_h
#define Oka_Config_h

#include <Arduino.h>
#include "Options.h"

const int REFERENCE_VOLTAGE       = 4095;   // It's actually 4.096, but 4095 allows to avoid calculations.

const float RESISTOR_UPPER        = 47;     // Divider resistor to the input
const float RESISTOR_LOWER        = 1.5;    // Divider resistor to ground

const float CHART_VOLT_MIN        = 2.8;    // Minimum LiON voltage
const float CHART_VOLT_MAX        = 4.3;    // Maximum LiON voltage (also min + 15 x 0.1v)
const float CHART_VOLT_WARN1      = 3.2;    // Almost dead
const float CHART_VOLT_WARN2      = 3.0;    // Dead, time to turn off

const int UPDATE_INTERVAL_MS      = 500;    // Loop update interval

const int PIN_CHIP_SELECT_0       = 10;     // MAX-11632 #0 select
const int PIN_END_OF_CONVERSION_0 = A0;     // MAX-11632 #0 EOC

const int PIN_CHIP_SELECT_1       = 9;      // MAX-11632 #1 select
const int PIN_END_OF_CONVERSION_1 = A1;     // MAX-11632 #1 EOC

const int PIN_OPT_1_DEMO_MODE     = 2;
const int PIN_OPT_2_GREETING      = 3;
const int PIN_OPT_3_FILL_CHART    = 4;
const int PIN_OPT_4_ROTATE        = 5;
const int PIN_OPT_5_DRAW_BOTTOM   = 6;
const int PIN_OPT_6_BLINK_LOW     = 7;
const int PIN_OPT_7               = 8;
const int PIN_OPT_8               = A7;

// My matrices are wired with 0x70 on the right in the normal
// connector-top orientation. And the LED driver library assumes
// the connector is normally zero coordinate. Listing them backwards
// here.
//
const uint8_t LED_MATRIX_IDS[4]   = {0x73,0x72,0x71,0x70};

// Voltage channels (hardware bound)
//
const uint8_t VCHANNELS           = 32;

// Display hysteresis half-step
//
const float HYSTERESIS_HSTEP      = 0.3;

// Debug options (Serial output)
//
const bool DEBUG_ANY              = false;
const bool DEBUG_VOLTAGE          = true && DEBUG_ANY;
const bool DEBUG_OPTIONS          = false && DEBUG_ANY;
const bool DEBUG_DISPLAY          = false && DEBUG_ANY;

// DIP Switch options
//
extern Options opts;

// A brilliant streaming serial output hack from:
// https://playground.arduino.cc/Main/StreamingOutput
//
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 

#endif // Oka_Config_h
