#include "Mosfetv2wifiserver.h"
#include "utilities.h"

#include <AsyncElegantOTA.h>

#include "ui.h"

//TODO: penser à tout gzipper

extern AAMVirtualReplica replica;

unsigned long Mosfetv2wifiserver::lastActivityTimeMs = millis();
boolean Mosfetv2wifiserver::wifiIsOn = false;
AsyncWebServer Mosfetv2wifiserver::webServer(80);
IPAddress Mosfetv2wifiserver::ip(APPIP);
DNSServer Mosfetv2wifiserver::dnsServer;

const String postForms = "yolo";


void Mosfetv2wifiserver::begin()
{
  delay(1000);//Note: I don't remember why I did that, I'll just let it there in case it is important #prophesional
  //WiFi.setHostname("openmosfet");

  if(OMConfiguration::connectToNetworkIfAvailable)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(OMConfiguration::availableNetworkAppSsid, OMConfiguration::availableNetworkAppPasswd);
    Mosfetv2wifiserver::wifiIsOn = true;
  
    Serial.println("");
    
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
    
    if(!MDNS.begin("openmosfet")) {
      Serial.println("Error starting mDNS");
    }
  }
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(OMConfiguration::appSsid, OMConfiguration::appPasswd);
  WiFi.softAPConfig(Mosfetv2wifiserver::ip, Mosfetv2wifiserver::ip, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00

  #ifdef DEBUG
    WiFi.printDiag(Serial);
  #endif

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  Mosfetv2wifiserver::dnsServer.start(OM_DNS_PORT, "*", Mosfetv2wifiserver::ip);

  #ifdef DEBUG
    Serial.print("AP IP address: ");
    Serial.println(WiFi.localIP());
  #endif

  Mosfetv2wifiserver::webServer.on("/", Mosfetv2wifiserver::handleRoot);//send back as web page
  Mosfetv2wifiserver::webServer.on("/json", Mosfetv2wifiserver::handleRoot);//update and send back as json (if POST)

  Mosfetv2wifiserver::webServer.serveStatic("/", FILESYSTEM, "/");
 
  // relay all unknown requests to root
  Mosfetv2wifiserver::webServer.onNotFound([](AsyncWebServerRequest *request) {
    Serial.print("webServer.onNotFound : ");
  
    const char *metaRefreshStr = "<head><meta http-equiv=\"refresh\" content=\"0; url=http://openmosfet.local/\" /></head><body><p>redirecting...</p></body>";
    request->send(200, "text/html", metaRefreshStr);//using this redirect method allows to send a 200 status and it helps with captive portal detection
      
  });
  /*
  Mosfetv2wifiserver::webServer.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404);
  });*/

  AsyncElegantOTA.begin(&Mosfetv2wifiserver::webServer);//OTA
  Mosfetv2wifiserver::webServer.begin();
  Serial.println("HTTP server started");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

/*
var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        console.log(this.responseText);
       }
     };

   xhttp.open("POST", "/", true);
   xhttp.send();
*/

void Mosfetv2wifiserver::handleRoot(AsyncWebServerRequest *request) {
  #ifdef DEBUG
  Serial.println("handleRoot :");
  #endif

  //update activity
  Mosfetv2wifiserver::lastActivityTimeMs = millis();
  replica.updateLastActive();

  if (request->method() != HTTP_POST) {
    //thank you https://github.com/ayushsharma82/AsyncElegantOTA/blob/master/src/AsyncElegantOTA.h
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", UI_HTML, UI_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  } else {
    #ifdef DEBUG
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){ //p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    
      Serial.println("config initiale :");
      OMConfiguration::printCfg();
    #endif
    //we will check each parameters individually

    

    //-------------------- fireModes -------------------------
    int i = 0;
    for(i = 0; i < OM_MAX_NB_STORED_MODES; ++i)
    {
      if (request->hasParam(String("burstMode") + i, true)) {
        String currentBurstModeValue = request->getParam(String("burstMode") + i, true)->value();
        
        switch(currentBurstModeValue.toInt())
        {
          case 0 :
            OMConfiguration::fireModes[i].setBurstMode(AAMFiringSettings::burstModeNormal);
          break;
    
          case 1 :
            OMConfiguration::fireModes[i].setBurstMode(AAMFiringSettings::burstModeInterruptible);
          break;
          
          case 2 :
            OMConfiguration::fireModes[i].setBurstMode(AAMFiringSettings::burstModeExtendible);
          break;
        }
      }
      if (request->hasParam(String("burstLength") + i, true)) {
        OMConfiguration::fireModes[i].setBurstLength( (uint8_t)(request->getParam(String("burstLength") + i, true)->value().toInt()) );
      }
      if (request->hasParam(String("precockDuration_ms") + i, true)) {
        OMConfiguration::fireModes[i].setPrecockDuration_ms( (unsigned int)(request->getParam(String("precockDuration_ms") + i, true)->value().toInt()) );
      }
      if (request->hasParam(String("motorPower") + i, true)) {
        OMConfiguration::fireModes[i].setMotorPower( (uint8_t)(request->getParam(String("motorPower") + i, true)->value().toInt()) );
      }
      if (request->hasParam(String("timeBetweenShots_ms") + i, true)) {
        OMConfiguration::fireModes[i].setTimeBetweenShots_ms( (unsigned int)(request->getParam(String("timeBetweenShots_ms") + i, true)->value().toInt()) );
      }
    } 

    //-------------------- batteryNominalVoltage -------------------------
    if(request->hasParam("batteryNominalVoltage", true)) {
        Serial.print("batteryNominalVoltage : ");
      float batteryNominalVoltage = request->getParam("batteryNominalVoltage", true)->value().toFloat();
        Serial.println(batteryNominalVoltage);
      if(OM_MIN_NOMINAL_VOLTAGE < batteryNominalVoltage && batteryNominalVoltage < OM_MAX_NOMINAL_VOLTAGE) {
        OMConfiguration::batteryNominalVoltage = batteryNominalVoltage;
      }
    }
    
    //-------------------- wifiShutdownDelayMinutes -------------------------
    if(request->hasParam("wifiShutdownDelayMinutes", true)) {
      int wifiShutdownDelayMinutes = request->getParam("wifiShutdownDelayMinutes", true)->value().toInt();
      if(OM_MIN_WIFI_SHUTDOWN_DELAY_MINUTES <= wifiShutdownDelayMinutes) {
        OMConfiguration::wifiShutdownDelayMinutes = wifiShutdownDelayMinutes;
      }
    }
    
    //-------------------- deepSleepDelayMinutes -------------------------
    if(request->hasParam("deepSleepDelayMinutes", true)) {
      int deepSleepDelayMinutes = request->getParam("deepSleepDelayMinutes", true)->value().toInt();
      if(OM_MIN_DEEP_SLEEP_DELAY_MINUTES <= deepSleepDelayMinutes) {
        OMConfiguration::deepSleepDelayMinutes = deepSleepDelayMinutes;
      }
    }
    
    //-------------------- batteryLowVoltage -------------------------
    if(request->hasParam("batteryLowVoltage", true)) {
      float batteryLowVoltage = request->getParam("batteryLowVoltage", true)->value().toFloat();
      if(OM_MIN_LOW_VOLTAGE < batteryLowVoltage && batteryLowVoltage < OM_MAX_LOW_VOLTAGE) {
        OMConfiguration::batteryLowVoltage = batteryLowVoltage;
      }
    }
       
    //-------------------- batteryShutdownVoltage -------------------------
    if(request->hasParam("batteryShutdownVoltage", true)) {
      float batteryShutdownVoltage = request->getParam("batteryShutdownVoltage", true)->value().toFloat();
      if(OM_MIN_SHUTDOWN_VOLTAGE < batteryShutdownVoltage && batteryShutdownVoltage < OM_MAX_SHUTDOWN_VOLTAGE) {
        OMConfiguration::batteryShutdownVoltage = batteryShutdownVoltage;
      }
    }  

    //-------------------- connectToNetworkIfAvailable -------------------------
    if(request->hasParam("connectToNetworkIfAvailable", true)) {
      boolean connectToNetworkIfAvailable = request->getParam("connectToNetworkIfAvailable", true)->value().equals("true");//compairing to true : failsafe to default value
      OMConfiguration::connectToNetworkIfAvailable = connectToNetworkIfAvailable;
    }
    
    //-------------------- useBatteryProtection -------------------------
    if(request->hasParam("useBatteryProtection", true)) {
      boolean useBatteryProtection = ! request->getParam("useBatteryProtection", true)->value().equals("false");//compairing to false for safety reasons : failsafe to default value
      OMConfiguration::useBatteryProtection = useBatteryProtection;
    }
    
    //-------------------- useActiveBreaking -------------------------
    if(request->hasParam("useActiveBreaking", true)) {
      boolean useActiveBreaking = request->getParam("useActiveBreaking", true)->value().equals("true");
      OMConfiguration::useActiveBreaking = useActiveBreaking;
    }
    
    //-------------------- appSsid -------------------------
    if(request->hasParam("appSsid", true)) {
      String appSsid = request->getParam("appSsid", true)->value();//compairing to false for safety reasons
      unsigned int appSsidLength = (unsigned int) appSsid.length();
      //TODO: '=' or '\r' are forbidden. validate
      if( ((unsigned int)OM_WIFI_SSID_MIN_SIZE) <= appSsidLength && appSsidLength <= ((unsigned int) OM_WIFI_SSID_MAX_SIZE) ) {
        appSsid.toCharArray(OMConfiguration::appSsid, appSsidLength+1);
      }
    }

    //-------------------- appPasswd -------------------------
    if(request->hasParam("appPasswd", true)) {
      String appPasswd = request->getParam("appPasswd", true)->value();//compairing to false for safety reasons
      unsigned int appPasswdLength = (unsigned int) appPasswd.length();
      //TODO: '=' or '\r' are forbidden. validate
      if( ((unsigned int)OM_WIFI_PSSWD_MIN_SIZE) <= appPasswdLength && appPasswdLength <= ((unsigned int) OM_WIFI_PSSWD_MAX_SIZE) ) {
        appPasswd.toCharArray(OMConfiguration::appPasswd, appPasswdLength+1);
      }
    }

    
    //-------------------- availableNetworkAppSsid -------------------------
    if(request->hasParam("availableNetworkAppSsid", true)) {
      String availableNetworkAppSsid = request->getParam("availableNetworkAppSsid", true)->value();//compairing to false for safety reasons
      unsigned int availableNetworkAppSsidLength = (unsigned int) availableNetworkAppSsid.length();
      //TODO: '=' or '\r' are forbidden. validate
      if( ((unsigned int)OM_WIFI_SSID_MIN_SIZE) <= availableNetworkAppSsidLength && availableNetworkAppSsidLength <= ((unsigned int) OM_WIFI_SSID_MAX_SIZE) ) {
        availableNetworkAppSsid.toCharArray(OMConfiguration::availableNetworkAppSsid, availableNetworkAppSsidLength+1);
      }
    }

    //-------------------- availableNetworkAppPasswd -------------------------
    if(request->hasParam("availableNetworkAppPasswd", true)) {
      String availableNetworkAppPasswd = request->getParam("availableNetworkAppPasswd", true)->value();//compairing to false for safety reasons
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

    /*
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["heap"] = ESP.getFreeHeap();
    root["ssid"] = WiFi.SSID();
    root.printTo(*response);
    request->send(response);
    */

    //send back the json
    if(request->url().equals("/json")){
      request->send(FILESYSTEM, "/cfg.json");//for ajax
    }else{
      //thank you https://github.com/ayushsharma82/AsyncElegantOTA/blob/master/src/AsyncElegantOTA.h
      AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", UI_HTML, UI_HTML_SIZE);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
  }
}

void Mosfetv2wifiserver::update() {
  if(
    Mosfetv2wifiserver::wifiIsOn
    &&
    ( millis() - Mosfetv2wifiserver::lastActivityTimeMs > OMConfiguration::wifiShutdownDelayMinutes * 60000 )
    )
  {
    #ifdef DEBUG
    Serial.print("wifi disabled after ");
    Serial.print( float(millis() - Mosfetv2wifiserver::lastActivityTimeMs) / 60000 );
    Serial.println(" minutes.");
    #endif
    //esp_bt_controller_disable();
    esp_wifi_stop();
    //esp_light_sleep_start();
    
    Mosfetv2wifiserver::wifiIsOn = false;
  }else{
    Mosfetv2wifiserver::dnsServer.processNextRequest();
  }
  
  AsyncElegantOTA.loop();
  
  //Mosfetv2wifiserver::webServer.handleClient();
}
