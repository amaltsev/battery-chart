#ifndef Oka_Voltage_h
#define Oka_Voltage_h

const uint8_t VCHANNELS = 32;

// Reading voltage for all channels.
//
class Voltage {
  public:
    Voltage() {
      memset(volts,0,sizeof(volts));
    };

    // Initial setup
    //
    virtual void setup()=0;
    
    // Measure and remember all channels.
    //
    virtual void measureAll()=0;

    // Calculated voltage for any given line, in volts.
    //
    float getLine(uint8_t c) {
      return c<VCHANNELS ? volts[c] : -1;
    }

    // Grand total voltage across all lines, in volts.
    //
    float getTotal() {
      return total;
    }

    // Get the index of the first active channel number.
    //
    uint8_t getChannelFirst() {
      return cFirst;
    }

    // Get the index of the last active channel number.
    //
    uint8_t getChannelLast() {
      return cLast;
    }

    // Get the count of active channels (how many banks we're actually
    // measuring, even if there are gaps and disconnects).
    //
    uint8_t getActiveCount() {
      return nActive;
    }

  protected:
    float volts[VCHANNELS];
    float total=0;
    uint8_t cFirst=VCHANNELS;
    uint8_t cLast=VCHANNELS;
    uint8_t nActive=0;
};

// Demo (randomized) voltage readings
//
class VoltageDemo : public Voltage {
  public:
    virtual void setup();
    virtual void measureAll();
};

// Real readings from MAX-11632 chips and resistor dividers.
//
class VoltageBoard : public Voltage {
  public:
    VoltageBoard() {
      memset(volts,0,sizeof(volts));
    };

    virtual void setup();
    virtual void measureAll();
  private:
    int16_t input[VCHANNELS];
};

void max11632_setup(int pin_chip_select, int pin_eoc);
int16_t max11632_read(uint8_t channel, int pin_chip_select, int pin_eoc, uint16_t refv);

#endif // Oka_Voltage_h
