# openmosfet

OpenMosfet is a high quality open source airsoft mosfet based on arduino (currently esp32 board).

The software is meant to be modular and to allow different sensors configurations.

The hardware provides all the electronics needed and allows different boards to be created.

## the current team
The team is composed of a programmer ([Simon](https://github.com/simonjamain)) and two electronitians (Romain & Marcellin), all french airsofters.

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
    - AsyncElegantOTA (https://github.com/ayushsharma82/AsyncElegantOTA)
- (Optional, for uploading settings json file through arduino IDE) Install the SPIFFS data upload tool : https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/

### compiling the ui (in case of )
For reasons, the ui files (html, css,js, etc... ) are compiled in `ui.h` file.
If you wish to edit the ui, you have to go through the (straightforward) process of building the `ui.h` file:

- (Optional) for developping the UI
  you will need node.js to compile the html/css/js files into a .h file
  - install node.js
  - inside the `ui` folder, run `npm install`
  - after each change on the ui files, recompile `ui.h` by running the command `npm run build` (again inside the `ui` folder)
  - you are then ready to upload the code to arduino or generate the `.bin` file

## hardware

## contribute
Don't hesitate to contact us via github, or your can come and talk with us on our [discord server](https://discord.gg/ppGzhn).
We are french but we can chat in english no prob.

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

