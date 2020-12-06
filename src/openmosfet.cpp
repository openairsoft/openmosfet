#include <Arduino.h>

#include "utilities.h"
#include "config.h"
#include "inputsInterface.h"
#include "components.h"
#include "configuration.h"
#include "wifiServer.h"
#include "otaUploader.h"

void setup();
void loop();

void setup() {
  //important initialize the input interface first, beacause this the sets the outputs and it may be important depending on the fet input logic
  OMInputsInterface::begin();
  OMVirtualReplica::begin();

  Serial.begin(115200);

  
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
  
 
  OMConfiguration::load();
  #ifdef DEBUG
    Serial.println("config chargée :");
    OMConfiguration::printCfg();
  #endif

  #ifdef OM_ADDITIONAL_HIGH_PIN
    pinMode (OM_ADDITIONAL_HIGH_PIN, OUTPUT);
    digitalWrite(OM_ADDITIONAL_HIGH_PIN, HIGH);
  #endif

  #ifdef OM_ADDITIONAL_LOW_PIN
    pinMode (OM_ADDITIONAL_LOW_PIN, OUTPUT);
    digitalWrite(OM_ADDITIONAL_LOW_PIN, LOW);
  #endif

  OMwifiserver::begin();
  OMOtaUploader::begin();
}

void loop() {
  OMwifiserver::update();
  OMOtaUploader::update();
  OMInputsInterface::update();
  OMVirtualReplica::update();
}
