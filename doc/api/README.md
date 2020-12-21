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

- **Trigger state** : `/api/components/trigger/state`
  - `GET,POST` : set/read the trigger state
    - parameters *application/json*
      - *boolean* : [true, false] true for pulling the trigger, false for releasing
- **Trigger short pull** : `/api/components/trigger/bump`
  - `POST` : pull and release the trigger
- **Selector state** : `/api/components/selector/state`
  - `GET,POST` : set/read the selector state
    - parameters *application/json*
      - *int* : [`0`, `1`, `2`]
- **Gearbox uncocking** : `/api/components/gearbox/uncock`
  - `POST` : uncock the piston (if cocked) ⚠Be careful, this will eventully fire a BB⚠

--------------

*inspired by https://github.com/jamescooke/restapidocs/*