#include "wifiServer.h"
#include "ui.h"
#include "config.h"
#include "utilities.h"
#include "autoUpdater.h"
#include "configuration.h"
#include "components.h"

#include <Update.h>
#include <esp_wifi.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

//TODO: penser à tout gzipper (ah ?)

extern OMVirtualReplica replica;

unsigned long OMwifiserver::lastActivityTimeMs = millis();
boolean OMwifiserver::wifiIsOn = false;
AsyncWebServer OMwifiserver::webServer(80);
AsyncEventSource OMwifiserver::events("/events");
IPAddress OMwifiserver::ip(APPIP);
DNSServer OMwifiserver::dnsServer;

const String postForms = "yolo";


void OMwifiserver::begin()
{
  delay(1000);//Note: I don't remember why I did that, I'll just let it there in case it is important #prophesional

  OMwifiserver::webServer.on("/", OMwifiserver::handleRoot);//send back as web page
  OMwifiserver::webServer.on("/api/core/update", HTTP_GET, OMwifiserver::handleAutoUpdateApi);
  OMwifiserver::webServer.on("/api/core/update", HTTP_POST, OMwifiserver::handleManualUpdateApi, OMwifiserver::handleManualUpdateApiFile);
  OMwifiserver::webServer.on("/api/core/version", HTTP_GET, OMwifiserver::handleVersionApi);
  OMwifiserver::webServer.on("/api/config", HTTP_GET, OMwifiserver::handleConfigApi);
  OMwifiserver::webServer.on("/api/config", HTTP_POST, OMwifiserver::handleConfigApi, NULL, OMwifiserver::handleConfigApiBody);
  OMwifiserver::webServer.on("/api/components/trigger/state", HTTP_GET, OMwifiserver::handleTriggerStateApi);
  OMwifiserver::webServer.on("/api/components/trigger/state", HTTP_POST, OMwifiserver::handleTriggerStateApi, NULL, OMwifiserver::handleTriggerStateApiBody);
  OMwifiserver::webServer.on("/api/components/trigger/bump", HTTP_POST, OMwifiserver::handleTriggerBumpApi);
  OMwifiserver::webServer.on("/api/components/selector/state", HTTP_GET, OMwifiserver::handleSelectorStateApi);
  OMwifiserver::webServer.on("/api/components/selector/state", HTTP_POST, OMwifiserver::handleSelectorStateApi, NULL, OMwifiserver::handleSelectorStateApiBody);
  OMwifiserver::webServer.on("/api/components/gearbox/uncock", OMwifiserver::handleGearboxUncockingApi);
  OMwifiserver::webServer.addHandler(&OMwifiserver::events);
  OMwifiserver::webServer.serveStatic("/", FILESYSTEM, "/");
  OMwifiserver::webServer.onNotFound(OMwifiserver::handleRedirectToRoot);

  if(OMConfiguration::connectToNetworkIfAvailable)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.persistent(false);
    WiFi.begin(OMConfiguration::availableNetworkAppSsid, OMConfiguration::availableNetworkAppPasswd);
    OMwifiserver::wifiIsOn = true;
    #ifdef DEBUG
    Serial.println("");
    #endif
    // Wait for connection for OM_DEFAULT_WIFI_SEARCH_TIMEOUT_SECONDS
    unsigned long searchStartTime = millis();
    #ifdef DEBUG
      Serial.println("connecting to existing network...");
    #endif
    while (WiFi.status() != WL_CONNECTED) {
      #ifdef DEBUG
        Serial.print(".");
      #endif
      if( millis() - searchStartTime > SEC_TO_MILLIS(OM_DEFAULT_WIFI_SEARCH_TIMEOUT_SECONDS) )
      {
        #ifdef DEBUG
          Serial.println("");
          Serial.println("station connection timeout.");
          WiFi.disconnect();
        #endif
        WiFi.mode(WIFI_AP);
        break;
      }
    }
  }
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(OMConfiguration::appSsid, OMConfiguration::appPasswd);
  delay(2000);
  WiFi.softAPConfig(OMwifiserver::ip, OMwifiserver::ip, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00

  #ifdef DEBUG
    WiFi.printDiag(Serial);
  #endif

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  OMwifiserver::dnsServer.start(OM_DNS_PORT, "*", OMwifiserver::ip);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.localIP());

  OMwifiserver::webServer.begin();
  IPAddress myIP = WiFi.softAPIP();
  #ifdef DEBUG
  Serial.println("HTTP server started");
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  #endif
}
void OMwifiserver::handleRedirectToRoot(AsyncWebServerRequest *request) {
    #ifdef DEBUG
    Serial.print("webServer.onNotFound : ");
    #endif

    const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://openmosfet.local/\" /></head><body><p>redirecting...</p></body>";
    request->send(200, "text/html", metaRefreshStr);//using this redirect method allows to send a 200 status and it helps with captive portal detection
     
}

