#pragma once
#include "events.h"

class User_SetRotAccelerationEvent : public RZUF3_Event {
public:
	User_SetRotAccelerationEvent(double value) {
		this->m_value = value;
	}

	double getValue() const { return m_value; }

private:
	double m_value;
};