## Endpoints

For now, all endpoints doesn't require any Authentication.

### Firmware

- **Updates** : `/api/core/update/`
  - `POST` : auto-update replica from the github releases  
  *Note: be carefull of timeout, this usually take 70-80 secs*
  - `PUT` : upload a .bin to update the firmware
    - parameters *multipart/form-data*
      - `firmware` *application/octet-stream* : the binary file
- **Firmware version** : `/api/core/version/`
  - `GET` : 
    - parameters *application/json*
      - *string* : the current version (semver, see [releases](https://github.com/simonjamain/openmosfet/releases))

### Configuration

This is closely linked to [configuration.h](/src/configuration.h)

- **global configuration** : `/api/config/`
  - `GET` : reads the global configuration 
    - parameters *application/json*
      - `appSsid` *string [8-32]* : Access point ssid
      - `appPasswd` *string [8-16]* : Access point password
      - `connectToNetworkIfAvailable` *boolean* : Will try to connect at startup
      - `availableNetworkAppSsid` *string [8-32]* : External network ssid
      - `availableNetworkAppPasswd` *string [8-16]* : External network password
      - `enableEspNow` *boolean* : Enable communication with esp-now protocol
      - `wifiShutdownDelayMinutes` *int* : delay for wifi shutdown after last network activity (0 = never shuts down)
      - `deepSleepDelayMinutes` *int* : delay for sleep mode after replica inactivity
      - `disableMotor` *boolean* : Motor won't run if this is true (safety feature)
      - `enableBatteryProtection` *boolean* : Will monitor battery voltage
      - `batteryNominalVoltage` *float* : Honestly I don't precisely know yet what to do with that
      - `batteryLowVoltage` *float* : Will alert the user below this voltage
      - `batteryShutdownVoltage` *float* : Will disable the replica below this voltage
      - `enableActiveBreaking` *boolean* : Duh.
      - `enablePrecocking` *boolean* : Enable precocking based on values specified for each firemode
      - `selectorCalibration` *array* [READONLY]: The calibration of the selector optic sensor (update the values via the `/api/components/selector/state` endpoint)
        - [0] *int* [0-4095] : value for selector position 1 or negative value if not calibrated (divide by 1241 to obtain real voltage)
        - [1] *int* [0-4095] : value for selector position 2 or negative value if not calibrated (divide by 1241 to obtain real voltage)
        - [2] *int* [0-4095] : value for selector position 3 or negative value if not calibrated (divide by 1241 to obtain real voltage)
      - `fireModes` *object array* :
        - `burstMode` *int [0-2]* : Burst mode, 0=interruptible, 1=normal, 2=extendible
        - `burstLength` *int* :  
          normal burstmode : Number of shot fired when trigger pulled  
          interruptible burstmode : Maximum number of shot fired during trigger pull  
          extendible burstmode : Minimum number of shot fired when trigger pulled
        - `precockDuration_ms` *int* : Amount of precocking time after cycle in milliseconds (may be replaced with a %)
        - `motorPower` *float [0-1]* : Percentage of battery power delivered to the motor (with pwm)
        - `timeBetweenShots_ms` : Time between each shots, can be used for RoF control or dmr setup
        - `decockAfter_s` *float* : If the trigger is maintained pressed for set ammount of time after end of burst, cycle and decock ⚠this fires a bb⚠. (0 = no decocking)
  - `POST` : Set the global configuration and retrieve the saved values in response
    - parameters *application/json*  
    same data format as `GET`

### Network

- **ESP-NOW pairing** : `/api/network/espnow/pair`
  - `POST` : instantly scan and auto-pair with all available esp-now modules (see: https://github.com/openairsoft/openmosfet_esp-now)

### Replica
 
This is closely linked to [components.h](/src/components.h)

[comment]: # (todo: a method to get all the components state in one request)

[comment]: # (todo: uniformize Selector state api values, include both voltage and state)

- **Trigger state** : `/api/components/trigger/state`
  - `GET,POST` : reads/sets the trigger state
    - return values/input parameters *application/json*
      - *boolean* : [true, false] true for pulling the trigger, false for releasing
- **Trigger short pull** : `/api/components/trigger/bump`
  - `POST` : pull and release the trigger
- **Selector state** : `/api/components/selector/state`
  - `GET,POST` : respectively reads or sets the selector state
    - return values/input parameters *application/json*
      - *int* : [`0`, `1`, `2`]
  - `PATCH` : calibrates the sensor for a given state (`PATCH`)
    - input parameters *application/json*
      - *int* : [`0`, `1`, `2`] the current position of the selector
    - return value :
      - *float* : voltage read from the sensor and stored in the configuration
    - highlighted return codes :
      - `206` : value accepted but calibration incomplete (other positions still needs calibrating)
      - `200` : value accepted and calibration complete
- **Gearbox state** : `/api/components/gearbox/state`
  - `GET,POST` : reads/sets the gearbox state, this can be used to reset the gearbox state after an error
    - return values/input parameters *application/json*
      - *int* : [`0`: resting, `1`: precocked, `2`: cycling, `3`: error]
- **Gearbox uncocking** : `/api/components/gearbox/uncock`
  - `POST` : uncock the piston (if cocked) ⚠Be careful, this will eventully fire a BB⚠

--------------

*inspired by https://github.com/jamescooke/restapidocs/*