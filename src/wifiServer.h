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
        static void handleRoot(AsyncWebServerRequest *request);
        static void handleAutoUpdateApi(AsyncWebServerRequest *request);

        static void handleManualUpdateApi(AsyncWebServerRequest *request);
        static void handleManualUpdateApiFile(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);

        static void handleVersionApi(AsyncWebServerRequest *request);

        static void handleConfigApi(AsyncWebServerRequest *request);
        static void handleConfigApiBody(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen, size_t index, size_t total);

        static void handleTriggerStateApi(AsyncWebServerRequest *request);
        static void handleTriggerStateApiBody(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen, size_t index, size_t total);

        static void handleTriggerBumpApi(AsyncWebServerRequest *request);

        static void handleSelectorStateApi(AsyncWebServerRequest *request);
        static void handleSelectorStateApiBody(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen, size_t index, size_t total);

        static void handleGearboxUncockingApi(AsyncWebServerRequest *request);
        static AsyncWebServer webServer;
        static DNSServer dnsServer;
        static IPAddress ip;
        static void refreshActivity();
};

#endif
