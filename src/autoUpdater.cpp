#include "config.h"
#include "autoUpdater.h"
#include "utilities.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Update.h>

boolean OMAutoUpdater::isUpdateRequested = false;

void OMAutoUpdater::update(){
	if(OMAutoUpdater::isUpdateRequested == true){
		OMAutoUpdater::updateFromGit();
	}
}

void OMAutoUpdater::requestUpdate(){
	//change the status
	OMAutoUpdater::isUpdateRequested = true;
}

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
			StaticJsonDocument<200> filter;//200 = estimation au pifomètre
			filter["assets"][0]["name"] = true;
			filter["assets"][0]["browser_download_url"] = true;
			filter["assets"][0]["size"] = true;

  			DynamicJsonDocument doc(2000);//2000 = estimation au doigt mouillé
			deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
			http.end();

			for(int i = 0; i < doc["assets"].size(); ++i){
				if( doc["assets"][i]["name"].as<String>().equals(OM_DEFAULT_BINARY_FILE_NAME)){

					Serial.println( "Begin downloading of -> " + doc["assets"][i]["browser_download_url"].as<String>() );

					HTTPClient http2;
					//http2.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS); // code 302
					//http2.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // code 404
					http2.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS); // code 404
					http2.begin(doc["assets"][i]["browser_download_url"].as<const char*>());

					httpCode = http2.GET();
					if(httpCode == HTTP_CODE_OK) {
						if (!Update.begin(http2.getSize(), U_FLASH)) {
							Update.printError(Serial);
						}
						int initialSize = http2.getSize();
						Serial.println( "lets go" );
						int finalSize = Update.writeStream(http2.getStream());
						http2.end();

						Serial.print( "initialSize : " );
						Serial.println( initialSize );
						Serial.print( "finalSize : " );
						Serial.println( finalSize );
						if (finalSize == initialSize) {
							Serial.println("Written : " + String(finalSize) + " successfully");
						} else {
							Serial.println("Written only : " + String(finalSize) + "/" + String(initialSize) + ". Retry?" );

						}
						if (Update.end()) {
							Serial.println("OTA done!");
							if (Update.isFinished()) {
							Serial.println("Update successfully completed. Rebooting.");
							ESP.restart();
							} else {
							Serial.println("Update not finished? Something went wrong!");
							}
						} else {
							Serial.println("Error Occurred. Error #: " + String(Update.getError()));
						}

					}else{
						Serial.printf("[HTTP] GET... code: %d\n", httpCode);
					}

					break;
				}
			}


		}
	} else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}
	OMAutoUpdater::isUpdateRequested = false;
}