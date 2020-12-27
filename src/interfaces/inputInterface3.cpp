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
#define MOTOR_LEDC_CHANNEL 1
#define MOTOR_LEDC_FREQ 12000
#define MOTOR_LEDC_RES 8

Bounce triggerDebouncer = Bounce();
Bounce cutoffDebouncer = Bounce();

void OMInputsInterface::begin()
{
  // motor pwm setup
  ledcAttachPin(motorPin, MOTOR_LEDC_CHANNEL);
  ledcSetup(MOTOR_LEDC_CHANNEL, MOTOR_LEDC_FREQ, MOTOR_LEDC_RES);
  ledcWrite(MOTOR_LEDC_CHANNEL, 0); //turn off motor
  
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

    if( selectorValue < threshold1 && OMVirtualSelector::getState() != OMVirtualSelector::stateSafe) {
      OMVirtualSelector::setState(OMVirtualSelector::stateSafe);
    } else if (selectorValue < threshold2 && OMVirtualSelector::getState() != OMVirtualSelector::stateSemi) {
      OMVirtualSelector::setState(OMVirtualSelector::stateSemi);
    } else if(OMVirtualSelector::getState() != OMVirtualSelector::stateAuto){
      OMVirtualSelector::setState(OMVirtualSelector::stateAuto);
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
  ledcWrite(MOTOR_LEDC_CHANNEL, 255);
}

void OMInputsInterface::motorOff()
{
  ledcWrite(MOTOR_LEDC_CHANNEL, 0);
}

//this interface do not wrok with analog slector values
float OMInputsInterface::getSelectorCalibrationValue()
{
  return analogRead(selectorPin);
  // return selectorPin;
}
#endif