void OMwifiserver::handleManualUpdateApi(AsyncWebServerRequest *request) {
  #ifdef DEBUG
    Serial.print("OMwifiserver::handleManualUpdate :");
  #endif
  AsyncWebServerResponse *response = request->beginResponse((Update.hasError())?500:200, "text/plain", (Update.hasError())?"FAIL":"OK");
  response->addHeader("Connection", "close");
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}

// some inspiration : https://github.com/ayushsharma82/AsyncElegantOTA/blob/master/src/AsyncElegantOTA.h
void OMwifiserver::handleManualUpdateApiFile(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) { // Start with max available size
      #ifdef DEBUG
        Update.printError(Serial);
      #endif
      request->send(500, "text/plain", "OTA could not begin1");
    }else{
      #ifdef DEBUG
        Serial.println("OMwifiserver::handleManualUpdateFile : begin ota");
        Serial.print("filename : ");
        Serial.println(filename);
      #endif
    }
  }

  if(len){
    if (Update.write(data, len) != len) {
        request->send(500, "text/plain", "OTA could not begin2");
    }
  }
  if (final) { // if the final flag is set then this is the last frame of data
    if (!Update.end(true)) { //true to set the size to the current progress
      #ifdef DEBUG
        Update.printError(Serial);
      #endif
      return request->send(500, "text/plain", "Could not end OTA");
    }else{
      #ifdef DEBUG
        Serial.println("ota ok");
      #endif
    }
  }else{
      return;
  }
}

void OMwifiserver::handleAutoUpdateApi(AsyncWebServerRequest *request) {

  #ifdef DEBUG
  Serial.println("patch");
  #endif
  OMwifiserver::webServer.end();
  OMAutoUpdater::requestUpdate();
  while(OMAutoUpdater::stateUpdating){
  }
  request->send(200, "text/html", "ok");
}

void OMwifiserver::handleConfigApi(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  serializeJson(OMConfiguration::toJson(), *response);
  request->send(response);
}

