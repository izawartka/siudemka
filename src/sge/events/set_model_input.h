#pragma once
#include "events.h"

class SGE_SetModelInputEvent : public RZUF3_Event {
public:
	SGE_SetModelInputEvent(std::string name, uint16_t value) {
		m_name = name;
		m_value = value;
	}

	std::string getName() const { return m_name; }
	uint16_t getValue() const { return m_value; }

private:
	std::string m_name;
	uint16_t m_value;
};