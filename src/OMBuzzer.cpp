#include "OMBuzzer.h"
#include "config.h"

//initialization
OMBuzzer::BuzzerState OMBuzzer::_state = OMBuzzer::stateIdle;
uint8_t OMBuzzer::_buzzerPin = OM_DEFAULT_BUZZER_PIN;
unsigned long OMBuzzer::_stopBuzzingAtMs = 0;

void OMBuzzer::begin()
{
  OMBuzzer::begin(OM_DEFAULT_BUZZER_PIN);
}

void OMBuzzer::begin(uint8_t buzzerPin)
{
  ledcSetup(OM_DEFAULT_BUZZER_CHANNEL, OM_DEFAULT_BUZZER_FREQ_HZ, OM_DEFAULT_BUZZER_RESOLUTION);
  ledcAttachPin(buzzerPin, OM_DEFAULT_BUZZER_CHANNEL);
  ledcWriteTone(OM_DEFAULT_BUZZER_CHANNEL, OM_DEFAULT_BUZZER_FREQ_HZ);
  ledcWrite(OM_DEFAULT_BUZZER_CHANNEL, 0);
}

void OMBuzzer::update()
{
  if(OMBuzzer::_state == OMBuzzer::stateBuzzing && millis() > OMBuzzer::_stopBuzzingAtMs)
  {
    ledcWrite(OM_DEFAULT_BUZZER_CHANNEL, 0);
    OMBuzzer::_state = OMBuzzer::stateIdle;
  }
}

void OMBuzzer::buzz()
{
  OMBuzzer::buzz(OM_DEFAULT_BUZZER_DURATION_MS, OM_DEFAULT_BUZZER_FREQ_HZ);
}

void OMBuzzer::buzz(double freq)
{
  OMBuzzer::buzz(OM_DEFAULT_BUZZER_DURATION_MS, freq);
}

void OMBuzzer::buzz(unsigned int duration_ms)
{
  OMBuzzer::buzz(duration_ms, OM_DEFAULT_BUZZER_FREQ_HZ);
}

void OMBuzzer::buzz(unsigned int duration_ms, double freq)
{
  ledcWriteTone(OM_DEFAULT_BUZZER_CHANNEL, freq);
  ledcWrite(OM_DEFAULT_BUZZER_CHANNEL, 127);
  OMBuzzer::_state = OMBuzzer::stateBuzzing;
  OMBuzzer::_stopBuzzingAtMs = millis() + duration_ms;
}