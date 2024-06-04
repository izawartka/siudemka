#pragma once
#include "../common.h"

class RZUF3_ObjectScript;
class RZUF3_UIValueChangeEvent;

class InputIfUIEquals : public RZUF3_ObjectScript {
public:
	InputIfUIEquals(std::string targetObjectName, int condition, uint16_t inputIndex, uint8_t valueIfTrue);
	~InputIfUIEquals();

	void init();
	void deinit();

	void setTargetObject(std::string targetObjectName);
	void setCondition(int condition) { m_condition = condition; }
	void setInputIndex(uint16_t inputIndex) { m_inputIndex = inputIndex; }
	void setValueIfTrue(uint8_t valueIfTrue) { m_valueIfTrue = valueIfTrue; }

protected:
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);

	std::string mp_targetObjectName;
	int mp_condition;
	uint16_t mp_inputIndex;
	uint8_t mp_valueIfTrue;

	std::string m_targetObjectName = "";
	int m_condition = 0;
	uint16_t m_inputIndex = 0;
	uint8_t m_valueIfTrue = 0;
	RZUF3_EventsManager* m_objectEventsManager = nullptr;
	RZUF3_EventsManager* m_targetEventsManager = nullptr;

	_DECLARE_LISTENER(UIValueChange)
};