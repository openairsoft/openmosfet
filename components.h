#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "config.h"

class AAMFiringSettings;
class AAMVirtualTrigger;
class AAMVirtualReplica;
class AAMVirtualSelector;

//----------------------------- FIRING SETTINGS -----------------------------
class AAMFiringSettings
{
  public:
  
    enum BurstMode
    {
      burstModeNormal,
      burstModeInterruptible,
      burstModeExtendible,
    };

    AAMFiringSettings(AAMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, uint8_t motorPower, unsigned int timeBetweenShots_ms)
    :_burstMode(burstMode),_burstLength(burstLength),_precockDuration_ms(_precockDuration_ms),_motorPower(motorPower),_timeBetweenShots_ms(timeBetweenShots_ms)
    {};

    AAMFiringSettings()
    :AAMFiringSettings(AAMFiringSettings::burstModeNormal, 1, 0, 255, 0)//Note those are kind of arbitrary default values
    {};

    AAMFiringSettings::BurstMode getBurstMode(){ return this->_burstMode; }
    uint8_t getBurstLength(){ return this->_burstLength; }
    unsigned int getPrecockDurationMs(){ return this->_precockDuration_ms; }
    uint8_t getMotorPower(){ return this->_motorPower; }
    unsigned int getTimeBetweenShotsMs(){ return this->_timeBetweenShots_ms; }

    void setBurstMode(AAMFiringSettings::BurstMode burstMode){this->_burstMode = burstMode;}
    void setBurstLength(uint8_t burstLength){this->_burstLength = burstLength;}
    void setPrecockDuration_ms(unsigned int precockDuration_ms){this->_precockDuration_ms = precockDuration_ms;}
    void setMotorPower(uint8_t motorPower){this->_motorPower = motorPower;}
    void setTimeBetweenShots_ms(unsigned int timeBetweenShots_ms){this->_timeBetweenShots_ms = timeBetweenShots_ms;}
    
  private:
    AAMFiringSettings::BurstMode _burstMode;
    uint8_t _burstLength;
    unsigned int _precockDuration_ms;
    uint8_t _motorPower;
    unsigned int _timeBetweenShots_ms;

    
};

//----------------------------- TRIGGER -------------------------------------




class AAMVirtualTrigger
{
  public:
    enum TriggerState
    {
      stateReleased,
      statePulled,
    };
    
    AAMVirtualTrigger(AAMVirtualReplica *replica)
    :_replica(replica),_state(AAMVirtualTrigger::stateReleased)
    {};
    
    void pull(void);
    
    void release(void);
    
    AAMVirtualTrigger::TriggerState getState(void){ return this->_state; }
    
  private:    
    AAMVirtualTrigger::TriggerState _state;
    AAMVirtualReplica *_replica;
};




//----------------------------- GEARBOX -------------------------------------




class AAMVirtualGearbox
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

    AAMVirtualGearbox(AAMVirtualReplica *replica)
    :_replica(replica),_state(AAMVirtualGearbox::stateResting),_cycleState(AAMVirtualGearbox::stateCocking)
    {};

    void cycle(unsigned int precockDuration_ms);
    
    AAMVirtualGearbox::GearboxState getState(void){ return this->_state; }

    void cycleEndDetected(void);
    
    void endCycle(AAMVirtualGearbox::GearboxState state);

    void update(void);
    
  private:
    uint8_t _motorPin;
    unsigned int _precockDuration_ms;
    unsigned long _precockEndTime_ms;
    AAMVirtualGearbox::GearboxState _state;
    AAMVirtualGearbox::GearboxCycleState _cycleState;
    AAMVirtualReplica *_replica;
    
    //unsigned int precockedTime_ms;
};




//----------------------------- SELECTOR -------------------------------------




class AAMVirtualSelector
{
  public:    
    enum SelectorState
    {
      stateSafe,
      stateSemi,
      stateAuto,
    };
    
    AAMVirtualSelector(AAMVirtualReplica *replica)
    :_replica(replica),_state(AAMVirtualSelector::stateSafe)
    {};
    
    AAMVirtualSelector(AAMVirtualReplica *replica, SelectorState state)
    :AAMVirtualSelector(replica)
    {
      this->_state = state;
    };
    
    void setState(AAMVirtualSelector::SelectorState state);
    
    AAMVirtualSelector::SelectorState getState(void){ return this->_state; }
  
    AAMFiringSettings *currentFiringSettings;

  private:
    AAMVirtualSelector::SelectorState _state;
    AAMVirtualReplica *_replica;
    
    AAMFiringSettings firingSettingsSemi;
    AAMFiringSettings firingSettingsFull;
};




//----------------------------- REPLICA -------------------------------------




class AAMVirtualReplica
{
  public:
    
    enum ReplicaState
    {
      stateIdle,
      stateFiring,
    };
    
    AAMVirtualReplica()
    :_gearbox(this), _trigger(this), _selector(this), _bbs_fired(0), _state(AAMVirtualReplica::stateIdle), _currentBurstBBCount(0), _lastActiveTimeMs(millis())
    {}

    AAMVirtualReplica(AAMVirtualSelector::SelectorState selectorState)
    :AAMVirtualReplica()
    {
      _selector.setState(selectorState);
    }

    void updateLastActive(void);
    
    void triggerPulled(void);
    
    void triggerReleased(void);

    void gearboxCycleEndDetected(void);
    
    void setSelectorState(AAMVirtualSelector::SelectorState state);

    void update(void);

    uint8_t _bbs_fired;

    AAMVirtualTrigger &getTrigger(){ return this->_trigger; }
    AAMVirtualGearbox &getGearbox(){ return this->_gearbox; }
    AAMVirtualSelector &getSelector(){ return this->_selector; }

  private:
    unsigned long _lastActiveTimeMs;
    
    AAMVirtualTrigger _trigger;
    AAMVirtualGearbox _gearbox;
    AAMVirtualSelector _selector;

    uint8_t _currentBurstBBCount;
    
    void startFiringCycle();
    void endFiringCycle();
    
    AAMVirtualReplica::ReplicaState _state;

        
  friend class AAMVirtualGearbox;
};
#endif
