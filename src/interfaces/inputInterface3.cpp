#if REPLICA_TYPE == 3

#include "../inputsInterface.h"
#include "../config.h"
#include "../components.h"
#include "../configuration.h"

#include <Bounce2.h>

uint8_t motorPin = OM_DEFAULT_MOTOR_PIN;
uint8_t firingGroupPin = OM_DEFAULT_FIRINGGROUP_PIN;
uint8_t cyclePin = OM_DEFAULT_CYCLE_PIN;
uint8_t selectorPin = OM_DEFAULT_SELECTOR_PIN;


//channels
#define MOTOR_LEDC_FREQ 12000
#define MOTOR_LEDC_RES 8

#define OM_DELAY_BETWEEN_SELECTOR_CHANGE_MS 200 //remove a bit of noise, also important for esp-now


unsigned int nextSelectorStateAllowChange_ms = 0;

Bounce triggerDebouncer = Bounce();
Bounce cutoffDebouncer = Bounce();

void OMInputsInterface::begin()
{
  // motor pwm setup
  ledcAttachPin(motorPin, OM_INTERFACE_PWM_CHANNEL_1);
  ledcSetup(OM_INTERFACE_PWM_CHANNEL_1, MOTOR_LEDC_FREQ, MOTOR_LEDC_RES);
  ledcWrite(OM_INTERFACE_PWM_CHANNEL_1, 0); //turn off motor
  
  pinMode(motorPin, OUTPUT);

  triggerDebouncer.attach(firingGroupPin,INPUT_PULLUP);
  triggerDebouncer.interval(OM_DEBOUNCE_TIME_MS);
  
  cutoffDebouncer.attach(cyclePin,INPUT_PULLUP);
  cutoffDebouncer.interval(OM_DEBOUNCE_TIME_MS);

  OMVirtualSelector::setState(OMVirtualSelector::stateSafe);
}

void OMInputsInterface::update()
{
  triggerDebouncer.update();
  cutoffDebouncer.update();
   
  if( triggerDebouncer.fell()) {
    OMVirtualTrigger::pull();
  }else if( triggerDebouncer.rose()) {
    OMVirtualTrigger::release();
  }
   
  if( cutoffDebouncer.rose()) {
    OMVirtualGearbox::cycleEndDetected();
  }

  // cycling throught modes on selector press
  if(OMConfiguration::isSelectorCalibrated())
  {
    int selectorValue =  analogRead(selectorPin);
    int threshold1 = (OMConfiguration::selectorCalibration[0] + OMConfiguration::selectorCalibration[1]) / 2;
    int threshold2 = (OMConfiguration::selectorCalibration[1] + OMConfiguration::selectorCalibration[2]) / 2;

    if(millis() > nextSelectorStateAllowChange_ms) {
      if( selectorValue < threshold1) {
        if(OMVirtualSelector::getState() != OMVirtualSelector::stateSafe) {
          OMVirtualSelector::setState(OMVirtualSelector::stateSafe);
          nextSelectorStateAllowChange_ms = millis() + OM_DELAY_BETWEEN_SELECTOR_CHANGE_MS;
        }
      } else if (selectorValue < threshold2) {
        if(OMVirtualSelector::getState() != OMVirtualSelector::stateSemi) {
          OMVirtualSelector::setState(OMVirtualSelector::stateSemi);
          nextSelectorStateAllowChange_ms = millis() + OM_DELAY_BETWEEN_SELECTOR_CHANGE_MS;
        }
      } else {
        if(OMVirtualSelector::getState() != OMVirtualSelector::stateAuto) {
          OMVirtualSelector::setState(OMVirtualSelector::stateAuto);
          nextSelectorStateAllowChange_ms = millis() + OM_DELAY_BETWEEN_SELECTOR_CHANGE_MS;
        }
      }
    }
    
  }
}

OMFiringSettings &OMInputsInterface::getCurrentFiringSetting()
{
  switch(OMVirtualSelector::getState())
  {
    case OMVirtualSelector::stateAuto:
      return OMConfiguration::fireModes[1];
      break;

    case OMVirtualSelector::stateSafe://NOTE: in an ideal world, this should raise an exception, but meh...
    case OMVirtualSelector::stateSemi:
    default:
      return OMConfiguration::fireModes[0];
  }
}

void OMInputsInterface::motorOn()
{
  ledcWrite(OM_INTERFACE_PWM_CHANNEL_1, 255);
}

void OMInputsInterface::motorOff()
{
  ledcWrite(OM_INTERFACE_PWM_CHANNEL_1, 0);
}

//this interface do not wrok with analog slector values
float OMInputsInterface::getSelectorCalibrationValue()
{
  return analogRead(selectorPin);
  // return selectorPin;
}
#endif
