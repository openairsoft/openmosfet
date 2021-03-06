#include "configuration.h"
#include "utilities.h"

const int capacity = OM_JSON_DOCUMENT_SIZE;//see https://arduinojson.org/v6/assistant/ with a cfg file with maximum allowed size 
//StaticJsonDocument<capacity> doc;


//initialization
char OMConfiguration::appSsid[OM_WIFI_SSID_REAL_MAX_SIZE] = OM_DEFAULT_APSSID;
char OMConfiguration::appPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE] = OM_DEFAULT_APP_PASSWD;
boolean OMConfiguration::connectToNetworkIfAvailable = OM_DEFAULT_CONNECT_TO_NETWORK_IF_AVAILABLE;
char OMConfiguration::availableNetworkAppSsid[OM_WIFI_SSID_REAL_MAX_SIZE] = OM_DEFAULT_AVAILABLE_NETWORK_APSSID;
char OMConfiguration::availableNetworkAppPasswd[OM_WIFI_PSSWD_REAL_MAX_SIZE] = OM_DEFAULT_AVAILABLE_NETWORK_APP_PASSWD;
boolean OMConfiguration::enableEspNow = OM_DEFAULT_ENABLE_ESP_NOW;
boolean OMConfiguration::disableMotor = OM_DEFAULT_DISABLE_MOTOR;
boolean OMConfiguration::enableBatteryProtection = OM_DEFAULT_USE_BATTERY_PROTECTION;
int OMConfiguration::wifiShutdownDelayMinutes = OM_DEFAULT_WIFI_SHUTDOWN_DELAY_MINUTES;
int OMConfiguration::deepSleepDelayMinutes = OM_DEFAULT_DEEP_SLEEP_DELAY_MINUTES;
float OMConfiguration::batteryNominalVoltage = OM_DEFAULT_NOMINAL_VOLTAGE;
float OMConfiguration::batteryLowVoltage = OM_DEFAULT_LOW_VOLTAGE;
float OMConfiguration::batteryShutdownVoltage = OM_DEFAULT_SHUTDOWN_VOLTAGE;
boolean OMConfiguration::enableActiveBreaking = OM_DEFAULT_USE_ACTIVE_BRAKING;
float OMConfiguration::decockAfter_s = OM_DEFAULT_DECOCK_AFTER_SECONDS;
boolean OMConfiguration::enablePrecocking = OM_DEFAULT_ENABLE_PRECOCKING;
int OMConfiguration::selectorCalibration[OM_MAX_NB_STORED_MODES+1] = {-1, -1, -1};
OMFiringSettings OMConfiguration::fireModes[OM_MAX_NB_STORED_MODES] = {OMFiringSettings(OMFiringSettings::burstModeNormal, 1, 0, 255, 0), OMFiringSettings(OMFiringSettings::burstModeExtendible, 1, 0, 255, 0)};
boolean OMConfiguration::enableSetupScreen = OM_DEFAULT_ENABLE_STARTUP_SCREEN;

