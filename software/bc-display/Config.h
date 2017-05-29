#include <Arduino.h>

#ifndef Oka_Config_h
#define Oka_Config_h

const int REFERENCE_VOLTAGE       = 4095;   // It's actually 4.096, but 4095 allows to avoid calculations.

const float RESISTOR_UPPER        = 47;     // Divider resistor to the input
const float RESISTOR_LOWER        = 1.5;    // Divider resistor to ground

const int PIN_CHIP_SELECT_0       = 10;     // MAX-11632 #0 select
const int PIN_END_OF_CONVERSION_0 = A0;     // MAX-11632 #0 EOC

const int PIN_CHIP_SELECT_1       = 9;      // MAX-11632 #1 select
const int PIN_END_OF_CONVERSION_1 = A1;     // MAX-11632 #1 EOC

extern bool debug;

#endif // Oka_Config_h
