#ifndef NOWINTERFACE_H
#define NOWINTERFACE_H

#include <Arduino.h>

enum class EspnowCommand {
	TRIGGER_STATE = 0,
	TRIGGER_BUMP = 1,
	SELECTOR_STATE = 2,
	GEARBOX_UNCOCK = 3
	};

typedef struct struct_command_triggerState {
  	const EspnowCommand command = EspnowCommand::TRIGGER_STATE;
	uint8_t state;
} struct_command_triggerState;

typedef struct struct_command_selectorState {
  	const EspnowCommand command = EspnowCommand::SELECTOR_STATE;
	uint8_t state;
} struct_command_selectorState;

enum class EspnowStatus {
	MAC_ADDRESS = 0,
	BBS_FIRED = 1,
	BATTERY_VOLTAGE = 2,
	SELECTOR_STATE = 3
	};

typedef struct struct_status_MacAdress {
  	const EspnowStatus status = EspnowStatus::MAC_ADDRESS;
	uint8_t macAddress[6];
} struct_status_MacAdress;

typedef struct struct_status_BbsFired {
  	const EspnowStatus status = EspnowStatus::BBS_FIRED;
	long bbsFired;
} struct_status_BbsFired;

typedef struct struct_status_BatteryVoltage {
  	const EspnowStatus status = EspnowStatus::BATTERY_VOLTAGE;
	float batteryVoltage;
} struct_status_BatteryVoltage;

typedef struct struct_status_SelectorState {
  	const EspnowStatus status = EspnowStatus::SELECTOR_STATE;
	float selectorState;
} struct_status_SelectorState;

class EspNowClient
{
	public:
		static uint8_t openMosfetMacAddress[6];
		static void begin();
		static void handleMessage(const uint8_t mac[6], const uint8_t *incomingData, size_t len, void* cbarg);
};

#endif