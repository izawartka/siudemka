#pragma once
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UIValueChangeEvent;

class InputByUI : public RZUF3_ObjectScript {
public:
	InputByUI(std::string targetObjectName, std::string inputName, uint16_t range = 0);
	~InputByUI();

	void init();
	void deinit();

	void setTargetObject(std::string targetObjectName);
	void setInputName(std::string inputName) { m_inputName = inputName; }
	void setRange(uint16_t range) { m_range = range; }

protected:
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);

	std::string mp_targetObjectName;
	std::string mp_inputName;
	uint16_t mp_range;

	std::string m_targetObjectName = "";
	std::string m_inputName = "";
	uint16_t m_range = 0;
	RZUF3_EventsManager* m_objectEventsManager = nullptr;
	RZUF3_EventsManager* m_targetEventsManager = nullptr;

	_DECLARE_LISTENER(UIValueChange)
};