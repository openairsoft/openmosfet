#ifndef INPUTS_INTERFACE_H
#define INPUTS_INTERFACE_H

#include <Arduino.h>
#include "components.h"

class OMInputsInterface
{
  public:
  
    static void begin();
    static void update();
    static OMFiringSettings &getCurrentFiringSetting();
    static void motorOn();
    static void motorOff();
};

#endif
