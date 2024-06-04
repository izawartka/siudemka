#pragma once
#include "events.h"

class User_SetAutorotateEvent : public RZUF3_Event {
public:
	User_SetAutorotateEvent(bool value) {
		this->m_value = value;
	}

	int getEventType() const { return User_EventType_SetAutorotate; }
	bool getValue() const { return m_value; }

private:
	bool m_value;
};