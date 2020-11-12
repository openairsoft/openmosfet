#include "configuration.h"
#include "utilities.h"

const int capacity = JSON_ARRAY_SIZE(OM_MAX_NB_STORED_MODES) + OM_MAX_NB_STORED_MODES*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(13) + 535 + 2000;//see https://arduinojson.org/v6/assistant/ with a cfg file with maximum allowed size 
//StaticJsonDocument<capacity> doc;


//initialization
AAMFiringSettings OMConfiguration::fireModes[OM_MAX_NB_STORED_MODES] = {AAMFiringSettings(AAMFiringSettings::burstModeNormal, 1, 0, 255, 0), AAMFiringSettings(AAMFiringSettings::burstModeExtendible, 1, 0, 255, 0)};
char OMConfiguration::appSsid[OM_WIFI_SSID_REAL_MAX_SIZE] = OM_DEFAULT_APSSID;
char OMConfiguration::appPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE] = OM_DEFAULT_APP_PASSWD;
char OMConfiguration::availableNetworkAppSsid[OM_WIFI_SSID_REAL_MAX_SIZE] = OM_DEFAULT_AVAILABLE_NETWORK_APSSID;
char OMConfiguration::availableNetworkAppPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE] = OM_DEFAULT_AVAILABLE_NETWORK_APP_PASSWD;

boolean OMConfiguration::connectToNetworkIfAvailable = OM_DEFAULT_CONNECT_TO_NETWORK_IF_AVAILABLE;
boolean OMConfiguration::useBatteryProtection = OM_DEFAULT_USE_BATTERY_PROTECTION;
int OMConfiguration::wifiShutdownDelayMinutes = OM_DEFAULT_WIFI_SHUTDOWN_DELAY_MINUTES;
int OMConfiguration::deepSleepDelayMinutes = OM_DEFAULT_DEEP_SLEEP_DELAY_MINUTES;
float OMConfiguration::batteryNominalVoltage = OM_DEFAULT_NOMINAL_VOLTAGE;
float OMConfiguration::batteryLowVoltage = OM_DEFAULT_LOW_VOLTAGE;
float OMConfiguration::batteryShutdownVoltage = OM_DEFAULT_SHUTDOWN_VOLTAGE;
boolean OMConfiguration::useActiveBreaking = OM_DEFAULT_USE_ACTIVE_BRAKING;

boolean OMConfiguration::loadFromJson(Stream &stream){
  DynamicJsonDocument doc(capacity);
  
  Serial.println(deserializeJson(doc, stream).c_str());

  Serial.println("JSON CHARGE");
  serializeJson(doc, Serial);
  
  strcpy(OMConfiguration::appSsid, doc["appSsid"]);
  strcpy(OMConfiguration::appPasswd, doc["appPasswd"]);
  strcpy(OMConfiguration::availableNetworkAppSsid, doc["availableNetworkAppSsid"]);
  strcpy(OMConfiguration::availableNetworkAppPasswd, doc["availableNetworkAppPasswd"]);
  OMConfiguration::connectToNetworkIfAvailable = doc["connectToNetworkIfAvailable"];
  OMConfiguration::useBatteryProtection = doc["useBatteryProtection"];
  OMConfiguration::wifiShutdownDelayMinutes = doc["wifiShutdownDelayMinutes"];
  OMConfiguration::deepSleepDelayMinutes = doc["deepSleepDelayMinutes"];
  OMConfiguration::batteryNominalVoltage = doc["batteryNominalVoltage"];
  OMConfiguration::batteryLowVoltage = doc["batteryLowVoltage"];
  OMConfiguration::batteryShutdownVoltage = doc["batteryShutdownVoltage"];
  OMConfiguration::useActiveBreaking = doc["useActiveBreaking"];
  
  int i = 0;
  for(i = 0; i < OM_MAX_NB_STORED_MODES; ++i)
  {
    JsonObject currentFireMode = doc["fireModes"][i];
    //info: AAMFiringSettings(AAMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, uint8_t motorPower, unsigned int timeBetweenShots_ms)
    AAMFiringSettings::BurstMode currentBurstMode;
    
    switch((int)currentFireMode["burstMode"])
    {
      case 0 :
        currentBurstMode = AAMFiringSettings::burstModeNormal;
      break;

      case 1 :
        currentBurstMode = AAMFiringSettings::burstModeInterruptible;
      break;
      
      case 2 :
        currentBurstMode = AAMFiringSettings::burstModeExtendible;
      break;
    }
    uint8_t currentBurstLength = (uint8_t)currentFireMode["burstLength"];
    unsigned int currentPrecockDuration = (unsigned int)currentFireMode["precockDuration_ms"];
    uint8_t currentMotorPower = (uint8_t)currentFireMode["motorPower"];
    unsigned int currentTimeBetweenShots = (unsigned int)currentFireMode["timeBetweenShots_ms"];
    
    OMConfiguration::fireModes[i] = AAMFiringSettings(currentBurstMode, currentBurstLength, currentPrecockDuration, currentMotorPower, currentTimeBetweenShots);
  }
}

