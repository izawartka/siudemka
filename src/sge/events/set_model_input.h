#pragma once
#include "events.h"

class SGE_SetModelInputEvent : public RZUF3_Event {
public:
	SGE_SetModelInputEvent(uint16_t index, uint8_t value) {
		m_index = index;
		m_value = value;
	}

	int getEventType() const { return SGE_EventType_SetModelInput; }
	uint16_t getIndex() const { return m_index; }
	uint8_t getValue() const { return m_value; }

private:
	uint16_t m_index;
	uint8_t m_value;
};