void OMwifiserver::handleConfigApiBody(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen, size_t index, size_t total) {
  
  DynamicJsonDocument json(OM_JSON_DOCUMENT_SIZE);
                
  deserializeJson(json, (const char*)bodyData);

  #ifdef DEBUG
  Serial.println("handleConfigApi :");
  Serial.println("json reçu :");
  serializeJson(json, Serial);
  Serial.println("");
  #endif

  #ifdef DEBUG  
  Serial.println("config initiale :");
  OMConfiguration::printCfg();
  #endif
  //-------------------- fireModes -------------------------
  if(json.containsKey("fireModes")){
    for(int currentFiremodeIndex = 0; currentFiremodeIndex < json["fireModes"].size() || currentFiremodeIndex < OM_MAX_NB_STORED_MODES; ++currentFiremodeIndex){

      Serial.print("currentFiremodeIndex :");
      Serial.println(currentFiremodeIndex);

      if (json["fireModes"][currentFiremodeIndex].containsKey("burstMode")) {

        OMFiringSettings::BurstMode receivedBurstMode = json["fireModes"][currentFiremodeIndex]["burstMode"].as<OMFiringSettings::BurstMode>();
        switch(receivedBurstMode){
          case OMFiringSettings::burstModeNormal:
          case OMFiringSettings::burstModeInterruptible:
          case OMFiringSettings::burstModeExtendible:
            OMConfiguration::fireModes[currentFiremodeIndex].setBurstMode(receivedBurstMode);
          break;
        }
      }
      if (json["fireModes"][currentFiremodeIndex].containsKey("burstLength")) {//todo compare to max burst length
        OMConfiguration::fireModes[currentFiremodeIndex].setBurstLength( json["fireModes"][currentFiremodeIndex]["burstLength"].as<uint8_t>() );
      }
      if (json["fireModes"][currentFiremodeIndex].containsKey("precockDuration_ms")) {//todo compare to max burst length
        OMConfiguration::fireModes[currentFiremodeIndex].setPrecockDuration_ms( json["fireModes"][currentFiremodeIndex]["precockDuration_ms"].as<unsigned int>() );
      }
      if (json["fireModes"][currentFiremodeIndex].containsKey("motorPower")) {//todo compare to max / min motor power
        OMConfiguration::fireModes[currentFiremodeIndex].setMotorPower( json["fireModes"][currentFiremodeIndex]["motorPower"].as<float>() );
      }
      if (json["fireModes"][currentFiremodeIndex].containsKey("timeBetweenShots_ms")) {//todo compare to max timeBetweenShots
        OMConfiguration::fireModes[currentFiremodeIndex].setTimeBetweenShots_ms( json["fireModes"][currentFiremodeIndex]["timeBetweenShots_ms"].as<unsigned int>() );
      }
    }
  }

  //-------------------- batteryNominalVoltage -------------------------
  if(json.containsKey("batteryNominalVoltage")) {
    float batteryNominalVoltage = json["batteryNominalVoltage"].as<float>();
    if(OM_MIN_NOMINAL_VOLTAGE < batteryNominalVoltage && batteryNominalVoltage < OM_MAX_NOMINAL_VOLTAGE) {
      OMConfiguration::batteryNominalVoltage = batteryNominalVoltage;
    }
  }
  
  //-------------------- wifiShutdownDelayMinutes -------------------------
  if(json.containsKey("wifiShutdownDelayMinutes")) {
    int wifiShutdownDelayMinutes = json["wifiShutdownDelayMinutes"].as<int>();
    if(OM_MIN_WIFI_SHUTDOWN_DELAY_MINUTES <= wifiShutdownDelayMinutes) {
      OMConfiguration::wifiShutdownDelayMinutes = wifiShutdownDelayMinutes;
    }
  }
  
  //-------------------- deepSleepDelayMinutes -------------------------
  if(json.containsKey("deepSleepDelayMinutes")) {
    int deepSleepDelayMinutes = json["deepSleepDelayMinutes"].as<int>();
    if(OM_MIN_DEEP_SLEEP_DELAY_MINUTES <= deepSleepDelayMinutes) {
      OMConfiguration::deepSleepDelayMinutes = deepSleepDelayMinutes;
    }
  }
  
  //-------------------- batteryLowVoltage -------------------------
  if(json.containsKey("batteryLowVoltage")) {
    float batteryLowVoltage = json["batteryLowVoltage"].as<float>();
    if(OM_MIN_LOW_VOLTAGE < batteryLowVoltage && batteryLowVoltage < OM_MAX_LOW_VOLTAGE) {
      OMConfiguration::batteryLowVoltage = batteryLowVoltage;
    }
  }
      
  //-------------------- batteryShutdownVoltage -------------------------
  if(json.containsKey("batteryShutdownVoltage")) {
    float batteryShutdownVoltage = json["batteryShutdownVoltage"].as<float>();
    if(OM_MIN_SHUTDOWN_VOLTAGE < batteryShutdownVoltage && batteryShutdownVoltage < OM_MAX_SHUTDOWN_VOLTAGE) {
      OMConfiguration::batteryShutdownVoltage = batteryShutdownVoltage;
    }
  }  

  //-------------------- connectToNetworkIfAvailable -------------------------
  if(json.containsKey("connectToNetworkIfAvailable")) {
    OMConfiguration::connectToNetworkIfAvailable = json["connectToNetworkIfAvailable"].as<boolean>();
  }
  
  //-------------------- enableBatteryProtection -------------------------
  if(json.containsKey("enableBatteryProtection")) {
    OMConfiguration::enableBatteryProtection = json["enableBatteryProtection"].as<boolean>();
  }
  
  //-------------------- enableActiveBreaking -------------------------
  if(json.containsKey("enableActiveBreaking")) {
    OMConfiguration::enableActiveBreaking = json["enableActiveBreaking"].as<boolean>();
  }
  
  //-------------------- decockAfter_s -------------------------
  if(json.containsKey("decockAfter_s")) {
    OMConfiguration::decockAfter_s = json["decockAfter_s"].as<float>();
  }

  //-------------------- enablePrecocking -------------------------
  if(json.containsKey("enablePrecocking")) {
    OMConfiguration::enablePrecocking = json["enablePrecocking"].as<boolean>();
  }
  //-------------------- appSsid -------------------------
  if(json.containsKey("appSsid")) {
    String appSsid = json["appSsid"].as<String>();//compairing to false for safety reasons
    unsigned int appSsidLength = (unsigned int) appSsid.length();
    //TODO: '=' or '\r' are forbidden. validate
    if( ((unsigned int)OM_WIFI_SSID_MIN_SIZE) <= appSsidLength && appSsidLength <= ((unsigned int) OM_WIFI_SSID_MAX_SIZE) ) {
      appSsid.toCharArray(OMConfiguration::appSsid, appSsidLength+1);
    }
  }

  //-------------------- appPasswd -------------------------
  if(json.containsKey("appPasswd")) {
    String appPasswd = json["appPasswd"].as<String>();//compairing to false for safety reasons
    unsigned int appPasswdLength = (unsigned int) appPasswd.length();
    //TODO: '=' or '\r' are forbidden. validate
    if( ((unsigned int)OM_WIFI_PSSWD_MIN_SIZE) <= appPasswdLength && appPasswdLength <= ((unsigned int) OM_WIFI_PSSWD_MAX_SIZE) ) {
      appPasswd.toCharArray(OMConfiguration::appPasswd, appPasswdLength+1);
    }
  }

  
  //-------------------- availableNetworkAppSsid -------------------------
  if(json.containsKey("availableNetworkAppSsid")) {
    String availableNetworkAppSsid = json["availableNetworkAppSsid"].as<String>();//compairing to false for safety reasons
    unsigned int availableNetworkAppSsidLength = (unsigned int) availableNetworkAppSsid.length();
    //TODO: '=' or '\r' are forbidden. validate
    if( ((unsigned int)OM_WIFI_SSID_MIN_SIZE) <= availableNetworkAppSsidLength && availableNetworkAppSsidLength <= ((unsigned int) OM_WIFI_SSID_MAX_SIZE) ) {
      availableNetworkAppSsid.toCharArray(OMConfiguration::availableNetworkAppSsid, availableNetworkAppSsidLength+1);
    }
  }

  //-------------------- availableNetworkAppPasswd -------------------------
  if(json.containsKey("availableNetworkAppPasswd")) {
    String availableNetworkAppPasswd = json["availableNetworkAppPasswd"].as<String>();//compairing to false for safety reasons
    unsigned int availableNetworkAppPasswdLength = (unsigned int) availableNetworkAppPasswd.length();
    //TODO: '=' or '\r' are forbidden. validate
    if( ((unsigned int)OM_WIFI_PSSWD_MIN_SIZE) <= availableNetworkAppPasswdLength && availableNetworkAppPasswdLength <= ((unsigned int) OM_WIFI_PSSWD_MAX_SIZE) ) {
      availableNetworkAppPasswd.toCharArray(OMConfiguration::availableNetworkAppPasswd, availableNetworkAppPasswdLength+1);
    }
  }

  #ifdef DEBUG
  Serial.print("current status before save: ");
  OMConfiguration::printCfg();
  #endif
  OMConfiguration::save();
  #ifdef DEBUG
  Serial.println("config finale :");
  OMConfiguration::printCfg();
  #endif
}

