#include "components.h"
#include "configuration.h"
#include "inputsInterface.h"



//----------------------------- TRIGGER -------------------------------------

void OMVirtualTrigger::pull(void)
{
  #ifdef DEBUG
  Serial.println("OMVirtualTrigger::pull");
  #endif
  _state = OMVirtualTrigger::statePulled;
  this->_replica->triggerPulled();
  this->_replica->updateLastActive();
}

void OMVirtualTrigger::release(void)
{  
  #ifdef DEBUG
  Serial.println("OMVirtualTrigger::release");
  #endif
  _state = OMVirtualTrigger::stateReleased;
  this->_replica->triggerReleased();
  this->_replica->updateLastActive();
}




//----------------------------- GEARBOX -------------------------------------




void OMVirtualGearbox::cycle(unsigned int precockDuration_ms)
{
  #ifdef DEBUG
  Serial.println("OMVirtualGearbox::cycle");
  #endif
  OMInputsInterface::motorOn();
  this->_precockDuration_ms = precockDuration_ms;
  this->_state = OMVirtualGearbox::stateCycling;
  this->_cycleState = OMVirtualGearbox::stateCocking;
}

void OMVirtualGearbox::cycleEndDetected(void)
{
  #ifdef DEBUG
  Serial.println("cycleEndDetected");
  #endif
  if(this->_precockDuration_ms <= 0)
  {
    this->endCycle(OMVirtualGearbox::stateResting);
  }else
  {
    this->_precockEndTime_ms = millis() + _precockDuration_ms;
    this->_cycleState = OMVirtualGearbox::statePrecocking;
  }
}

void OMVirtualGearbox::update(void)
{
  if(
      this->_state == OMVirtualGearbox::stateCycling
      &&
      this->_cycleState == OMVirtualGearbox::statePrecocking
    )
  {
    if(millis() >= this->_precockEndTime_ms)
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
  
  this->_state = state;
  this->_replica->endFiringCycle();
}




//----------------------------- SELECTOR -------------------------------------




void OMVirtualSelector::setState(OMVirtualSelector::SelectorState state)
{
  #ifdef DEBUG
  Serial.println("OMVirtualSelector::setState");
  #endif
  _state = state;
  this->_replica->updateLastActive();
}




//----------------------------- REPLICA -------------------------------------




void OMVirtualReplica::update(void)
{
  if( millis() - this->_lastActiveTimeMs > OMConfiguration::deepSleepDelayMinutes * 60000 )
  {
    #ifdef DEBUG
    Serial.print("deepsleep enabled after ");
    Serial.print( float(millis() - this->_lastActiveTimeMs) / 60000 );
    Serial.println(" minutes.");
    #endif

    //ESP.deepSleep(0);//esp8266
    esp_deep_sleep_start();
  }else{
    this->_gearbox.update();
  }
}

void OMVirtualReplica::updateLastActive(void)
{
  this->_lastActiveTimeMs = millis();
}

void OMVirtualReplica::triggerPulled(void)
{
  #ifdef DEBUG
  Serial.println("triggerPulled");
  #endif
  this->_bbs_fired = 0;
  this->startFiringCycle();
}

void OMVirtualReplica::triggerReleased(void)
{
  #ifdef DEBUG
  Serial.println("triggerReleased");
  #endif
  this->_state = OMVirtualReplica::stateIdle;
}


#define COND_SAFETY_OFF this->_selector.getState() != OMVirtualSelector::stateSafe
#define COND_GEARBOX_NOT_CYCLING this->_gearbox.getState() != OMVirtualGearbox::stateCycling
#define COND_TRIGGER_PULLED this->_trigger.getState() == OMVirtualTrigger::statePulled
#define COND_BURST_NOT_INTERRUPTIBLE OMInputsInterface::getCurrentFiringSetting(this->getSelector()).getBurstMode() != OMFiringSettings::burstModeInterruptible
#define COND_BURST_NOT_FINISHED this->_bbs_fired < OMInputsInterface::getCurrentFiringSetting(this->getSelector()).getBurstLength()
#define COND_BURST_EXTENDIBLE OMInputsInterface::getCurrentFiringSetting(this->getSelector()).getBurstMode() == OMFiringSettings::burstModeExtendible

void OMVirtualReplica::startFiringCycle(void)
{
  #ifdef DEBUG
  Serial.println("startFiringCycle");
  #endif
  
  if(COND_SAFETY_OFF && COND_GEARBOX_NOT_CYCLING )
  {
    if    
      (
        ( COND_TRIGGER_PULLED && (COND_BURST_NOT_FINISHED || COND_BURST_EXTENDIBLE) )
        ||
        ( COND_BURST_NOT_INTERRUPTIBLE && COND_BURST_NOT_FINISHED )
      )
      {
      #ifdef DEBUG
      Serial.println("\nCOND_TRIGGER_PULLED");
      Serial.println(COND_TRIGGER_PULLED);
      Serial.println("COND_BURST_NOT_FINISHED");
      Serial.println(COND_BURST_NOT_FINISHED);
      Serial.println("\nCOND_BURST_EXTENDIBLE");
      Serial.println(COND_BURST_EXTENDIBLE);
      Serial.println("\nCOND_BURST_NOT_INTERRUPTIBLE");
      Serial.println(COND_BURST_NOT_INTERRUPTIBLE);
      Serial.println("\nCOND_BURST_NOT_FINISHED");
      Serial.println(COND_BURST_NOT_FINISHED);
      #endif
        
        this->_state = OMVirtualReplica::stateFiring;
        this->_gearbox.cycle(OMInputsInterface::getCurrentFiringSetting(this->getSelector()).getPrecockDurationMs());
      }
  }
}

void OMVirtualReplica::endFiringCycle(void)
{
  #ifdef DEBUG
  Serial.println("endFiringCycle");
  #endif
  
  this->_bbs_fired++;
  this->startFiringCycle();
}
