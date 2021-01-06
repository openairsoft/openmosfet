#include <openMosfetEspNow.h>

#include "components.h"
#include "configuration.h"
#include "inputsInterface.h"

//----------------------------- TRIGGER -------------------------------------
OMVirtualTrigger::TriggerState OMVirtualTrigger::_state = OMVirtualTrigger::stateReleased;

void OMVirtualTrigger::pull(void)
{
#ifdef DEBUG
  Serial.println("OMVirtualTrigger::pull");
#endif
  OMVirtualTrigger::_state = OMVirtualTrigger::statePulled;
  OMVirtualReplica::triggerPulled();
  OMVirtualReplica::updateLastActive();
}

void OMVirtualTrigger::release(void)
{
#ifdef DEBUG
  Serial.println("OMVirtualTrigger::release");
#endif
  OMVirtualTrigger::_state = OMVirtualTrigger::stateReleased;
  OMVirtualReplica::triggerReleased();
  OMVirtualReplica::updateLastActive();
}

//----------------------------- GEARBOX -------------------------------------
unsigned long OMVirtualGearbox::_cycleStartTime_ms = 0;
unsigned int OMVirtualGearbox::_precockDuration_ms = 0;
unsigned long OMVirtualGearbox::_precockEndTime_ms = 0;
OMVirtualGearbox::GearboxState OMVirtualGearbox::_state = OMVirtualGearbox::stateResting;
OMVirtualGearbox::GearboxCycleState OMVirtualGearbox::_cycleState = OMVirtualGearbox::stateCocking;

void OMVirtualGearbox::cycle(unsigned int precockDuration_ms)
{
  if(OMVirtualGearbox::_state != OMVirtualGearbox::stateCycling || OMVirtualGearbox::_state != OMVirtualGearbox::stateError)
  {
    #ifdef DEBUG
      Serial.println("OMVirtualGearbox::cycle");
    #endif
    if(!OMConfiguration::disableMotor){OMInputsInterface::motorOn();}
    OMVirtualGearbox::_precockDuration_ms = precockDuration_ms;
    OMVirtualGearbox::_state = OMVirtualGearbox::stateCycling;
    OMVirtualGearbox::_cycleState = OMVirtualGearbox::stateCocking;
    OMVirtualGearbox::_cycleStartTime_ms = millis();
  }
}

void OMVirtualGearbox::setState(OMVirtualGearbox::GearboxState state)
{
#ifdef DEBUG
  Serial.println("OMVirtualGearbox::setState");
#endif
  OMVirtualGearbox::_state = state;
  OMVirtualReplica::updateLastActive();
}

void OMVirtualGearbox::cycleEndDetected(void)
{
#ifdef DEBUG
  Serial.println("OMVirtualGearbox::cycleEndDetected");
#endif
  if (OMVirtualGearbox::_precockDuration_ms <= 0 || !OMConfiguration::enablePrecocking)
  {
    OMVirtualGearbox::endCycle(OMVirtualGearbox::stateResting);
  }
  else
  {
    OMVirtualGearbox::_precockEndTime_ms = millis() + _precockDuration_ms;
    OMVirtualGearbox::_cycleState = OMVirtualGearbox::statePrecocking;
  }
}

void OMVirtualGearbox::update(void)
{
  if(OMVirtualGearbox::_state == OMVirtualGearbox::stateCycling)
  {
    if(millis() - OMVirtualGearbox::_cycleStartTime_ms > MAX_TIME_BETWEEN_CYCLES_MS){
      OMVirtualGearbox::endCycle(OMVirtualGearbox::stateError);
      OMVirtualReplica::_state = OMVirtualReplica::stateIdle;
      //todo: beep
    }else if
    (
      OMVirtualGearbox::_cycleState == OMVirtualGearbox::statePrecocking &&
      millis() >= OMVirtualGearbox::_precockEndTime_ms
    )
    {
      OMVirtualGearbox::endCycle(OMVirtualGearbox::statePrecocked);  
    }

  }
}

void OMVirtualGearbox::endCycle(OMVirtualGearbox::GearboxState state)
{
#ifdef DEBUG
  Serial.println("endCycle");
#endif
  OMInputsInterface::motorOff();

  OMVirtualGearbox::_state = state;
  OMVirtualReplica::endFiringCycle();
}

//----------------------------- SELECTOR -------------------------------------
OMVirtualSelector::SelectorState OMVirtualSelector::_state = OMVirtualSelector::stateSafe;

void OMVirtualSelector::setState(OMVirtualSelector::SelectorState state)
{
#ifdef DEBUG
  Serial.println("OMVirtualSelector::setState");
#endif
  OMVirtualSelector::_state = state;
  if(OMConfiguration::enableEspNow){ OpenMosfetEspNowAsyncServer::sendSelectorState((uint8_t)state); }
  OMVirtualReplica::updateLastActive();
}

//----------------------------- REPLICA -------------------------------------

