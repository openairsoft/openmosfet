#ifndef MOSFETV2WIFISERVER_HPP
#define MOSFETV2WIFISERVER_HPP

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

class OMwifiserver
{
    public:
        static void begin();
        static void update();
        static AsyncEventSource events;
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
