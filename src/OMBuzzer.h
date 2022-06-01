#ifndef OM_BUZZER_H
#define OM_BUZZER_H

#include <Arduino.h>

class OMBuzzer {
	public:
		static void buzz();
		static void buzz(double freq);
		static void buzz(unsigned int duration_ms);
		static void buzz(unsigned int duration_ms, double freq);
		
		static void begin();
		static void begin(uint8_t buzzerPin);
		static void update(void);

	private:

		enum BuzzerState
		{
			stateIdle,
			stateBuzzing
		};

		static BuzzerState _state;
		static uint8_t _buzzerPin;
		static unsigned long _stopBuzzingAtMs;
};

#endif