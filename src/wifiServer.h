#ifndef MOSFETV2WIFISERVER_HPP
#define MOSFETV2WIFISERVER_HPP

#include <Arduino.h>
#include "config.h"
#include "configuration.h"
#include "autoUpdater.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>


class OMwifiserver
{
    public:
        static void begin();
        static void update();
    private:
        static boolean wifiIsOn;//Note this may be replaced by doing proper status tests on the esp wifi module
        static unsigned long lastActivityTimeMs;
        static boolean queryHasParameter(String parameterName);
        static void handleRoot(AsyncWebServerRequest *request);
        static void handleUpdate(AsyncWebServerRequest *request);
        static AsyncWebServer webServer;
        static DNSServer dnsServer;
        static IPAddress ip;
};

#endif
