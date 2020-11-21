#include <Arduino.h>

#include "utilities.h"
#include "config.h"
#include "inputsInterface.h"
#include "components.h"
#include "configuration.h"
#include "wifiServer.h"
#include "otaUploader.h"

OMVirtualReplica replica;

void setup();
void loop();

void setup() {
  Serial.begin(115200);

  OMInputsInterface::begin(replica);
  
  if (!FILESYSTEM.begin()) {
    #ifdef DEBUGM
      Serial.println("Failed to mount file system, trying to format...");
    #endif
    if (!FILESYSTEM.format()) {
      #ifdef DEBUG
        Serial.println("Unable to format(), aborting...");
      #endif   
      return;
    }else{
      if (!FILESYSTEM.begin()) {
      #ifdef DEBUG
        Serial.println("Failed to mount file system, evn after formating...");
      #endif
      }
    }
  }
  
  #ifdef DEBUG
    Serial.print("load status: ");
    Serial.println(OMConfiguration::load());
    Serial.println("config chargée :");
    OMConfiguration::printCfg();
  #else
    OMConfiguration::load();
  #endif


  OMwifiserver::begin();
  OMOtaUploader::begin();
}

void loop() {
  OMwifiserver::update();
  OMOtaUploader::update();
  OMInputsInterface::update(replica);
  replica.update();
}
