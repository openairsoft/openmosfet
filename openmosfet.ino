//comment line below if not debug
#define DEBUG

#include "utilities.h"
#include "config.h"
#include "components.h"
#include "configuration.h"
#include "Mosfetv2wifiserver.h"

AAMVirtualReplica replica;

//---------- X : Replica specific code 2 -----------------------------------------------------------------------------------------
#ifdef REPLICA_TYPE_GGTR16
#include <Bounce2.h>
Bounce ggtr16TriggerDebouncer = Bounce();
Bounce ggtr16CutoffDebouncer = Bounce();
Bounce ggtr16SelectorDebouncer = Bounce();

//AAMFiringSettings ggtr16FiringSettingsSemi(AAMFiringSettings::burstModeNormal, 1, 0, 255, 0);
//AAMFiringSettings ggtr16FiringSettingsAuto(AAMFiringSettings::burstModeExtendible, 3, 0, 255, 0);

AAMFiringSettings &getCurrentFiringSetting(AAMVirtualSelector &selector)//MANDATORY FUNCTION
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

void motorOn()//MANDATORY FUNCTION
{
  //digitalWrite(GGTR16_MOTOR_PIN, HIGH);
  ledcWrite(MOTOR_LEDC_CHANNEL, 255);
}

void motorOff()//MANDATORY FUNCTION
{
  //digitalWrite(GGTR16_MOTOR_PIN, LOW);
  ledcWrite(MOTOR_LEDC_CHANNEL, 0);
}
#endif

void setup() {
  Serial.begin(115200);

  // motor pwm setup
  ledcAttachPin(GGTR16_MOTOR_PIN, MOTOR_LEDC_CHANNEL);
  ledcSetup(MOTOR_LEDC_CHANNEL, MOTOR_LEDC_FREQ, MOTOR_LEDC_RES);
  ledcWrite(MOTOR_LEDC_CHANNEL, 0); //turn off motor
  
  if (!FILESYSTEM.begin()) {
    #ifdef DEBUG
      Serial.println("Failed to mount file system, trying to format...");
    #endif
    if (!FILESYSTEM.format()) {
      #ifdef DEBUG
        Serial.println("Unable to format(), aborting...");
      #endif   
      return;
    }else{
      if (!FILESYSTEM.begin()) {
      #ifdef DEBUG
        Serial.println("Failed to mount file system, evn after formating...");
      #endif
      }
    }
  }
  
  #ifdef DEBUG
    Serial.print("load status: ");
    Serial.println(OMConfiguration::load());
    Serial.println("config chargée :");
    OMConfiguration::printCfg();
  #endif

  OMConfiguration::load();
  
  // START G&G TR16 specific code
  #ifdef REPLICA_TYPE_GGTR16
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
  

  #endif
  // END G&G TR16 specific code 


  Mosfetv2wifiserver::begin();
  Serial.println("lol");
}

void loop() {
  Mosfetv2wifiserver::update();
  replica.update();
  // START G&G TR16 specific code
  #ifdef REPLICA_TYPE_GGTR16
  
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
  #endif
  // END G&G TR16 specific code

}
