#ifndef Oka_Options_h
#define Oka_Options_h

class Options {
  public:
    void setup();
    void update();
    
    // Switch 1=ON: Run in demo mode, not reading any actual data.
    //
    bool demoMode=false;

    // Switch 2=ON: Skip flash screen.
    //
    bool showGreeting=false;

    // Switch 3=ON: Fill chart lines (default is to leave them open).
    //
    bool fillChart=false;

    // Draw a pixel at the bottom? Might help in the dark
    // when there is no fill chart.
    //
    bool drawBottom=false;

    // Switch 4=ON: Rotate screen connector down
    //
    bool rotateScreen=false;

    // Blink when low
    //
    bool blinkLow=false;

    // Enable brightness control
    //
    bool enableBright=false;
};

#endif // Oka_Options_h
