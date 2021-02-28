#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include "components.h"
#include "FS.h"

// FFat or SPIFFS
#define FILESYSTEM SPIFFS

// include de corresponding filesystem libraries
#include "SPIFFS.h"
//#include "FFat.h"

#include <ArduinoJson.h>


/** Le nombre magique et le numéro de version actuelle */
#define OM_CFG_VERSION 8 //NOTE: change this value if you modify the data structure ?

/** La structure qui contient les données */
class OMConfiguration {
  public:

    #ifdef DEBUG
      static void printCfg();
    #endif
    
    // NOTE: usefull ressource here: https://arduino.stackexchange.com/questions/60112/value-of-uninitialized-eeprom-in-esp8266
    static void loadFromJson(Stream &stream);
    static boolean load();
    static DynamicJsonDocument toJson();
    static boolean save();

    static boolean isSelectorCalibrated();

    byte versionNumber;
    uint16_t checksum;
  
    static char appSsid[OM_WIFI_SSID_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 1
    static char appPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 1
    static boolean connectToNetworkIfAvailable; // since OM_CFG_VERSION = 3
    static char availableNetworkAppSsid[OM_WIFI_SSID_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 4
    static char availableNetworkAppPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 4
    static boolean enableEspNow; // since OM_CFG_VERSION = 6
    static boolean disableMotor;// since OM_CFG_VERSION = 6
    static boolean enableBatteryProtection; // since OM_CFG_VERSION = 1
    static int wifiShutdownDelayMinutes; // since OM_CFG_VERSION = 2
    static int deepSleepDelayMinutes; // since OM_CFG_VERSION = 2
    static float batteryNominalVoltage; // since OM_CFG_VERSION = 1
    static float batteryLowVoltage; // since OM_CFG_VERSION = 1
    static float batteryShutdownVoltage; // since OM_CFG_VERSION = 1
    static boolean enableActiveBreaking; // since OM_CFG_VERSION = 1
    static float decockAfter_s; // since OM_CFG_VERSION = 5
    static boolean enablePrecocking; // since OM_CFG_VERSION = 5
    static int selectorCalibration[OM_MAX_NB_STORED_MODES+1]; // since OM_CFG_VERSION = 7
    //static float selectorThresholds[OM_MAX_NB_STORED_MODES]; // since OM_CFG_VERSION = 7 //this is a calculated value
    static OMFiringSettings fireModes[OM_MAX_NB_STORED_MODES]; // since OM_CFG_VERSION = 1
    static boolean enableSetupScreen; // since OM_CFG_VERSION = 8
};

#endif
