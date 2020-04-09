# openmosfet

OpenMosfet is a high quality open source airsoft mosfet based on arduino (currently esp32 board).

The software is meant to be modular and to allow different sensors configuration.

The hardware provides all the electronics needed and allows different boards to be created.


## installation
- Install arduino IDE https://www.arduino.cc/en/main/software.
- add `https://dl.espressif.com/dl/package_esp32_index.json` in **File > Preferences > Additional Boards Manager URLs**
- go to **Tools > Board > Boards Manager** and add the **esp32** board
- Install required libraries
  - via arduino IDE libraby manager (**Sketch > Library > Manage Libraries**)
    - Bounce2
    - ArduinoJson
  - manually (consult the internet for howto)
    - ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
    - AsyncTCP (https://github.com/me-no-dev/AsyncTCP)
- Install the littleFs data upload tool : https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases
## hardware
## contribute
## TODO
- ~~launch in AP mode when wifi router is not available~~
- ~~implement sleep modes to prevent battery over-discharge~~
    - ~~ability to cut wifi off after X sec to save power~~
    - ~~deep sleep when inactive for two long~~
- add buzzer ?
- implement motor vibration
- create an example module
- implement all battery protection types :
  - auto
  - predefined
  - manual
  - none
- change max ssid and password length to 32 chars instead of 16 : https://serverfault.com/questions/45439/what-is-the-maximum-length-of-a-wifi-access-points-ssid
- add "try to connect now" button next to external wifi settings
- check if wifi cutoff current economy is reliable

