#if REPLICA_TYPE == 1

#include "../inputsInterface.h"
#include "../config.h"
#include "../components.h"
#include "../configuration.h"

#include <Bounce2.h>

uint8_t motorPin = OM_DEFAULT_MOTOR_PIN;
uint8_t firingGroupPin = OM_DEFAULT_FIRINGGROUP_PIN;
uint8_t cutoffPin = OM_DEFAULT_CUTOFF_PIN;
uint8_t selectorPin = OM_DEFAULT_SELECTOR_PIN;

#define GGTR16_DEBOUNCE_TIME_MS 3

//channels
#define GGTR16_MOTOR_LEDC_CHANNEL 1
#define GGTR16_MOTOR_LEDC_FREQ 12000
#define GGTR16_MOTOR_LEDC_RES 8

Bounce triggerDebouncer = Bounce();
Bounce cutoffDebouncer = Bounce();
Bounce selectorDebouncer = Bounce();

void OMInputsInterface::begin()
{
  // motor pwm setup
  ledcAttachPin(motorPin, GGTR16_MOTOR_LEDC_CHANNEL);
  ledcSetup(GGTR16_MOTOR_LEDC_CHANNEL, GGTR16_MOTOR_LEDC_FREQ, GGTR16_MOTOR_LEDC_RES);
  ledcWrite(GGTR16_MOTOR_LEDC_CHANNEL, 0); //turn off motor
  
  pinMode(motorPin, OUTPUT);

  triggerDebouncer.attach(firingGroupPin,INPUT_PULLUP);
  triggerDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);
  
  cutoffDebouncer.attach(cutoffPin,INPUT_PULLUP);
  cutoffDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);

  selectorDebouncer.attach(selectorPin,INPUT_PULLUP);
  selectorDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);

  if(digitalRead(selectorPin) == LOW)
  {
    OMVirtualSelector::setState(OMVirtualSelector::stateAuto);
  }else
  {
    OMVirtualSelector::setState(OMVirtualSelector::stateSemi);//NOTE: here we have no mean to know when selector is on safe, it just physicaly disable the firing group, so we have to initialize on semi.
  }

}

void OMInputsInterface::update()
{
  triggerDebouncer.update();
  cutoffDebouncer.update();
  selectorDebouncer.update();
   
  if( triggerDebouncer.fell()) {
    OMVirtualTrigger::pull();
  }else if( triggerDebouncer.rose()) {
    OMVirtualTrigger::release();
  }
   
  if( cutoffDebouncer.fell()) {
    OMVirtualGearbox::cycleEndDetected();
  }

  if( selectorDebouncer.fell()) {
    OMVirtualSelector::setState(OMVirtualSelector::stateAuto);
  }else if( selectorDebouncer.rose()) {
    OMVirtualSelector::setState(OMVirtualSelector::stateSemi);
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
  ledcWrite(GGTR16_MOTOR_LEDC_CHANNEL, 255);
}

void OMInputsInterface::motorOff()
{
  ledcWrite(GGTR16_MOTOR_LEDC_CHANNEL, 0);
}

#endif