#define COND_SAFETY_OFF OMVirtualSelector::getState() != OMVirtualSelector::stateSafe
#define COND_GEARBOX_NOT_CYCLING OMVirtualGearbox::getState() != OMVirtualGearbox::stateCycling
#define COND_STATE_NOT_ERROR OMVirtualGearbox::getState() != OMVirtualGearbox::stateError
#define COND_TRIGGER_PULLED OMVirtualTrigger::getState() == OMVirtualTrigger::statePulled
#define COND_DELAY_BETWEEN_SHOTS_PASSED (millis() - OMVirtualReplica::_lastShotFiredMs) >= OMInputsInterface::getCurrentFiringSetting().getTimeBetweenShotsMs()
#define COND_BURST_NOT_INTERRUPTIBLE OMInputsInterface::getCurrentFiringSetting().getBurstMode() != OMFiringSettings::burstModeInterruptible
#define COND_BURST_NOT_FINISHED OMVirtualReplica::_bbs_fired < OMInputsInterface::getCurrentFiringSetting().getBurstLength()
#define COND_BURST_EXTENDIBLE OMInputsInterface::getCurrentFiringSetting().getBurstMode() == OMFiringSettings::burstModeExtendible
#define COND_CURRENTLY_FIRING OMVirtualReplica::_state == OMVirtualReplica::stateFiring
#define COND_TRIGGER_AS_BEEN_MAINTAINED_AFTER_LAST_END_CYCLE OMVirtualReplica::_lastTriggerReleaseMs < OMVirtualReplica::_lastEndCycleMs && OMVirtualTrigger::getState() == OMVirtualTrigger::statePulled

uint8_t OMVirtualReplica::_bbs_fired = 0;
OMVirtualReplica::ReplicaState OMVirtualReplica::_state = OMVirtualReplica::stateIdle;
uint8_t OMVirtualReplica::_currentBurstBBCount = 0;
unsigned long OMVirtualReplica::_lastActiveTimeMs = 0;
unsigned long OMVirtualReplica::_lastTriggerReleaseMs = 0;
unsigned long OMVirtualReplica::_lastEndCycleMs = 0;
unsigned long OMVirtualReplica::_lastShotFiredMs = 0;

void OMVirtualReplica::begin(void)
{
  OMVirtualReplica::_lastActiveTimeMs = millis();
  OMVirtualReplica::_lastTriggerReleaseMs = millis();
  OMVirtualReplica::_lastEndCycleMs = millis();
}



void OMVirtualReplica::update(void)
{
  OMVirtualReplica::testFiringCycle();
  
  // uncock if trigger is maintained after a set amount of time
  if(
    OMVirtualReplica::_state == OMVirtualReplica::stateIdle
    &&
    OMVirtualGearbox::getState() == OMVirtualGearbox::statePrecocked
    &&
    OMConfiguration::decockAfter_s > 0
    &&
    COND_TRIGGER_AS_BEEN_MAINTAINED_AFTER_LAST_END_CYCLE
    &&
    OMVirtualReplica::_lastEndCycleMs + (unsigned long)(OMConfiguration::decockAfter_s * 1000) < millis()
    )
  {
    OMVirtualGearbox::cycle(0);
  }

  if (millis() - OMVirtualReplica::_lastActiveTimeMs > OMConfiguration::deepSleepDelayMinutes * 60000)
  {
#ifdef DEBUG
    Serial.print("deepsleep enabled after ");
    Serial.print(float(millis() - OMVirtualReplica::_lastActiveTimeMs) / 60000);
    Serial.println(" minutes.");
#endif

    //ESP.deepSleep(0);//esp8266
    esp_deep_sleep_start();
  }
  else
  {
    OMVirtualGearbox::update();
  }
}

void OMVirtualReplica::updateLastActive(void)
{
  OMVirtualReplica::_lastActiveTimeMs = millis();
}

void OMVirtualReplica::triggerPulled(void)
{
  #ifdef DEBUG
    Serial.println("triggerPulled");
  #endif
  OMVirtualReplica::_bbs_fired = 0;
  OMVirtualReplica::testFiringCycle();
}

void OMVirtualReplica::triggerReleased(void)
{
#ifdef DEBUG
  Serial.println("triggerReleased");
#endif
  OMVirtualReplica::_lastTriggerReleaseMs = millis();
  #ifdef DEBUG
    printf("OMVirtualReplica::_lastTriggerReleaseMs : %lu\n", OMVirtualReplica::_lastTriggerReleaseMs);
  #endif
  OMVirtualReplica::_state = OMVirtualReplica::stateIdle;
}

void OMVirtualReplica::testFiringCycle(void)
{
#ifdef DEBUG
  Serial.println("testFiringCycle");
#endif

  if
  (
    COND_STATE_NOT_ERROR
    &&
    COND_SAFETY_OFF
    &&
    COND_GEARBOX_NOT_CYCLING
    &&
    COND_DELAY_BETWEEN_SHOTS_PASSED
    &&
    (
      ( COND_TRIGGER_PULLED && (COND_BURST_NOT_FINISHED || COND_BURST_EXTENDIBLE) )
      ||
      ( COND_CURRENTLY_FIRING && COND_BURST_NOT_INTERRUPTIBLE && COND_BURST_NOT_FINISHED )
    )
  )
  {
    OMVirtualReplica::_state = OMVirtualReplica::stateFiring;
    OMVirtualReplica::_lastShotFiredMs = millis();
    OMVirtualGearbox::cycle(OMInputsInterface::getCurrentFiringSetting().getPrecockDurationMs());
  }else{
    OMVirtualReplica::_state = OMVirtualReplica::stateIdle;
    #ifdef DEBUG
      Serial.println("stateIdle");
    #endif
  }
}

void OMVirtualReplica::endFiringCycle(void)
{
  #ifdef DEBUG
    Serial.println("endFiringCycle");
  #endif

  OMVirtualReplica::_lastEndCycleMs = millis();
  #ifdef DEBUG
    printf("OMVirtualReplica::_lastEndCycleMs : %lu\n", OMVirtualReplica::_lastEndCycleMs);
  #endif
  OMVirtualReplica::_bbs_fired++;
  OMVirtualReplica::testFiringCycle();
}
