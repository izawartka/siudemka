#pragma once
#include "events.h"

class SGE_SetModelAngleInputEvent : public RZUF3_Event {
public:
	SGE_SetModelAngleInputEvent(std::string name, double angle) {
		m_name = name;
		m_angle = angle;
	}

	std::string getName() const { return m_name; }
	double getAngle() const { return m_angle; }

private:
	std::string m_name;
	double m_angle;
};