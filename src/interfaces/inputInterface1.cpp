#if REPLICA_TYPE == 1

#include "../inputsInterface.h"
#include "../config.h"
#include "../components.h"
#include "../configuration.h"

#include <Bounce2.h>

uint8_t ggtr16MotorPin = OM_DEFAULT_MOTOR_PIN;
uint8_t ggtr16FiringGroupPin = OM_DEFAULT_FIRINGGROUP_PIN;
uint8_t ggtr16CutoffPin = OM_DEFAULT_CUTOFF_PIN;
uint8_t ggtr16SelectorPin = OM_DEFAULT_SELECTOR_PIN;

#define GGTR16_DEBOUNCE_TIME_MS 5

//channels
#define GGTR16_MOTOR_LEDC_CHANNEL 1
#define GGTR16_MOTOR_LEDC_FREQ 12000
#define GGTR16_MOTOR_LEDC_RES 8

Bounce ggtr16TriggerDebouncer = Bounce();
Bounce ggtr16CutoffDebouncer = Bounce();
Bounce ggtr16SelectorDebouncer = Bounce();

void OMInputsInterface::begin(OMVirtualReplica &replica)
{
  // motor pwm setup
  ledcAttachPin(ggtr16MotorPin, GGTR16_MOTOR_LEDC_CHANNEL);
  ledcSetup(GGTR16_MOTOR_LEDC_CHANNEL, GGTR16_MOTOR_LEDC_FREQ, GGTR16_MOTOR_LEDC_RES);
  ledcWrite(GGTR16_MOTOR_LEDC_CHANNEL, 0); //turn off motor
  
  pinMode(ggtr16MotorPin, OUTPUT);

  ggtr16TriggerDebouncer.attach(ggtr16FiringGroupPin,INPUT_PULLUP);
  ggtr16TriggerDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);
  
  ggtr16CutoffDebouncer.attach(ggtr16CutoffPin,INPUT_PULLUP);
  ggtr16CutoffDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);

  ggtr16SelectorDebouncer.attach(ggtr16SelectorPin,INPUT_PULLUP);
  ggtr16SelectorDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);

  if(digitalRead(ggtr16SelectorPin) == LOW)
  {
    replica.getSelector().setState(OMVirtualSelector::stateAuto);
  }else
  {
    replica.getSelector().setState(OMVirtualSelector::stateSemi);//NOTE: here we have no mean to know when selector is on safe, it just physicaly disable the firing group, so we have to initialize on semi.
  }

}

void OMInputsInterface::update(OMVirtualReplica &replica)
{
  ggtr16TriggerDebouncer.update();
  ggtr16CutoffDebouncer.update();
  ggtr16SelectorDebouncer.update();
   
  if( ggtr16TriggerDebouncer.fell()) {
    replica.getTrigger().pull();
  }else if( ggtr16TriggerDebouncer.rose()) {
    replica.getTrigger().release();
  }
   
  if( ggtr16CutoffDebouncer.fell()) {
    replica.getGearbox().cycleEndDetected();
  }

  if( ggtr16SelectorDebouncer.fell()) {
    replica.getSelector().setState(OMVirtualSelector::stateAuto);
  }else if( ggtr16SelectorDebouncer.rose()) {
    replica.getSelector().setState(OMVirtualSelector::stateSemi);
  }
}

OMFiringSettings &OMInputsInterface::getCurrentFiringSetting(OMVirtualSelector &selector)
{
  switch(selector.getState())
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
