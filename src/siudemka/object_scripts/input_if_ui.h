#pragma once
#include "object_scripts.h"

class RZUF3_ObjectScript;
class RZUF3_UIValueChangeEvent;

class InputIfUI : public RZUF3_ObjectScript {
public:
	InputIfUI(std::string targetObjectName, std::string inputName, int condition, uint16_t valueIfTrue);
	~InputIfUI();

	void init();
	void deinit();

	void setTargetObject(std::string targetObjectName);
	void setCondition(int condition) { m_condition = condition; }
	void setInputName(std::string inputName) { m_inputName = inputName; }
	void setValueIfTrue(uint16_t valueIfTrue) { m_valueIfTrue = valueIfTrue; }

protected:
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);

	std::string mp_targetObjectName;
	std::string mp_inputName;
	int mp_condition;
	uint8_t mp_valueIfTrue;

	std::string m_targetObjectName = "";
	std::string m_inputName = "";
	int m_condition = 0;
	uint16_t m_valueIfTrue = 0;
	RZUF3_EventsManager* m_objectEventsManager = nullptr;
	RZUF3_EventsManager* m_targetEventsManager = nullptr;

	_DECLARE_LISTENER(UIValueChange)
};