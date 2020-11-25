#ifndef MOSFETV2WIFISERVER_HPP
#define MOSFETV2WIFISERVER_HPP

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <AsyncJson.h>

class OMwifiserver
{
    public:
        static void begin();
        static void update();
        static AsyncEventSource events;
    private:
        static AsyncCallbackJsonWebHandler jsonApiHandler;
        static boolean wifiIsOn;//Note this may be replaced by doing proper status tests on the esp wifi module
        static unsigned long lastActivityTimeMs;
        static void handleRedirectToRoot(AsyncWebServerRequest *request);
        static void handleConfigApi(AsyncWebServerRequest *request);
        static void handleConfigApiBody(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen, size_t index, size_t total);
        static void handleRoot(AsyncWebServerRequest *request);
        static void handleUpdate(AsyncWebServerRequest *request);
        static AsyncWebServer webServer;
        static DNSServer dnsServer;
        static IPAddress ip;
        static void refreshActivity();
};

#endif
