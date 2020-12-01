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
    burstModeInterruptible,
    burstModeNormal,
    burstModeExtendible,
  };

  OMFiringSettings(OMFiringSettings::BurstMode burstMode, uint8_t burstLength, unsigned int _precockDuration_ms, float motorPower, unsigned int timeBetweenShots_ms)
      : _burstMode(burstMode), _burstLength(burstLength), _precockDuration_ms(_precockDuration_ms), _motorPower(motorPower), _timeBetweenShots_ms(timeBetweenShots_ms){};

  OMFiringSettings()
      : OMFiringSettings(OMFiringSettings::burstModeNormal, 1, 0, 255, 0) //Note those are kind of arbitrary default values
        {};

  OMFiringSettings::BurstMode getBurstMode() { return this->_burstMode; }
  uint8_t getBurstLength() { return this->_burstLength; }
  unsigned int getPrecockDurationMs() { return this->_precockDuration_ms; }
  float getMotorPower() { return this->_motorPower; }
  unsigned int getTimeBetweenShotsMs() { return this->_timeBetweenShots_ms; }

  void setBurstMode(OMFiringSettings::BurstMode burstMode) { this->_burstMode = burstMode; }
  void setBurstLength(uint8_t burstLength) { this->_burstLength = burstLength; }
  void setPrecockDuration_ms(unsigned int precockDuration_ms) { this->_precockDuration_ms = precockDuration_ms; }
  void setMotorPower(float motorPower) { this->_motorPower = motorPower; }
  void setTimeBetweenShots_ms(unsigned int timeBetweenShots_ms) { this->_timeBetweenShots_ms = timeBetweenShots_ms; }

private:
  OMFiringSettings::BurstMode _burstMode;
  uint8_t _burstLength;
  unsigned int _precockDuration_ms;
  float _motorPower;
  unsigned int _timeBetweenShots_ms;
  float _decockAfter_s;
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
  static void pull(void);
  static void release(void);
  static OMVirtualTrigger::TriggerState getState(void) { return OMVirtualTrigger::_state; }

private:
  static OMVirtualTrigger::TriggerState _state;
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
  static void cycle(unsigned int precockDuration_ms);
  static OMVirtualGearbox::GearboxState getState(void) { return OMVirtualGearbox::_state; }
  static void cycleEndDetected(void);
  static void endCycle(OMVirtualGearbox::GearboxState state);
  static void update(void);

private:
  static unsigned int _precockDuration_ms;
  static unsigned long _precockEndTime_ms;
  static OMVirtualGearbox::GearboxState _state;
  static OMVirtualGearbox::GearboxCycleState _cycleState;
};

//----------------------------- SELECTOR -------------------------------------

class OMVirtualSelector
{
public:
  enum SelectorState
  {
    stateSafe = 0,
    stateSemi = 1,
    stateAuto = 2,
  };
  static void setState(OMVirtualSelector::SelectorState state);
  static OMVirtualSelector::SelectorState getState(void) { return OMVirtualSelector::_state; }
  static OMFiringSettings *currentFiringSettings;

private:
  static OMVirtualSelector::SelectorState _state;
  static OMFiringSettings _firingSettingsSemi;
  static OMFiringSettings _firingSettingsFull;
};

//----------------------------- REPLICA -------------------------------------

class OMVirtualReplica
{
  friend class OMVirtualGearbox;

  public:
    enum ReplicaState
    {
      stateIdle,
      stateFiring,
    };
    static void begin(void);
    static void update(void);
    static void updateLastActive(void);
    static void triggerPulled(void);
    static void triggerReleased(void);
    static void gearboxCycleEndDetected(void);
    static void setSelectorState(OMVirtualSelector::SelectorState state);

  private:
    static uint8_t _bbs_fired;
    static OMVirtualReplica::ReplicaState _state;
    static uint8_t _currentBurstBBCount;
    static unsigned long _lastActiveTimeMs;
    static unsigned long _lastTriggerReleaseMs;
    static unsigned long _lastEndCycleMs;

    static void startFiringCycle();
    static void endFiringCycle();

};
#endif
