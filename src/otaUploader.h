#ifndef OTA_UPLOADER_H
#define OTA_UPLOADER_H

#include <Arduino.h>
#include "components.h"

class OMOtaUploader
{
  public:
  
    static void begin();
    static void update();
};

#endif