#pragma once
#include "../common.h"

class RZUF3_ObjectScript;
class RZUF3_UIValueChangeEvent;

class InputByUI : public RZUF3_ObjectScript {
public:
	InputByUI(std::string targetObjectName, uint16_t inputIndex, uint8_t range = 0);
	~InputByUI();

	void init();
	void deinit();

	void setTargetObject(std::string targetObjectName);
	void setInputIndex(uint16_t inputIndex) { m_inputIndex = inputIndex; }
	void setRange(uint8_t range) { m_range = range; }

protected:
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);

	std::string mp_targetObjectName;
	uint16_t mp_inputIndex;
	uint8_t mp_range;

	std::string m_targetObjectName = "";
	uint16_t m_inputIndex = 0;
	uint8_t m_range = 0;
	RZUF3_EventsManager* m_objectEventsManager = nullptr;
	RZUF3_EventsManager* m_targetEventsManager = nullptr;

	_DECLARE_LISTENER(UIValueChange)
};