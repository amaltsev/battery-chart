#include <Arduino.h>
#include "Config.h"
#include "Options.h"

void Options::setup() {
  pinMode(PIN_OPT_1_DEMO_MODE   ,INPUT_PULLUP);
  pinMode(PIN_OPT_2_GREETING    ,INPUT_PULLUP);
  pinMode(PIN_OPT_3_FILL_CHART  ,INPUT_PULLUP);
  pinMode(PIN_OPT_4_ROTATE      ,INPUT_PULLUP);
  pinMode(PIN_OPT_5_DRAW_BOTTOM ,INPUT_PULLUP);
  pinMode(PIN_OPT_6_BLINK_LOW   ,INPUT_PULLUP);
  pinMode(PIN_OPT_7_ENABLE_BRT  ,INPUT_PULLUP);
  pinMode(PIN_OPT_8             ,INPUT_PULLUP);
}

void Options::update() {
  demoMode=     digitalRead(PIN_OPT_1_DEMO_MODE)==LOW;
  showGreeting= digitalRead(PIN_OPT_2_GREETING)==LOW;
  fillChart=    digitalRead(PIN_OPT_3_FILL_CHART)==LOW;
  rotateScreen= digitalRead(PIN_OPT_4_ROTATE)==LOW;
  drawBottom=   digitalRead(PIN_OPT_5_DRAW_BOTTOM)==LOW;
  blinkLow=     digitalRead(PIN_OPT_6_BLINK_LOW)==LOW;
  enableBright= digitalRead(PIN_OPT_7_ENABLE_BRT)==LOW;

  if(DEBUG_OPTIONS) {
    Serial.print("Options: ");
    Serial.print("1.demo=");  Serial.print(demoMode);     Serial.print(" ");
    Serial.print("2.greet="); Serial.print(showGreeting); Serial.print(" ");
    Serial.print("3.fill=");  Serial.print(fillChart);    Serial.print(" ");
    Serial.print("4.rot=");   Serial.print(rotateScreen); Serial.print(" ");
    Serial.print("5.btm=");   Serial.print(drawBottom);   Serial.print(" ");
    Serial.print("6.blink="); Serial.print(blinkLow);     Serial.print(" ");
    Serial.println("");
  }
}

