#include "autoUpdater.h"
#include "config.h"
//#include <AsyncElegantOTA.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

const int capacity = 2000;

void OMAutoUpdater::updateFromGit(){

	HTTPClient http;

	Serial.print("[HTTP] begin...\n");
	// configure traged server and url
	//http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
	http.begin(OM_DEFAULT_GITHUB_API_LATEST_RELEASE_URL); //HTTP

	Serial.print("[HTTP] GET...\n");
	// start connection and send HTTP header
	int httpCode = http.GET();

	// httpCode will be negative on error
	if(httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);

		// file found at server
		if(httpCode == HTTP_CODE_OK) {

			// The filter: it contains "true" for each value we want to keep
			StaticJsonDocument<200> filter;
			filter["assets"][0]["name"] = true;
			filter["assets"][0]["browser_download_url"] = true;
			filter["assets"][0]["size"] = true;

  			DynamicJsonDocument doc(capacity);
			deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));

			boolean updateFound = false;
			for(int i = 0; i < doc["assets"].size(); ++i){
				if( doc["assets"][0]["name"].as<String>().equals(OM_DEFAULT_BINARY_FILE_NAME)){

					Serial.println( "lol -> " + doc["assets"][0]["name"].as<String>() );
					updateFound = true;
				}
			}


		}
	} else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}

} 