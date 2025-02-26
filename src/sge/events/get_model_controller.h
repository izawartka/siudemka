#pragma once
#include "events.h"

class SGE_ModelController;

class SGE_GetModelControllerEvent : public RZUF3_Event {
public:
	SGE_GetModelControllerEvent() {};

	void setController(SGE_ModelController* controller) {
		if (m_reachedTarget) {
			spdlog::error("SGE_GetModelControllerEvent reached multiple targets");
		}

		m_controller = controller;
		m_reachedTarget = true;
	}

	SGE_ModelController* getController() const {
		if (!m_reachedTarget) {
			spdlog::error("SGE_GetModelControllerEvent target not reached");
		}

		return m_controller;
	}

	bool reachedTarget() const {
		return m_reachedTarget;
	}

private:
	SGE_ModelController* m_controller = nullptr;
	bool m_reachedTarget = false;
};