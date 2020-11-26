#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <Arduino.h>
#include "config.h"

class OMFiringSettings;
class OMVirtualTrigger;
class OMVirtualReplica;
class OMVirtualSelector;

//----------------------------- FIRING SETTINGS -----------------------------
class OMFiringSettings
{
  public:
  
    enum BurstMode
    {
      burstModeNormal,
      burstModeInterruptible,
      burstModeExtendible,
    };

    OMFiringSettings(OMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, uint8_t motorPower, unsigned int timeBetweenShots_ms)
    :_burstMode(burstMode),_burstLength(burstLength),_precockDuration_ms(_precockDuration_ms),_motorPower(motorPower),_timeBetweenShots_ms(timeBetweenShots_ms)
    {};

    OMFiringSettings()
    :OMFiringSettings(OMFiringSettings::burstModeNormal, 1, 0, 255, 0)//Note those are kind of arbitrary default values
    {};

    OMFiringSettings::BurstMode getBurstMode(){ return this->_burstMode; }
    uint8_t getBurstLength(){ return this->_burstLength; }
    unsigned int getPrecockDurationMs(){ return this->_precockDuration_ms; }
    uint8_t getMotorPower(){ return this->_motorPower; }
    unsigned int getTimeBetweenShotsMs(){ return this->_timeBetweenShots_ms; }

    void setBurstMode(OMFiringSettings::BurstMode burstMode){this->_burstMode = burstMode;}
    void setBurstLength(uint8_t burstLength){this->_burstLength = burstLength;}
    void setPrecockDuration_ms(unsigned int precockDuration_ms){this->_precockDuration_ms = precockDuration_ms;}
    void setMotorPower(uint8_t motorPower){this->_motorPower = motorPower;}
    void setTimeBetweenShots_ms(unsigned int timeBetweenShots_ms){this->_timeBetweenShots_ms = timeBetweenShots_ms;}
    
  private:
    OMFiringSettings::BurstMode _burstMode;
    uint8_t _burstLength;
    unsigned int _precockDuration_ms;
    uint8_t _motorPower;
    unsigned int _timeBetweenShots_ms;

    
};

//----------------------------- TRIGGER -------------------------------------




class OMVirtualTrigger
{
  public:
    enum TriggerState
    {
      stateReleased,
      statePulled
    };

  private:    
    OMVirtualReplica *_replica;
    OMVirtualTrigger::TriggerState _state;
    
  public:
    OMVirtualTrigger(OMVirtualReplica *replica)
    :_replica(replica),_state(OMVirtualTrigger::stateReleased)
    {};
    
    void pull(void);
    
    void release(void);
    
    OMVirtualTrigger::TriggerState getState(void){ return this->_state; }


    
};




//----------------------------- GEARBOX -------------------------------------




class OMVirtualGearbox
{
  public:
    enum GearboxState
    {
      stateResting,
      statePrecocked,
      stateCycling,
    };

    enum GearboxCycleState
    {
      stateCocking,
      statePrecocking,
    };

  private:
    uint8_t _motorPin;
    unsigned int _precockDuration_ms;
    unsigned long _precockEndTime_ms;
    OMVirtualReplica *_replica;
    OMVirtualGearbox::GearboxState _state;
    OMVirtualGearbox::GearboxCycleState _cycleState;

  public:

    OMVirtualGearbox(OMVirtualReplica *replica)
    :_replica(replica),_state(OMVirtualGearbox::stateResting),_cycleState(OMVirtualGearbox::stateCocking)
    {};

    void cycle(unsigned int precockDuration_ms);
    
    OMVirtualGearbox::GearboxState getState(void){ return this->_state; }

    void cycleEndDetected(void);
    
    void endCycle(OMVirtualGearbox::GearboxState state);

    void update(void);
    
    
    //unsigned int precockedTime_ms;
};




//----------------------------- SELECTOR -------------------------------------




class OMVirtualSelector
{
  public:    
    enum SelectorState
    {
      stateSafe,
      stateSemi,
      stateAuto,
    };

  private:
    OMVirtualReplica *_replica;
    OMVirtualSelector::SelectorState _state;
    
    OMFiringSettings firingSettingsSemi;
    OMFiringSettings firingSettingsFull;
    
  public:
    OMVirtualSelector(OMVirtualReplica *replica)
    :_replica(replica),_state(OMVirtualSelector::stateSafe)
    {};
    
    OMVirtualSelector(OMVirtualReplica *replica, SelectorState state)
    :OMVirtualSelector(replica)
    {
      this->_state = state;
    };
    
    void setState(OMVirtualSelector::SelectorState state);
    
    OMVirtualSelector::SelectorState getState(void){ return this->_state; }
  
    OMFiringSettings *currentFiringSettings;
};




//----------------------------- REPLICA -------------------------------------




class OMVirtualReplica
{  
  public:
    enum ReplicaState
    {
      stateIdle,
      stateFiring,
    };

  private:
    OMVirtualGearbox _gearbox;
    OMVirtualTrigger _trigger;
    OMVirtualSelector _selector;

  public:
    uint8_t _bbs_fired;

  private:
    OMVirtualReplica::ReplicaState _state;
    uint8_t _currentBurstBBCount;
    unsigned long _lastActiveTimeMs;
    
    void startFiringCycle();
    void endFiringCycle();
    
  public:
    OMVirtualReplica()
    :_gearbox(this), _trigger(this), _selector(this), _bbs_fired(0), _state(OMVirtualReplica::stateIdle), _currentBurstBBCount(0), _lastActiveTimeMs(millis())
    {}

    OMVirtualReplica(OMVirtualSelector::SelectorState selectorState)
    :OMVirtualReplica()
    {
      _selector.setState(selectorState);
    }

    void updateLastActive(void);
    
    void triggerPulled(void);
    
    void triggerReleased(void);

    void gearboxCycleEndDetected(void);
    
    void setSelectorState(OMVirtualSelector::SelectorState state);

    void update(void);


    OMVirtualTrigger &getTrigger(){ return this->_trigger; }
    OMVirtualGearbox &getGearbox(){ return this->_gearbox; }
    OMVirtualSelector &getSelector(){ return this->_selector; }

  friend class OMVirtualGearbox;
};
#endif
