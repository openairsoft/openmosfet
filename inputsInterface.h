#ifndef INPUTS_INTERFACE_H
#define INPUTS_INTERFACE_H

#include "components.h"

class OMInputsInterface
{
  public:
  
    static void begin(AAMVirtualReplica &replica);
    static void update(AAMVirtualReplica &replica);
    static AAMFiringSettings &getCurrentFiringSetting(AAMVirtualSelector &selector);
    static void motorOn();
    static void motorOff();
};

#endif
