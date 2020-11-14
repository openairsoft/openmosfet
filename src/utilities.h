#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index);

#ifndef MILLIS_TO_SEC
 #define MILLIS_TO_SEC(A)     ((A)/1000)
#endif

#ifndef SEC_TO_MILLIS
 #define SEC_TO_MILLIS(A)     ((A)*1000)
#endif

#endif