void OMwifiserver::handleVersionApi(AsyncWebServerRequest *request) {
  request->send(200, "application/json", String("\"") + OM_FIRMWARE_VERSION + "\"");
}


void OMwifiserver::handleTriggerStateApi(AsyncWebServerRequest *request) {
  switch(request->method()){
    case HTTP_POST:
    case HTTP_GET:
      request->send(200, "application/json", (OMVirtualTrigger::getState() == OMVirtualTrigger::statePulled) ? "true":"false");
    break;
    default:
      request->send(405, "text/html", "bad method, POST or GET only");
    break;
  }
}

void OMwifiserver::handleTriggerStateApiBody(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen, size_t index, size_t total) {
  if(request->method() == HTTP_POST) {
      DynamicJsonDocument pulledStateJson(0);
      deserializeJson(pulledStateJson, (const char*)bodyData);

      if(pulledStateJson.as<boolean>()) {
        OMVirtualTrigger::pull();
      }else{
        OMVirtualTrigger::release();
      }
  }else{
      request->send(405, "text/html", "bad method, POST or GET only");
  }
}
  

void OMwifiserver::handleTriggerBumpApi(AsyncWebServerRequest *request) {
  OMVirtualTrigger::pull();
  OMVirtualTrigger::release();
  request->send(200);
}



