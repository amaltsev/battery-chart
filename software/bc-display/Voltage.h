#ifndef Oka_Voltage_h
#define Oka_Voltage_h

const uint8_t VCHANNELS = 32;

// Reading voltage for all channels.
//
class Voltage {
  public:
    Voltage();
    
    // Measure and remember all channels.
    //
    virtual void measureAll()=0;

    // Calculated voltage for any given line, in volts.
    //
    virtual float getLine(uint8_t c)=0;

    // Grand total voltage across all lines, in volts.
    //
    virtual float getTotal()=0;

  protected:
    int16_t input[VCHANNELS];
    float volts[VCHANNELS];
    float total;
};

// Demo (randomized) voltage readings
//
class VoltageDemo : public Voltage {
  
};

// Real readings from MAX-11632 chips and resistor dividers.
//
class VoltageBoard : public Voltage {
  public:
    void measureAll();
    float getLine(uint8_t c);
    float getTotal();
};

void max11632_setup(int pin_chip_select, int pin_eoc);
int16_t max11632_read(uint8_t channel, int pin_chip_select, int pin_eoc, uint16_t refv);

#endif // Oka_Voltage_h
