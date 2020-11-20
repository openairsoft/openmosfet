#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>
#include <HTTPClient.h>

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index);

// https://github.com/espressif/arduino-esp32/blob/1.0.4/libraries/HTTPUpdate/examples/httpUpdateSecure/httpUpdateSecure.ino
void setClock();

/**
 * Use this instead of HTTPClient.get() to follow redirections autimatically
 * This will soon be obsolete with: https://github.com/espressif/arduino-esp32/commit/ee88c42c3b5e87a2fc09303f9e82dd8172e99990
 */
//int getFollowRedirect(HTTPClient &http, const int maxRedirects = 4);

#ifndef MILLIS_TO_SEC
 #define MILLIS_TO_SEC(A)     ((A)/1000)
#endif

#ifndef SEC_TO_MILLIS
 #define SEC_TO_MILLIS(A)     ((A)*1000)
#endif

#endif