boolean OMConfiguration::load(void){
  #ifdef DEBUG
    Serial.println(F("Loading config"));
  #endif

  // code from https://projetsdiy.fr/esp8266-webserveur-partie4-arduinojson-charger-enregistrer-fichiers-spiffs/#Manipuler_le_JSON_Ajout_modification_suppression_de_donnees
  File file = FILESYSTEM.open(OM_CONFIGFILE_NAME, "r");
  if (!file){
    #ifdef DEBUG
      Serial.println("Aucun historique existe - No History Exist");
    #endif
    OMConfiguration::save();
  } else {
    size_t size = file.size();
    if ( size == 0 ) {
      #ifdef DEBUG
        Serial.println("Fichier historique vide - History file empty !");
      #endif
    OMConfiguration::save();
    } else {
      OMConfiguration::loadFromJson(file);
    }
    file.close();
  }

  return true;
}


boolean OMConfiguration::save(void){
  #ifdef DEBUG
    Serial.println(F("Saving config"));
  #endif
  DynamicJsonDocument doc(capacity);
  
  doc["appSsid"] = OMConfiguration::appSsid;
  doc["appPasswd"] = OMConfiguration::appPasswd;
  doc["availableNetworkAppSsid"] = OMConfiguration::availableNetworkAppSsid;
  doc["availableNetworkAppPasswd"] = OMConfiguration::availableNetworkAppPasswd;
  doc["connectToNetworkIfAvailable"] = OMConfiguration::connectToNetworkIfAvailable;
  doc["useBatteryProtection"] = OMConfiguration::useBatteryProtection;
  doc["wifiShutdownDelayMinutes"] = OMConfiguration::wifiShutdownDelayMinutes;
  doc["deepSleepDelayMinutes"] = OMConfiguration::deepSleepDelayMinutes;
  doc["batteryNominalVoltage"] = OMConfiguration::batteryNominalVoltage;
  doc["batteryLowVoltage"] = OMConfiguration::batteryLowVoltage;
  doc["batteryShutdownVoltage"] = OMConfiguration::batteryShutdownVoltage;
  doc["useActiveBreaking"] = OMConfiguration::useActiveBreaking;
  
  JsonArray fireModes = doc.createNestedArray("fireModes");
  
  int i = 0;
  for(i = 0; i < OM_MAX_NB_STORED_MODES; ++i)
  {
    //info: AAMFiringSettings(AAMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, uint8_t motorPower, unsigned int timeBetweenShots_ms)
    JsonObject currentFireMode = fireModes.createNestedObject();
    
    switch(OMConfiguration::fireModes[i].getBurstMode())
    {
      case AAMFiringSettings::burstModeNormal :
        currentFireMode["burstMode"] = 0;
      break;

      case AAMFiringSettings::burstModeInterruptible :
        currentFireMode["burstMode"] = 1;
      break;
      
      case AAMFiringSettings::burstModeExtendible :
        currentFireMode["burstMode"] = 2;
      break;
    }
    currentFireMode["burstLength"] = OMConfiguration::fireModes[i].getBurstLength();
    currentFireMode["precockDuration_ms"] = OMConfiguration::fireModes[i].getPrecockDurationMs();
    currentFireMode["motorPower"] = OMConfiguration::fireModes[i].getMotorPower();
    currentFireMode["timeBetweenShots_ms"] = OMConfiguration::fireModes[i].getTimeBetweenShotsMs();
  }
  
  Serial.println("JSON SAUVE");
  serializeJson(doc, Serial);

  File file = FILESYSTEM.open(OM_CONFIGFILE_NAME, "w");
  serializeJson(doc, file);
  file.close(); 

  return true;
}

