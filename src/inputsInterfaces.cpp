#include "inputsInterface.h"
#include "config.h"
#include "components.h"
#include "configuration.h"

/**
 * README
 * 
 * Here are all the implementations of OMInputsInterface in accordance 
 * to you input board and your wiring.
 * 
 */


/**
 * This is an implementation for the input board included inside a G&G TR16
 */
// START G&G TR16 & P90 specific code ---------------------------------------
#if defined(REPLICA_TYPE_GGTR16) || defined(REPLICA_TYPE_CYMACGP90)

//#define OM_NB_ACTIVE_MODES 2

#define GGTR16_MOTOR_PIN 32// this is the builtin led pin
#define GGTR16_FIRINGGROUP_PIN 27
#define GGTR16_CUTOFF_PIN 12
#define GGTR16_SELECTOR_PIN 10
#define GGTR16_DEBOUNCE_TIME_MS 5

//channels
#define GGTR16_MOTOR_LEDC_CHANNEL 1
#define GGTR16_MOTOR_LEDC_FREQ 12000
#define GGTR16_MOTOR_LEDC_RES 8




//---------- X : Replica specific code 2 -----------------------------------------------------------------------------------------

#include <Bounce2.h>
Bounce ggtr16TriggerDebouncer = Bounce();
Bounce ggtr16CutoffDebouncer = Bounce();
Bounce ggtr16SelectorDebouncer = Bounce();

//AAMFiringSettings ggtr16FiringSettingsSemi(AAMFiringSettings::burstModeNormal, 1, 0, 255, 0);
//AAMFiringSettings ggtr16FiringSettingsAuto(AAMFiringSettings::burstModeExtendible, 3, 0, 255, 0);

void OMInputsInterface::begin(AAMVirtualReplica &replica)
{
  // motor pwm setup
  ledcAttachPin(GGTR16_MOTOR_PIN, GGTR16_MOTOR_LEDC_CHANNEL);
  ledcSetup(GGTR16_MOTOR_LEDC_CHANNEL, GGTR16_MOTOR_LEDC_FREQ, GGTR16_MOTOR_LEDC_RES);
  ledcWrite(GGTR16_MOTOR_LEDC_CHANNEL, 0); //turn off motor
  
  pinMode(GGTR16_MOTOR_PIN, OUTPUT);

  ggtr16TriggerDebouncer.attach(GGTR16_FIRINGGROUP_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  ggtr16TriggerDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);
  
  ggtr16CutoffDebouncer.attach(GGTR16_CUTOFF_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  ggtr16CutoffDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);

  ggtr16SelectorDebouncer.attach(GGTR16_SELECTOR_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  ggtr16SelectorDebouncer.interval(GGTR16_DEBOUNCE_TIME_MS);

  if(digitalRead(GGTR16_SELECTOR_PIN) == LOW)
  {
    replica.getSelector().setState(AAMVirtualSelector::stateAuto);
  }else
  {
    replica.getSelector().setState(AAMVirtualSelector::stateSemi);//NOTE: on the G&G TR16 we have no mean to know when selector is on safe, it just physicaly disable the firing group, so we have to initialize on semi.
  }

}

void OMInputsInterface::update(AAMVirtualReplica &replica)
{
  ggtr16TriggerDebouncer.update();
  ggtr16CutoffDebouncer.update();
  ggtr16SelectorDebouncer.update();
   
  if( ggtr16TriggerDebouncer.fell()) {  // Call code if button transitions from HIGH to LOW
    replica.getTrigger().pull();
  }else if( ggtr16TriggerDebouncer.rose()) {
    replica.getTrigger().release();
  }
   
  if( ggtr16CutoffDebouncer.fell()) {  // Call code if button transitions from HIGH to LOW
    replica.getGearbox().cycleEndDetected();
  }

  if( ggtr16SelectorDebouncer.fell()) {  // Call code if button transitions from HIGH to LOW
    replica.getSelector().setState(AAMVirtualSelector::stateAuto);
  }else if( ggtr16SelectorDebouncer.rose()) {
    replica.getSelector().setState(AAMVirtualSelector::stateSemi);
  }
}

AAMFiringSettings &OMInputsInterface::getCurrentFiringSetting(AAMVirtualSelector &selector)
{
  switch(selector.getState())
  {
    case AAMVirtualSelector::stateAuto:
      //return ggtr16FiringSettingsAuto;
      return OMConfiguration::fireModes[1];
      break;

    case AAMVirtualSelector::stateSafe://NOTE: in an ideal world, this should raise an exception, but meh...
    case AAMVirtualSelector::stateSemi:
    default:
      //return ggtr16FiringSettingsSemi;
      return OMConfiguration::fireModes[0];
  }
}

void OMInputsInterface::motorOn()
{
  //digitalWrite(GGTR16_MOTOR_PIN, HIGH);
  ledcWrite(GGTR16_MOTOR_LEDC_CHANNEL, 255);
}

void OMInputsInterface::motorOff()
{
  //digitalWrite(GGTR16_MOTOR_PIN, LOW);
  ledcWrite(GGTR16_MOTOR_LEDC_CHANNEL, 0);
}

#endif
// END G&G TR16 specific code -----------------------------------------