void OMwifiserver::handleSelectorStateApi(AsyncWebServerRequest *request) {
  switch(request->method()){
    case HTTP_POST:
    case HTTP_GET:
      request->send(200, "application/json", String(OMVirtualSelector::getState()));
    break;
    default:
      request->send(405, "text/html", "bad method, POST or GET only");
    break;
  }
}

void OMwifiserver::handleSelectorStateApiBody(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen, size_t index, size_t total) {
  if(request->method() == HTTP_POST) {
      DynamicJsonDocument selectorStateJson(0);
      deserializeJson(selectorStateJson, (const char*)bodyData);

      OMVirtualSelector::SelectorState receivedState =  selectorStateJson.as<OMVirtualSelector::SelectorState>();

      switch(receivedState){
        case OMVirtualSelector::stateSafe:
        case OMVirtualSelector::stateSemi:
        case OMVirtualSelector::stateAuto:
          OMVirtualSelector::setState(receivedState);
        break;
        default:
          request->send(400, "text/html", "unknown selctor state value");
        break;
      }

  }else{
      request->send(405, "text/html", "bad method, POST or GET only");
  }
}

void OMwifiserver::handleGearboxUncockingApi(AsyncWebServerRequest *request) {
  switch(OMVirtualGearbox::getState()){
    case OMVirtualGearbox::statePrecocked:
      OMVirtualGearbox::cycle(0);
      request->send(200);
    break;
    case OMVirtualGearbox::stateResting:
      request->send(503, "text/html", "already uncocked");
    break;
    default:
      request->send(503, "text/html", "icompatible gearbox state at the moment");
    break;
  }
}

void OMwifiserver::handleRoot(AsyncWebServerRequest *request) {
  #ifdef DEBUG
  Serial.println("handleRoot :");
  #endif

  //update activity
  OMwifiserver::lastActivityTimeMs = millis();
  replica.updateLastActive();

  if (request->method() != HTTP_POST) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", UI_HTML, UI_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
}

void OMwifiserver::update() {
  if(
    OMwifiserver::wifiIsOn
    &&
    OMConfiguration::wifiShutdownDelayMinutes > 0
    &&
    ( millis() - OMwifiserver::lastActivityTimeMs > OMConfiguration::wifiShutdownDelayMinutes * 60000 )
    )
  {
    #ifdef DEBUG
    Serial.print("wifi disabled after ");
    Serial.print( float(millis() - OMwifiserver::lastActivityTimeMs) / 60000 );
    Serial.println(" minutes.");
    #endif
    //esp_bt_controller_disable();
    esp_wifi_stop();
    //esp_light_sleep_start();
    
    OMwifiserver::wifiIsOn = false;
  }else{
    OMwifiserver::dnsServer.processNextRequest();
  }
  
  OMAutoUpdater::update();
  
}
