
[![Open Source? Yes!](https://badgen.net/badge/Open%20Source%20%3F/Yes%21/green?icon=github)](https://github.com/simonjamain/openmosfet)
[![Maintenance](https://badgen.net/badge/Maintained%20%3F/Yes%21/green?icon=github)](https://github.com/simonjamain/openmosfet/graphs/commit-activity)
![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/simonjamain/openmosfet)

<br/>
<p align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="doc/assets/images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">OpenMosfet</h3>

  <p align="center">
    An high quality open source <strong>airsoft mosfet</strong><br> based on arduino.
    <br/><br/>
    <a href="#"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/simonjamain/openmosfet/issues">Report Bug</a>
    ·
    <a href="https://github.com/simonjamain/openmosfet/issues">Request Feature</a>
  </p>
</p>

<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li>
          <a href="#the-team">The team</a>
          <ul>
            <li><a href="#developers">Developers</a></li>
          </ul>
        </li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#installation-and-build">Installation and build</a></li>
        <li><a href="#replica-interface-types">Replica interface types</a></li>
        <li><a href="#custom-replica-interface-pinout">Custom replica interface pinout</a></li>
      </ul>
    </li>
    <li>
      <a href="#development">Development</a>
      <ul>
        <li>
          <a href="#local-ui-without-board">Local UI (without board)</a>
          <ul>
            <li><a href="#install-and-serve">Install and serve</a></li>
          </ul>
        </li>
      </ul>
    </li>
    <li><a href="#usage">Development</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#todo">To-Do</a></li>
  </ol>
</details>

## About the project


OpenMosfet is an high quality open source airsoft mosfet based on arduino (currently esp32 board).

The software is meant to be modular and to allow different sensors configurations.

The hardware provides all the electronics needed and allows different boards to be created.

The web dashboard allows full and easy controls over every settings as well an update manager.

![OpenMosfet Web Dashboard](doc/assets/images/ui.png)

### The team


The team is composed of two developers ([Simon](https://github.com/simonjamain) and [Zao Soula](https://github.com/zaosoula)) and two electronitians (Romain and Marcellin), all french airsofters.

#### Developers
<div style="display: flex; justify-content: space-around">
  <div align="center">
    <a href="https://github.com/simonjamain">
      <img src="https://avatars1.githubusercontent.com/u/6118295?v=4?s=100" width="100px;" alt=""/>
    <br /><sub><b>simonjamain</b></sub></a>
    </div>
    <div align="center">
      <a href="https://www.zaosoula.fr/">
        <img src="https://avatars2.githubusercontent.com/u/1884246?v=4?s=100" width="100px;" alt=""/>
        <br /><sub><b>Zao Soula</b></sub>
        </a>
    </div>
</div>


## Getting started
### Installation and build
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

### Replica interface types
Depending on your replica and the way you connect the mosfet, the signals received are not the same.

ex : the original input plate on the G&G TR 16 can be used but the signals are very specific.
- You only know if the safety is disengaged (and mag not empty) when you get a signal from the trigger
- You only know if selector is on full auto or not before firing

You have to choose which behavior you want by specify the value of the `REPLICA_TYPE` preprocessor flag (see platformio.ini).

Here is a table (work in progress) listing the different configuration available :

`REPLICA_TYPE` value | pins behavior | examples
---------------------|---------------|--------------
1 | `OM_DEFAULT_FIRINGGROUP_PIN` grounded when : trigger is pressed AND safety is off AND mag is not empty<br>`OM_DEFAULT_CUTOFF_PIN` grounded when cutoff is pressed<br>`OM_DEFAULT_SELECTOR_PIN` grounded when selector is on full auto position | G&G tr16 original input plate

### Custom replica interface pinout
For developpement reasons or other, you can change the default settings for the replica interface with the following flags :

- `OM_DEFAULT_MOTOR_PIN`
- `OM_DEFAULT_FIRINGGROUP_PIN`
- `OM_DEFAULT_CUTOFF_PIN`
- `OM_DEFAULT_SELECTOR_PIN`

*Note: it is not recommended that you change this value in the source file, but rather as a build flag in platformio.ini for example.*  
*Note: for example, the ESP32DEVKIT V1 doesnt have the pin 10 exposed, which is the default pin used on the board for the selector input.*

## Development

### Local UI (without board)
To serve the UI locally on your computer, we provide a fake server to simulate the board behavior.

#### Install and serve
  - inside the `/src/ui/` folder, run `npm install`
  - run `npm run serve-dev`
  - open http://localhost:3000/ in your browser
  - you can now edit ``src/`` without having to re-launch the server

*Note: The configuration is loaded from ``defaultConf.json`` and can be updated as long as the server is running.
Once it stops the updated configuration is lost*

## Contributing
Don't hesitate to contact us via github, or your can come and talk with us on our [discord server](https://discord.gg/XuzSSbgE).
We are french but we can chat in english without problems.

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