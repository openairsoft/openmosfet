#ifndef CONFIG_H
#define CONFIG_H

//#define APPIP 192, 168, 0, 1
#define APPIP 8, 8, 8, 8 //NOTE: in order for the captive portal to work properly on android this address has to be out of the submask, see: https://github.com/tzapu/WiFiManager/issues/553


//---------- X : Global default parameters ---------------------------------------------------------------------------------------

#define OM_CONFIGFILE_NAME "/cfg.json"
#define OM_DEFAULT_APSSID "OpenMosfet"//Note : must be [OM_WIFI_SSID_MIN_SIZE-OM_WIFI_SSID_MAX_SIZE] character (exluding '\0') in order to work
#define OM_DEFAULT_APP_PASSWD  "password"//Note : must be [OM_WIFI_PSSWD_MIN_SIZE-OM_WIFI_PSSWD_MAX_SIZE] character (exluding '\0') in order to work
//#define OM_DEFAULT_APSSID {'o','p','e','n','M','o','s','f','e','t','\0'}//Note : must be [8-32] character (exluding '\0') in order to work
//#define OM_DEFAULT_APP_PASSWD  {'p','a','s','s','w','o','r','d','\0'}//Note : must be [8-16] character (exluding '\0') in order to work

#define OM_DEFAULT_AVAILABLE_NETWORK_APSSID "externalssid"//Note : must be [OM_WIFI_SSID_MIN_SIZE-OM_WIFI_SSID_MAX_SIZE] character (exluding '\0') in order to work
#define OM_DEFAULT_AVAILABLE_NETWORK_APP_PASSWD  "externalpassword"//Note : must be [OM_WIFI_PSSWD_MIN_SIZE-OM_WIFI_PSSWD_MAX_SIZE] character (exluding '\0') in order to work


#define OM_DEFAULT_WIFI_SEARCH_TIMEOUT_SECONDS 5
#define OM_DEFAULT_WIFI_SHUTDOWN_DELAY_MINUTES 15
#define OM_DEFAULT_DEEP_SLEEP_DELAY_MINUTES 60
#define OM_DEFAULT_NOMINAL_VOLTAGE 11.1
#define OM_DEFAULT_LOW_VOLTAGE 10.5
#define OM_DEFAULT_SHUTDOWN_VOLTAGE 9.9 //Typically 3.3V x nb of cells
#define OM_DEFAULT_CONNECT_TO_NETWORK_IF_AVAILABLE false
#define OM_DEFAULT_USE_BATTERY_PROTECTION true
#define OM_DEFAULT_USE_ACTIVE_BRAKING false

#define OM_DNS_PORT  53

// If motor is spinning and no cycle has been detected in this time interval, then somethings wrong
// shut down the motor and report the error.
#define MAX_TIME_BETWEEN_CYCLES_MS 500

#ifdef DEBUG
#define OM_MIN_WIFI_SHUTDOWN_DELAY_MINUTES 0
#define OM_MIN_DEEP_SLEEP_DELAY_MINUTES 0
#else
#define OM_MIN_WIFI_SHUTDOWN_DELAY_MINUTES 1
#define OM_MIN_DEEP_SLEEP_DELAY_MINUTES 10
#endif

// define accepted tensions for voltages config
#define OM_MIN_NOMINAL_VOLTAGE 0.
#define OM_MAX_NOMINAL_VOLTAGE 20.
#define OM_MIN_LOW_VOLTAGE 0.
#define OM_MAX_LOW_VOLTAGE 20.
#define OM_MIN_SHUTDOWN_VOLTAGE 0.
#define OM_MAX_SHUTDOWN_VOLTAGE 20.

#define OM_WIFI_SSID_MIN_SIZE 8 //below 8 chars esp8266 wont take it into account 
#define OM_WIFI_SSID_MAX_SIZE 32
#define OM_WIFI_SSID_REAL_MAX_SIZE 33 //NOTE: must be equal to OM_WIFI_SSID_MAX_SIZE+1
#define OM_WIFI_PSSWD_MIN_SIZE 8
#define OM_WIFI_PSSWD_MAX_SIZE 16
#define OM_WIFI_PSSWD_REAL_MAX_SIZE 17 //NOTE: must be equal to OM_WIFI_PSSWD_MAX_SIZE+1

// If motor is spinning and no cycle has been detected in this time interval, then somethings wrong
// shut down the motor and report the error.
#define MAX_TIME_BETWEEN_CYCLES_MS 500

// If we detect a trigger pressed for more than this duration, we cut off the "trigger" and report 
// the information.
#define MAX_CONTINUOUS_TRIGGER_PULL_DURATION_MS 3000

//This constant specify the replica used
//#define REPLICA_TYPE_GGTR16
#define REPLICA_TYPE_CYMACGP90

//This specify the max number of config modes stored
#define OM_MAX_NB_STORED_MODES 2


//---------- X : Global variables ------------------------------------------------------------------------------------------------

#endif