#ifdef DEBUG
  void OMConfiguration::printCfg(void){
    
    Serial.println(" ");
    Serial.println("---------------");
    Serial.println(" ");

    int i = 0;
    for(i = 0; i < OM_MAX_NB_STORED_MODES; ++i)
    {
      //info: AAMFiringSettings(AAMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, uint8_t motorPower, unsigned int timeBetweenShots_ms)
      Serial.print(F("fireMode"));
      Serial.print(i);
      Serial.print(F("="));
      switch(OMConfiguration::fireModes[i].getBurstMode())
      {
        case AAMFiringSettings::burstModeNormal :
          Serial.print(F("0,"));
        break;
  
        case AAMFiringSettings::burstModeInterruptible :
          Serial.print(F("1,"));
        break;
        
        case AAMFiringSettings::burstModeExtendible :
          Serial.print(F("2,"));
        break;
      }
      Serial.print(OMConfiguration::fireModes[i].getBurstLength());
      Serial.print(F(","));
      Serial.print(OMConfiguration::fireModes[i].getPrecockDurationMs());
      Serial.print(F(","));
      Serial.print(OMConfiguration::fireModes[i].getMotorPower());
      Serial.print(F(","));
      Serial.println(OMConfiguration::fireModes[i].getTimeBetweenShotsMs());
    }
    Serial.print(F("appSsid="));
    Serial.println(OMConfiguration::appSsid);
    Serial.print(F("appPasswd="));
    Serial.println(OMConfiguration::appPasswd);
    Serial.print(F("connectToNetworkIfAvailable="));
    Serial.println(OMConfiguration::connectToNetworkIfAvailable);
    Serial.print(F("availableNetworkAppSsid="));
    Serial.println(OMConfiguration::availableNetworkAppSsid);
    Serial.print(F("availableNetworkAppPasswd="));
    Serial.println(OMConfiguration::availableNetworkAppPasswd);
    Serial.print(F("useBatteryProtection="));
    Serial.println(OMConfiguration::useBatteryProtection);
    Serial.print(F("wifiShutdownDelayMinutes="));
    Serial.println(OMConfiguration::wifiShutdownDelayMinutes);
    Serial.print(F("deepSleepDelayMinutes="));
    Serial.println(OMConfiguration::deepSleepDelayMinutes);
    Serial.print(F("batteryNominalVoltage="));
    Serial.println(OMConfiguration::batteryNominalVoltage);
    Serial.print(F("batteryLowVoltage="));
    Serial.println(OMConfiguration::batteryLowVoltage);
    Serial.print(F("batteryShutdownVoltage="));
    Serial.println(OMConfiguration::batteryShutdownVoltage);
    Serial.print(F("useActiveBreaking="));
    Serial.println(OMConfiguration::useActiveBreaking);
    
    Serial.println(" ");
    Serial.println("---------------");
    Serial.println(" ");
  }
#endif
