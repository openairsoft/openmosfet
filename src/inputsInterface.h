#ifndef INPUTS_INTERFACE_H
#define INPUTS_INTERFACE_H

#include <Arduino.h>
#include "components.h"

class OMInputsInterface
{
  public:
  
    static void begin(OMVirtualReplica &replica);
    static void update(OMVirtualReplica &replica);
    static OMFiringSettings &getCurrentFiringSetting(OMVirtualSelector &selector);
    static void motorOn();
    static void motorOff();
};

#endif
