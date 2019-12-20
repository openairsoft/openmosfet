#ifndef MOSFETV2WIFISERVER_HPP
#define MOSFETV2WIFISERVER_HPP

#include "config.h"
#include "configuration.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>


class Mosfetv2wifiserver
{
    public:
        static void begin();
        static void update();
    private:
        static boolean wifiIsOn;//Note this may be replaced by doing proper status tests on the esp wifi module
        static unsigned long lastActivityTimeMs;
        static boolean queryHasParameter(String parameterName);
        static void handleRoot(AsyncWebServerRequest *request);
        static AsyncWebServer webServer;
        static DNSServer dnsServer;
        static IPAddress ip;
};

#endif
