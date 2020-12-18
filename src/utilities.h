#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index);

// https://github.com/espressif/arduino-esp32/blob/1.0.4/libraries/HTTPUpdate/examples/httpUpdateSecure/httpUpdateSecure.ino
void setClock();

#ifndef MILLIS_TO_SEC
 #define MILLIS_TO_SEC(A)     ((A)/1000)
#endif

#ifndef SEC_TO_MILLIS
 #define SEC_TO_MILLIS(A)     ((A)*1000)
#endif

#endif
