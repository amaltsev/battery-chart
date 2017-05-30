#include "Config.h"
#include "Voltage.h"

static const uint8_t DEMOCHAN=30;
static const uint8_t MINPHASE=3;

// Initial setup
//
void VoltageDemo::setup() {
  phase=MINPHASE;
}
    
// Generate test values
//
void VoltageDemo::measureAll() {
//  if(debug) {
//    Serial << "--- Phase=" << phase << " step=" << step << "\n";
//  }

  // Generating values from Min to Max in a ladder pattern
  //
  if(phase==0) {
    volts[step]=CHART_VOLT_MIN + step * (CHART_VOLT_MAX - CHART_VOLT_MIN)/(DEMOCHAN-1);
    
    nextStep(DEMOCHAN);
  }

  // Full charge
  //
  else if(phase==1) {
    volts[step]=CHART_VOLT_MAX;
    
    nextStep(DEMOCHAN);
  }

  // Gaps
  //
  else if(phase==2) {
    if(step==0)
      volts[0]=0;
    else
      volts[random(0,DEMOCHAN)]=0;
      
    nextStep(10);
  }

  // Gap fill
  //
  else if(phase==3) {
    bool haveWork=false;
    
    for(uint8_t i=0; i<DEMOCHAN; ++i) {
      if(volts[i]<CHART_VOLT_MAX) {
        if(volts[i]==0)
          volts[i]=CHART_VOLT_MIN - 0.1;
        else
          volts[i]+=random(20,100)/1000.0;
          
        haveWork=true;
      }
    }
    
    if(!haveWork)
      phase=4;
  }

  // Random drop to below min
  //
  else {
    bool haveWork=false;

    for(uint8_t i=0; i<DEMOCHAN; ++i) {
      if(volts[i]>CHART_VOLT_MAX/2) {
        volts[i]-=random(3,75)/1000.0;
        haveWork=true;
      }
    }

    if(!haveWork) {
      phase=MINPHASE;
      step=0;
    }
  }

  // Got the data, analyzing
  //
  nActive=0;
  total=0;
  for(uint8_t i=0; i<VCHANNELS; ++i) {
    if(volts[i]) {
      total+=volts[i];
      ++nActive;
    }
  }
  
  cFirst=0;
  while(cFirst<VCHANNELS && volts[cFirst]==0) ++cFirst;
  
  cLast=VCHANNELS-1;
  while(cLast>0 && volts[cLast]==0) --cLast;
}

void VoltageDemo::nextStep(int16_t maxStep) {
  if(++step >= maxStep) {
    ++phase;
    step=0;
  }
}
