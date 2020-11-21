#include "config.h"
#include "autoUpdater.h"
#include "utilities.h"
#include "wifiServer.h"

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

	HTTPClient httpGithubApiClient;

	httpGithubApiClient.begin(OM_DEFAULT_GITHUB_API_LATEST_RELEASE_URL); //HTTP
	int httpCodeGithubApi = httpGithubApiClient.GET();

	// httpCodeGithubApi will be negative on error
	if(httpCodeGithubApi > 0) {
		// HTTP header has been send and Server response header has been handled
		
		#ifdef DEBUG
		Serial.printf("[HTTP] GET... code: %d\n", httpCodeGithubApi);
		#endif

		// file found at server
		if(httpCodeGithubApi == HTTP_CODE_OK) {

			// The filter: it contains "true" for each value we want to keep
			StaticJsonDocument<200> filter;//200 = estimation au pifomètre
			filter["assets"][0]["name"] = true;
			filter["assets"][0]["browser_download_url"] = true;
			filter["assets"][0]["size"] = true;

  			DynamicJsonDocument doc(2000);//2000 = estimation au doigt mouillé
			deserializeJson(doc, httpGithubApiClient.getStream(), DeserializationOption::Filter(filter));
			httpGithubApiClient.end();

			//we go through each asset to find the right one and download it
			String assetUrl;
			for(int currentAssetIndex = 0; currentAssetIndex < doc["assets"].size(); ++currentAssetIndex){
				if( doc["assets"][currentAssetIndex]["name"].as<String>().equals(OM_DEFAULT_BINARY_FILE_NAME)){

					#ifdef DEBUG
					Serial.println( "Begin downloading of -> " + doc["assets"][currentAssetIndex]["browser_download_url"].as<String>() );
					#endif

					assetUrl = doc["assets"][currentAssetIndex]["browser_download_url"].as<String>();

					//github api assets uses 302 redirections, we cannot just get the first page
					//currently setFollowRedirects does'nt work for me, until it eventually does, we'll handle redirects manually
					const int maxRedirects = 4;
					for(int redirectCounter = 1; redirectCounter <= maxRedirects; ++redirectCounter){					
						HTTPClient httpReleaseAssetClient;
						//httpReleaseAssetClient.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS); // code 302
						//httpReleaseAssetClient.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // code 404
						//httpReleaseAssetClient.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS); // code 404
						httpReleaseAssetClient.begin(assetUrl);

						//we declare which headers we want to keep (for redirects)
						const char * headerKeys[] = {"Location"} ;
						size_t headerKeysSize = sizeof(headerKeys)/sizeof(char*);
						httpReleaseAssetClient.collectHeaders(headerKeys, headerKeysSize);

						int httpCodeReleaseAsset = httpReleaseAssetClient.GET();

						
						if(httpCodeReleaseAsset == HTTP_CODE_MOVED_PERMANENTLY || httpCodeReleaseAsset == HTTP_CODE_FOUND){
							assetUrl = httpReleaseAssetClient.header("Location");// this is a redirection, store the new url and make another round-trip
							httpReleaseAssetClient.end();
						}
						else if(httpCodeReleaseAsset == HTTP_CODE_OK) {
							if (!Update.begin(httpReleaseAssetClient.getSize(), U_FLASH)) {
								#ifdef DEBUG
								Update.printError(Serial);
								#endif
								break;
							}
							int initialSize = httpReleaseAssetClient.getSize();
							// int sizeLeftToUpdate = initialSize;
      						OMwifiserver::events.send("","updateStarted",millis());
							#ifdef DEBUG
							Serial.println( "Starting update" );
							#endif
							int finalSize = 0;
							
							finalSize = Update.writeStream(httpReleaseAssetClient.getStream());
							httpReleaseAssetClient.end();
							//I would like to show progress with this and https://github.com/me-no-dev/ESPAsyncWebServer#async-event-source-plugin
							/*
							WiFiClient* stream = httpReleaseAssetClient.getStreamPtr();
							
							// inspired by https://github.com/esp8266/Arduino/issues/4010
							uint8_t buff[1024] = { 0 };
  							size_t sizePack;
							while(httpReleaseAssetClient.connected() && sizeLeftToUpdate > 0 && stream->available() > 0){
								sizePack = stream->available();
								if (sizePack) {
								int c = stream->readBytes(buff, ((sizePack > sizeof(buff)) ? sizeof(buff) : sizePack));
								int bytesWritten = Update.write(buff, c);
								if (sizeLeftToUpdate > 0)
									sizeLeftToUpdate -= c;
									finalSize += bytesWritten;
								}

								
								//n'envoyer la notif de progression (ou la stocker) que tout les x secondes et/ou pourcents
								// if (progress != int(Update.progress() * 100 / httpClient.getSize())) {
								// 	progress = int(Update.progress() * 100 / httpClient.getSize());
								// }

								Serial.print("sizeof(buff): ");
								Serial.println(sizeof(buff));
								Serial.print("sizePack: ");
								Serial.println(sizePack);
								Serial.print("Update.progress(): ");
								Serial.println(finalSize);
							}
							*/

							if (Update.end() && finalSize == initialSize) {
								if (Update.isFinished()) {
									#ifdef DEBUG
									Serial.println("Update successfully completed. Rebooting.");
									#endif
									OMwifiserver::events.send("","updateSuccessful",millis());
									delay(500);//"ensure" that event is fully sent
									ESP.restart();
								} else {
									#ifdef DEBUG
									Serial.println("Update not finished? Something went wrong!");
									#endif
								}
							} else {
      							OMwifiserver::events.send("","updateFailed",millis());
								#ifdef DEBUG
								Serial.println("Error Occurred. Error #: " + String(Update.getError()));
								#endif
								break;
							}
							break;
						}else{
      						OMwifiserver::events.send("","updateFailed",millis());
							break;
						}
					}
				}
			}


		}
	} else {
		#ifdef DEBUG
		Serial.printf("[HTTP] GET... failed, error: %s\n", httpGithubApiClient.errorToString(httpCodeGithubApi).c_str());
		#endif
      	OMwifiserver::events.send("","updateFailed",millis());
	}
	OMAutoUpdater::isUpdateRequested = false;
}