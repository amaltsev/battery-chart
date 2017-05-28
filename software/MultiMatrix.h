#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

// Multiplexor for displaying arbritrary graphics on multiple
// side-by-side 8x16 LED matrices.
//
class MultiMatrix : public Adafruit_GFX {
  public:

    // Initial setup, takes the number of 8x16
    // matrices (at most 8) and an array of their addresses.
    //
    MultiMatrix(uint8_t n, uint8_t *a);

    // Initialize hardware, prepare for drawing.
    //
    void setup();

    // Clear board, all dark.
    //
    void clear();

    // Set brightness (0...15).
    //
    void setBrightness(uint8_t brightness);

    // Reflect changes from display buffers to matrices.
    //
    void writeDisplay();

    // Called by GFX.
    //
    void drawPixel(int16_t x, int16_t y, uint16_t color);

  private:
    uint8_t addr[8];
    uint8_t mxnum;
    Adafruit_8x16matrix *mx;
    uint8_t dirty;

    void markDirty(uint8_t n);
};
