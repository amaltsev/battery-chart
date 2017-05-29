// Reading voltage from MAX11632 12-bit ADC
//
///////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include "Voltage.h"

const uint8_t REG_SEL_RESET        = 0b00010000;   // Reset register
const uint8_t REG_RESET_DEFAULTS   = 0b00000000;   // Power-on defaults

const uint8_t REG_SEL_SETUP        = 0b01000000;   // Setup register
const uint8_t REG_SETUP_CKSEL_INT  = (0b10 << 4);  // Internal timer, internal timing
const uint8_t REG_SETUP_REFSEL_INT = (0b10 << 2);  // Internal reference, always on
const uint8_t REG_SETUP_REFSEL_EXT = (0b01 << 2);  // External reference

const uint8_t REG_SEL_AVG          = 0b00100000;   // Averaging register
const uint8_t REG_AVG_AVGON        = (0b1 << 4);   // Averaging ON
const uint8_t REG_AVG_NAVG_4       = (0b00 << 2);  // Average 4 samples
const uint8_t REG_AVG_NAVG_8       = (0b01 << 2);  // Average 4 samples
const uint8_t REG_AVG_NAVG_16      = (0b10 << 2);  // Average 4 samples
const uint8_t REG_AVG_NAVG_32      = (0b11 << 2);  // Average 4 samples
const uint8_t REG_AVG_NSCAN_4      = (0b00);       // Return 4 results (in scan mode)
const uint8_t REG_AVG_NSCAN_8      = (0b01);       // Return 8 results
const uint8_t REG_AVG_NSCAN_12     = (0b10);       // Return 12 results
const uint8_t REG_AVG_NSCAN_16     = (0b11);       // Return 16 results

const uint8_t REG_SEL_CONV         = 0b10000000;   // Conversion register
const uint8_t REG_CONV_CH_SHIFT    = 3;            // Bit-shift for channel number N
const uint8_t REG_CONV_SCAN_MIN_N  = (0b00 << 1);  // Scan channels 0 through N
const uint8_t REG_CONV_SCAN_N_MAX  = (0b01 << 1);  // Scan channels N through MAX
const uint8_t REG_CONV_SCAN_REPEAT = (0b10 << 1);  // Scan channel N repeatedly NSCAN times
const uint8_t REG_CONV_SCAN_ONCE   = (0b11 << 1);  // Convert channel N once, no repeats

/////////////////////////////////////////////////////////////////////////

// Initialization
//
void max11632_setup(int pin_chip_select, int pin_eoc) {

  // End-of-conversion pin
  //
  pinMode(pin_eoc, INPUT);

  // Chip select pin
  //
  pinMode(pin_chip_select, OUTPUT);

  // High means not selected
  //
  digitalWrite(pin_chip_select, HIGH);

  SPI.begin();
}

// Read with optionally a debugging output
//
static uint8_t xfer(char const *label, uint8_t value) {
  uint8_t result=SPI.transfer(value);

  /// Serial.print("SPI: "); Serial.print(label); Serial.print(" "); Serial.print(value,BIN); Serial.print(" -> "); Serial.println(result,BIN);

  return result;
}

// Read voltage in milli-volts. Will return -1 if no value could be
// read. Actually read values are always positive.
//
int16_t max11632_read(uint8_t channel, int pin_chip_select, int pin_eoc, uint16_t refv) {

  // https://www.arduino.cc/en/Reference/SPI
  //
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

  digitalWrite(pin_chip_select, LOW);  // High means not selected

  uint8_t rcv;

  rcv=xfer("Reset",
        REG_SEL_RESET |         // Reset register
        REG_RESET_DEFAULTS);    // Power-on defaults

  rcv=xfer("Setup",
        REG_SEL_SETUP |         // Setup register
        REG_SETUP_CKSEL_INT |   // Internal timer, internal timing, all 16 channels available
        REG_SETUP_REFSEL_EXT);  // Internal reference always on

  rcv=xfer("Averg",
        REG_SEL_AVG |           // Averaging register
        REG_AVG_AVGON |         // Averaging ON
        REG_AVG_NAVG_32 |       // Average X samples
        REG_AVG_NSCAN_4);       // Return X results (if in repeat scan mode)

  rcv=xfer("Convr",
        REG_SEL_CONV |          // Conversion register (starts conversion)
        REG_CONV_SCAN_ONCE |    // Scan the given channel once
        (channel << REG_CONV_CH_SHIFT));

  int16_t voltage=-1;

  for(int i=0; i<30; ++i) {
    delay(1);

    uint8_t eoc=digitalRead(pin_eoc);

    if(eoc) {
      uint8_t hb=xfer("Input",0);
      uint8_t lb=xfer("Input",0);

      // Our reference voltage is 4.096 V, and the ADC is 12 bit (0...4095),
      // so there is no need to recalculate anything. There is a 1/4096 of
      // a volt inacurracy in that, but that's OK because it's stable
      // for all measurements.
      //
      if(refv == 4095) {
        voltage=(hb*256+lb);
      }
      else {
        float measured=(hb*256+lb);
        float fvolt=measured * refv / (float)0b111111111111;
        voltage=(uint16_t)(fvolt+0.5f);
      }

      break;
    }
  }

  digitalWrite(pin_chip_select, HIGH);  // High means not selected

  SPI.endTransaction();

  return voltage;
}