void OMConfiguration::loadFromJson(Stream &stream){
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, stream);

  #ifdef DEBUG
  Serial.println("JSON CHARGE");
  serializeJson(doc, Serial);
  #endif
  
  strcpy(OMConfiguration::appSsid, doc["appSsid"]);
  strcpy(OMConfiguration::appPasswd, doc["appPasswd"]);
  OMConfiguration::connectToNetworkIfAvailable = doc["connectToNetworkIfAvailable"];
  strcpy(OMConfiguration::availableNetworkAppSsid, doc["availableNetworkAppSsid"]);
  strcpy(OMConfiguration::availableNetworkAppPasswd, doc["availableNetworkAppPasswd"]);
  OMConfiguration::enableEspNow = doc["enableEspNow"];
  OMConfiguration::enableBatteryProtection = doc["enableBatteryProtection"];
  OMConfiguration::disableMotor = doc["disableMotor"];
  OMConfiguration::wifiShutdownDelayMinutes = doc["wifiShutdownDelayMinutes"];
  OMConfiguration::deepSleepDelayMinutes = doc["deepSleepDelayMinutes"];
  OMConfiguration::batteryNominalVoltage = doc["batteryNominalVoltage"];
  OMConfiguration::batteryLowVoltage = doc["batteryLowVoltage"];
  OMConfiguration::batteryShutdownVoltage = doc["batteryShutdownVoltage"];
  OMConfiguration::enableActiveBreaking = doc["enableActiveBreaking"];
  OMConfiguration::decockAfter_s = doc["decockAfter_s"];
  OMConfiguration::enablePrecocking = doc["enablePrecocking"];
  OMConfiguration::enableSetupScreen = doc["enableSetupScreen"];
  
  for(int i = 0; i < OM_MAX_NB_STORED_MODES + 1; ++i)
  {
    OMConfiguration::selectorCalibration[i] = doc["selectorCalibration"][i].as<int>();
  }
  
  for(int i = 0; i < OM_MAX_NB_STORED_MODES; ++i)
  {
    JsonObject currentFireMode = doc["fireModes"][i];

    OMFiringSettings::BurstMode currentBurstMode =  currentFireMode["burstMode"].as<OMFiringSettings::BurstMode>();
    uint8_t currentBurstLength = currentFireMode["burstLength"].as<uint8_t>();
    unsigned int currentPrecockDuration = currentFireMode["precockDuration_ms"].as<unsigned int>();
    float currentMotorPower = currentFireMode["motorPower"].as<float>();
    unsigned int currentTimeBetweenShots = currentFireMode["timeBetweenShots_ms"].as<unsigned int>();
    
    OMConfiguration::fireModes[i] = OMFiringSettings(currentBurstMode, currentBurstLength, currentPrecockDuration, currentMotorPower, currentTimeBetweenShots);
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

DynamicJsonDocument OMConfiguration::toJson(){
  DynamicJsonDocument doc(capacity);
  
  doc["appSsid"] = OMConfiguration::appSsid;
  doc["appPasswd"] = OMConfiguration::appPasswd;
  doc["connectToNetworkIfAvailable"] = OMConfiguration::connectToNetworkIfAvailable;
  doc["availableNetworkAppSsid"] = OMConfiguration::availableNetworkAppSsid;
  doc["availableNetworkAppPasswd"] = OMConfiguration::availableNetworkAppPasswd;
  doc["enableEspNow"] = OMConfiguration::enableEspNow;
  doc["disableMotor"] = OMConfiguration::disableMotor;
  doc["enableBatteryProtection"] = OMConfiguration::enableBatteryProtection;
  doc["wifiShutdownDelayMinutes"] = OMConfiguration::wifiShutdownDelayMinutes;
  doc["deepSleepDelayMinutes"] = OMConfiguration::deepSleepDelayMinutes;
  doc["batteryNominalVoltage"] = OMConfiguration::batteryNominalVoltage;
  doc["batteryLowVoltage"] = OMConfiguration::batteryLowVoltage;
  doc["batteryShutdownVoltage"] = OMConfiguration::batteryShutdownVoltage;
  doc["enableActiveBreaking"] = OMConfiguration::enableActiveBreaking;
  doc["decockAfter_s"] = OMConfiguration::decockAfter_s;
  doc["enablePrecocking"] = OMConfiguration::enablePrecocking;
  doc["enableSetupScreen"] = OMConfiguration::enableSetupScreen;
  
  for(int i = 0; i < OM_MAX_NB_STORED_MODES + 1; ++i)
  {
    doc["selectorCalibration"][i] = OMConfiguration::selectorCalibration[i];
  }

  JsonArray fireModes = doc.createNestedArray("fireModes");
  
  for(int i = 0; i < OM_MAX_NB_STORED_MODES; ++i)
  {
    //info: OMFiringSettings(OMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, float motorPower, unsigned int timeBetweenShots_ms)
    JsonObject currentFireMode = fireModes.createNestedObject();
    
    currentFireMode["burstMode"] = OMConfiguration::fireModes[i].getBurstMode();
    currentFireMode["burstLength"] = OMConfiguration::fireModes[i].getBurstLength();
    currentFireMode["precockDuration_ms"] = OMConfiguration::fireModes[i].getPrecockDurationMs();
    currentFireMode["motorPower"] = OMConfiguration::fireModes[i].getMotorPower();
    currentFireMode["timeBetweenShots_ms"] = OMConfiguration::fireModes[i].getTimeBetweenShotsMs();
  }

  return doc;
}

boolean OMConfiguration::save(void){

  #ifdef DEBUG
    Serial.println(F("Saving config"));
  #endif
  
  DynamicJsonDocument doc = OMConfiguration::toJson();

  #ifdef DEBUG
  Serial.println("JSON SAUVE");
  serializeJson(doc, Serial);
  #endif

  File file = FILESYSTEM.open(OM_CONFIGFILE_NAME, "w");
  serializeJson(doc, file);
  file.close(); 

  return true;
}

boolean OMConfiguration::isSelectorCalibrated(void){
  boolean isCalibrated = true;
  int previousValue = -1;

  // values should go up from safe to last mode, if not there is a problem or gardient is in wrong direction
  for(int selectorPosition = 0 ; selectorPosition < OM_MAX_NB_STORED_MODES + 1 ; selectorPosition++)
  {
    if(OMConfiguration::selectorCalibration[selectorPosition] < 0 && previousValue < OMConfiguration::selectorCalibration[selectorPosition])
    {
      isCalibrated = false;
      break;
    }

    previousValue = OMConfiguration::selectorCalibration[selectorPosition];
  }

  return isCalibrated;
}

#ifdef DEBUG
  void OMConfiguration::printCfg(void){

    Serial.println(" ");
    Serial.println("---------------");
    Serial.println("Mémoire SPIFFS : " + String(SPIFFS.usedBytes()) + "/" + String(SPIFFS.totalBytes()) + " (" + String(((float)SPIFFS.usedBytes()/(float)SPIFFS.totalBytes())*100) + "%)");
    Serial.println("---------------");
    Serial.println(" ");
    
    Serial.print("appSsid=");
    Serial.println(OMConfiguration::appSsid);
    Serial.print("appPasswd=");
    Serial.println(OMConfiguration::appPasswd);
    Serial.print("connectToNetworkIfAvailable=");
    Serial.println(OMConfiguration::connectToNetworkIfAvailable);
    Serial.print("availableNetworkAppSsid=");
    Serial.println(OMConfiguration::availableNetworkAppSsid);
    Serial.print("availableNetworkAppPasswd=");
    Serial.println(OMConfiguration::availableNetworkAppPasswd);
    Serial.print("enableEspNow=");
    Serial.println(OMConfiguration::enableEspNow);
    Serial.print("disableMotor=");
    Serial.println(OMConfiguration::disableMotor);
    Serial.print("enableBatteryProtection=");
    Serial.println(OMConfiguration::enableBatteryProtection);
    Serial.print("wifiShutdownDelayMinutes=");
    Serial.println(OMConfiguration::wifiShutdownDelayMinutes);
    Serial.print("deepSleepDelayMinutes=");
    Serial.println(OMConfiguration::deepSleepDelayMinutes);
    Serial.print("batteryNominalVoltage=");
    Serial.println(OMConfiguration::batteryNominalVoltage);
    Serial.print("batteryLowVoltage=");
    Serial.println(OMConfiguration::batteryLowVoltage);
    Serial.print("batteryShutdownVoltage=");
    Serial.println(OMConfiguration::batteryShutdownVoltage);
    Serial.print("enableActiveBreaking=");
    Serial.println(OMConfiguration::enableActiveBreaking);
    Serial.print("decockAfter_s=");
    Serial.println(OMConfiguration::decockAfter_s);
    Serial.print("enablePrecocking=");
    Serial.println(OMConfiguration::enablePrecocking);
    Serial.print("enableSetupScreen=");
    Serial.println(OMConfiguration::enableSetupScreen);
    Serial.println("selectorCalibration={");
    for(int i = 0; i < OM_MAX_NB_STORED_MODES + 1; ++i)
    {
      Serial.printf("%i: %i(%f.2v)\n", i, OMConfiguration::selectorCalibration[i], ((float)OMConfiguration::selectorCalibration[i]) / (4095.0f/3.3f));
    }
    Serial.println("}");

    for(int i = 0; i < OM_MAX_NB_STORED_MODES; ++i)
      {
        //info: OMFiringSettings(OMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, float motorPower, unsigned int timeBetweenShots_ms)
        Serial.print(F("fireMode"));
        Serial.printf(
          "%i,%i,%i,%f,%i",
          OMConfiguration::fireModes[i].getBurstMode(),
          OMConfiguration::fireModes[i].getBurstLength(),
          OMConfiguration::fireModes[i].getPrecockDurationMs(),
          OMConfiguration::fireModes[i].getMotorPower(),
          OMConfiguration::fireModes[i].getTimeBetweenShotsMs()
          );
      }
    Serial.println(" ");
    Serial.println("---------------");
    Serial.println(" ");
  }
#endif
