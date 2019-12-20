#ifndef UTILITIES_H
#define UTILITIES_H

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#ifndef MILLIS_TO_SEC
 #define MILLIS_TO_SEC(A)     ((A)/1000)
#endif

#ifndef SEC_TO_MILLIS
 #define SEC_TO_MILLIS(A)     ((A)*1000)
#endif

#endif
