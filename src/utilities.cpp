#include "utilities.h"
#include <time.h>

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
// String getValue(String data, char separator, int index)
// {
//   int found = 0;
//   int strIndex[] = {0, -1};
//   int maxIndex = data.length()-1;

//   for(int i=0; i<=maxIndex && found<=index; i++){
//     if(data.charAt(i)==separator || i==maxIndex){
//         found++;
//         strIndex[0] = strIndex[1]+1;
//         strIndex[1] = (i == maxIndex) ? i+1 : i;
//     }
//   }

//   return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
// }

// int getFollowRedirect(HTTPClient &http, const int maxRedirects = 4){
//   http.begin(OM_DEFAULT_GITHUB_API_LATEST_RELEASE_URL); //HTTP

// 	Serial.print("[HTTP] GET...\n");
// 	// start connection and send HTTP header
// 	int httpCode = http.GET();

// 	// httpCode will be negative on error
// 	if(httpCode > 0) {
// 		// HTTP header has been send and Server response header has been handled
// 		Serial.printf("[HTTP] GET... code: %d\n", httpCode);

// 		// file found at server
// 		if(httpCode == HTTP_CODE_OK) {
//   }
// }