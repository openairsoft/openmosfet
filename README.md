# openmosfet

OpenMosfet is a high quality open source airsoft mosfet based on arduino (currently esp32 board).

The software is meant to be modular and to allow different sensors configurations.

The hardware provides all the electronics needed and allows different boards to be created.

## the current team
The team is composed of a programmer ([Simon](https://github.com/simonjamain)) and two electronitians (Romain & Marcellin), all french airsofters.

## installation
- Install platformio
- "open" the project
- for building the ui (mandatory) :
  - install node.js
  - inside the `ui` folder, run `npm install`
  - *Note : `ui.h` is recompiled by running the command `npm run build` inside the `ui` folder. this is done automaticaly before each platformio build (see platformio.ini and build_ui.py)*
- I think thats about it...

## hardware

## contribute
Don't hesitate to contact us via github, or your can come and talk with us on our [discord server](https://discord.gg/XuzSSbgE).
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
- ~~change max ssid and password length to 32 chars instead of 16 : https://serverfault.com/questions/45439/what-is-the-maximum-length-of-a-wifi-access-points-ssid~~
- add "try to connect now" button next to external wifi settings
- check if wifi cutoff current saving is reliable

