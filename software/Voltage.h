#ifndef Oka_Voltage_h
#define Oka_Voltage_h

const int REFERENCE_VOLTAGE       = 4095; // It's actually 4.096, but 4095 allows to avoid calculations.

const int PIN_CHIP_SELECT_0       = 10;
const int PIN_END_OF_CONVERSION_0 = A0;

const int PIN_CHIP_SELECT_1       = 9;
const int PIN_END_OF_CONVERSION_1 = A1;

void max11632_setup(int pin_chip_select, int pin_eoc);
int16_t max11632_read(uint8_t channel, int pin_chip_select, int pin_eoc, uint16_t refv);

#endif // Oka_Voltage_h
