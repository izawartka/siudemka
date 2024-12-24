#pragma once
#include "events.h"
#include "../point.h"

class SGE_SetWorldPositionEvent : public RZUF3_Event {
public:
	SGE_SetWorldPositionEvent(SGE_Point position) {
		m_position = position;
	}

	SGE_Point getPosition() const { return m_position; }

private:
	SGE_Point m_position;
};