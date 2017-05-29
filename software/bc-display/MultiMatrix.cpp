#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include "MultiMatrix.h"

MultiMatrix::MultiMatrix(uint8_t n, uint8_t *a) : Adafruit_GFX(16,n*8) {
  mxnum=n;

  memcpy(addr,a,mxnum * sizeof(*addr));
  
  mx=new Adafruit_8x16matrix[mxnum];
  
  dirty=0;
}

void MultiMatrix::setup() {
  for(uint8_t i=0; i<mxnum; ++i) {
    mx[i].begin(addr[i]);

    // Portrait orientation on individual boards - just easier
    // to translate coordinates into that.
    //
    mx[i].setRotation(0);
  }
}

void MultiMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
  int16_t width=mxnum * 8;
  
  // Rotating the pixel
  //
  int16_t xx;
  int16_t yy;
  
  switch (getRotation()) {
    case 0:         // Portrait, connector Left
      xx=width-y-1;
      yy=x;
      break;
      
    case 1:         // Landscape, connector Up, "natural" view
      xx=x;
      yy=y;
      break;

    case 2:         // Portrait, connector Right
      xx=y;
      yy=16-x-1;
      break;
      
    case 3:         // Landscape, connector Down
      xx=width-x-1;
      yy=16-y-1;
      break;
  }

  // Serial.print(getRotation());Serial.print(": ");Serial.print(x);Serial.print(":");Serial.print(y);Serial.print(" -> ");Serial.print(xx);Serial.print(":");Serial.println(yy);

  if(xx < 0 || xx >= width) return;
  if(yy < 0 || yy >= 16) return;

  int16_t n=xx>>3;

  if(n<0 || n>=mxnum) return;

  mx[n].drawPixel(xx & 0x07,yy,color);

  markDirty(n);
}

void MultiMatrix::clear() {
  for(uint8_t i=0; i<mxnum; ++i) {
    mx[i].clear();
    markDirty(i);
  }
}

void MultiMatrix::setBrightness(uint8_t brightness) {
  for(uint8_t i=0; i<mxnum; ++i) {
    mx[i].setBrightness(brightness);
  }
}

void MultiMatrix::writeDisplay() {
  uint8_t bit=1;
  
  for(uint8_t i=0; i<mxnum; ++i, bit<<=1) {
    if(dirty & bit)
      mx[i].writeDisplay();
  }
  
  dirty=0;
}

void MultiMatrix::markDirty(uint8_t i) {
  dirty|=1<<i;
}
