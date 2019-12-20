#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "components.h"
#include <EEPROM.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

/** Le nombre magique et le numéro de version actuelle */
#define OM_CFG_VERSION 4 //NOTE: change this value if you modify the data structure ?

/** La structure qui contient les données */
class OMConfiguration {
  public:

    // NOTE: usefull ressource here: https://arduino.stackexchange.com/questions/60112/value-of-uninitialized-eeprom-in-esp8266
    static boolean loadFromJson(Stream &stream);
    static boolean load(void);
    static boolean save(void);

    #ifdef DEBUG
      static void printCfg(void);
    #endif

    byte versionNumber;
    uint16_t checksum;
  
    static AAMFiringSettings fireModes[OM_MAX_NB_STORED_MODES]; // since OM_CFG_VERSION = 1
    static char appSsid[OM_WIFI_SSID_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 1
    static char appPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 1
    static char availableNetworkAppSsid[OM_WIFI_SSID_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 4
    static char availableNetworkAppPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE]; // since OM_CFG_VERSION = 4
    static boolean connectToNetworkIfAvailable; // since OM_CFG_VERSION = 3
    static boolean useBatteryProtection; // since OM_CFG_VERSION = 1
    static int wifiShutdownDelayMinutes; // since OM_CFG_VERSION = 2
    static int deepSleepDelayMinutes; // since OM_CFG_VERSION = 2
    static float batteryNominalVoltage; // since OM_CFG_VERSION = 1
    static float batteryLowVoltage; // since OM_CFG_VERSION = 1
    static float batteryShutdownVoltage; // since OM_CFG_VERSION = 1
    static boolean useActiveBreaking; // since OM_CFG_VERSION = 1
};

#endif
