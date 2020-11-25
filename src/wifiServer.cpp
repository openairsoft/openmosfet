#include "wifiServer.h"
#include "ui.h"
#include "config.h"
#include "utilities.h"
#include "autoUpdater.h"
#include "configuration.h"

#include <AsyncElegantOTA.h>
#include <esp_wifi.h>
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WiFiAP.h>
// #include <AsyncTCP.h>
// #include <AsyncJson.h>


//TODO: penser à tout gzipper

extern OMVirtualReplica replica;

unsigned long OMwifiserver::lastActivityTimeMs = millis();
boolean OMwifiserver::wifiIsOn = false;
AsyncWebServer OMwifiserver::webServer(80);
AsyncEventSource OMwifiserver::events("/events");
AsyncCallbackJsonWebHandler OMwifiserver::jsonApiHandler("/api/config", OMwifiserver::handleConfigApi);
IPAddress OMwifiserver::ip(APPIP);
DNSServer OMwifiserver::dnsServer;

const String postForms = "yolo";


void OMwifiserver::begin()
{
  delay(1000);//Note: I don't remember why I did that, I'll just let it there in case it is important #prophesional

  OMwifiserver::webServer.on("/", OMwifiserver::handleRoot);//send back as web page
  OMwifiserver::webServer.on("/api/core", OMwifiserver::handleUpdate);
  OMwifiserver::webServer.addHandler(&OMwifiserver::events);
  OMwifiserver::webServer.addHandler(&OMwifiserver::jsonApiHandler);
  OMwifiserver::webServer.serveStatic("/", FILESYSTEM, "/");
 
  if(OMConfiguration::connectToNetworkIfAvailable)
  {
    WiFi.mode(WIFI_AP_STA);
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
  WiFi.softAPConfig(OMwifiserver::ip, OMwifiserver::ip, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00

  #ifdef DEBUG
    WiFi.printDiag(Serial);
  #endif

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  OMwifiserver::dnsServer.start(OM_DNS_PORT, "*", OMwifiserver::ip);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.localIP());

  // relay all unknown requests to root
  OMwifiserver::webServer.onNotFound([](AsyncWebServerRequest *request) {
    #ifdef DEBUG
    Serial.print("webServer.onNotFound : ");
    #endif

    const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://openmosfet.local/\" /></head><body><p>redirecting...</p></body>";
    request->send(200, "text/html", metaRefreshStr);//using this redirect method allows to send a 200 status and it helps with captive portal detection
      
  });

  AsyncElegantOTA.begin(&OMwifiserver::webServer);//OTA
  OMwifiserver::webServer.begin();
  Serial.println("HTTP server started");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void OMwifiserver::handleUpdate(AsyncWebServerRequest *request) {
  switch (request->method())
  {
    case HTTP_PATCH:
      #ifdef DEBUG
      Serial.println("patch");
      #endif
      OMwifiserver::webServer.end();
      OMAutoUpdater::requestUpdate();
      request->send(FILESYSTEM, "/cfg.json");//for ajax
    break;
  }
}

void OMwifiserver::handleConfigApi(AsyncWebServerRequest *request, JsonVariant &json) {
  #ifdef DEBUG
  Serial.println("handleConfigApi :");
  Serial.println("json reçu :");
  serializeJson(json, Serial);
  Serial.println("");
  #endif

  #ifdef DEBUG  
  Serial.println("config initiale :");
  // OMConfiguration::printCfg();
  #endif

  //-------------------- fireModes -------------------------
  if(json.containsKey("fireModes")){
    for(int currentFiremodeIndex = 0; currentFiremodeIndex < json["fireModes"].size() || currentFiremodeIndex < OM_MAX_NB_STORED_MODES; ++currentFiremodeIndex){

      Serial.print("currentFiremodeIndex :");
      Serial.println(currentFiremodeIndex);

      if (json["fireModes"][currentFiremodeIndex].containsKey("burstMode")) {
        switch(json["fireModes"][currentFiremodeIndex]["burstMode"].as<int>())
        {
          case 0 :
            OMConfiguration::fireModes[currentFiremodeIndex].setBurstMode(OMFiringSettings::burstModeNormal);
          break;
    
          case 1 :
            OMConfiguration::fireModes[currentFiremodeIndex].setBurstMode(OMFiringSettings::burstModeInterruptible);
          break;
          
          case 2 :
            OMConfiguration::fireModes[currentFiremodeIndex].setBurstMode(OMFiringSettings::burstModeExtendible);
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
        OMConfiguration::fireModes[currentFiremodeIndex].setMotorPower( json["fireModes"][currentFiremodeIndex]["motorPower"].as<uint8_t>() );
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
    boolean connectToNetworkIfAvailable = json["connectToNetworkIfAvailable"].as<boolean>();
    OMConfiguration::connectToNetworkIfAvailable = connectToNetworkIfAvailable;
  }
  
  //-------------------- useBatteryProtection -------------------------
  if(json.containsKey("useBatteryProtection")) {
    boolean useBatteryProtection = json["useBatteryProtection"].as<boolean>();
    OMConfiguration::useBatteryProtection = useBatteryProtection;
  }
  
  //-------------------- useActiveBreaking -------------------------
  if(json.containsKey("useActiveBreaking")) {
    boolean useActiveBreaking = json["useActiveBreaking"].as<boolean>();
    OMConfiguration::useActiveBreaking = useActiveBreaking;
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
    Serial.print("save status: ");
  #endif
    Serial.println(OMConfiguration::save());
  #ifdef DEBUG
    Serial.println("config finale :");
    OMConfiguration::printCfg();
  #endif

  //send back the json
  request->send(FILESYSTEM, "/cfg.json");//for ajax
}

void OMwifiserver::handleRoot(AsyncWebServerRequest *request) {
  #ifdef DEBUG
  Serial.println("handleRoot :");
  #endif

  //update activity
  OMwifiserver::lastActivityTimeMs = millis();
  replica.updateLastActive();

  if (request->method() != HTTP_POST) {
    //thank you https://github.com/ayushsharma82/AsyncElegantOTA/blob/master/src/AsyncElegantOTA.h
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", UI_HTML, UI_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
}

void OMwifiserver::update() {
  if(
    OMwifiserver::wifiIsOn
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
  
  AsyncElegantOTA.loop();
  OMAutoUpdater::update();
  
}
