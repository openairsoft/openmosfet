# openmosfet

OpenMosfet is a high quality open source airsoft mosfet based on arduino (currently esp32 board).

The software is meant to be modular and to allow different sensors configurations.

The hardware provides all the electronics needed and allows different boards to be created.

## the current team
The team is composed of a programmer ([Simon](https://github.com/simonjamain)) and two electronitians (Romain & Marcellin), all french airsofters.

## installation and build
- Install platformio
- "open" the project
- for building the ui (mandatory) :
  - install node.js
  - inside the `ui` folder, run `npm install`
  - *Note : `ui.h` is recompiled by running the command `npm run build` inside the `ui` folder. this is done automaticaly before each platformio build (see platformio.ini and build_ui.py)*
- OTA upload is enabled by default, if your board hasnt ota enabled or you want to upload with serial, uncomment that following parameters in `platformio.ini`
  - `upload_protocol`
  - `upload_port`
- I think thats about it...

### replica interface types
Depending on your replica and the way you connect the mosfet, the signals received are not the same.

ex : the original input plate on the G&G TR 16 can be used but the signals are very specific.
- You only know if the safety is disengaged (and mag not empty) when you get a signal from the trigger
- You only know if selector is on full auto or not before firing

You have to choose which behavior you want by specify the value of the `REPLICA_TYPE` preprocessor flag (see platformio.ini).

Here is a table (work in progress) listing the different configuration available :

`REPLICA_TYPE` value | pins behavior | examples
---------------------|---------------|--------------
1 | `OM_DEFAULT_FIRINGGROUP_PIN` grounded when : trigger is pressed AND safety is off AND mag is not empty<br>`OM_DEFAULT_CUTOFF_PIN` grounded when cutoff is pressed<br>`OM_DEFAULT_SELECTOR_PIN` grounded when selector is on full auto position | G&G tr16 original input plate

### custom replica interface pinout
For developpement reasons or other, you can change the default settings for the replica interface with the following flags :

- `OM_DEFAULT_MOTOR_PIN`
- `OM_DEFAULT_FIRINGGROUP_PIN`
- `OM_DEFAULT_CUTOFF_PIN`
- `OM_DEFAULT_SELECTOR_PIN`

*Note: it is not recommended that you change this value in the source file, but rather as a build flag in platformio.ini for example.*  
*Note: for example, the ESP32DEVKIT V1 doesnt have the pin 10 exposed, which is the default pin used on the board for the selector input.*

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

