#ifndef Oka_Display_h
#define Oka_Display_h

#include "Config.h"
#include "Voltage.h"
#include "MultiMatrix.h"

// The display controller.
//
// It does not have much poing being a class, but easier to code that way.
//
class Display {
  public:
    Display();
    void setup();
    void loop(Voltage &vdata);

  protected:
    void chart(Voltage &vdata);
    void greeting();
    void drawColumn(uint8_t n, uint8_t v);
    
    MultiMatrix matrix=MultiMatrix(4,LED_MATRIX_IDS);
    bool greetingShown=false;
};

#endif // Oka_Display_h
