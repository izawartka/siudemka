#pragma once
#include "../common.h"

class RZUF3_ObjectScript;
class RZUF3_UIValueChangeEvent;

class InputIfUISoft : public RZUF3_ObjectScript {
public:
	InputIfUISoft(std::string targetObjectName, int condition, uint16_t inputIndex, uint8_t targetValue, double step);
	~InputIfUISoft();

	void init();
	void deinit();

	void setTargetObject(std::string targetObjectName);
	void setCondition(int condition) { m_condition = condition; }
	void setInputIndex(uint16_t inputIndex) { m_inputIndex = inputIndex; }
	void setValueIfTrue(uint8_t targetValue) { m_targetValue = targetValue; }
	void setStep(uint8_t step) { m_step = step; }

protected:
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);
	void onUpdate(RZUF3_UpdateEvent* event);

	std::string mp_targetObjectName;
	int mp_condition;
	uint16_t mp_inputIndex;
	uint8_t mp_targetValue;
	double mp_step;

	std::string m_targetObjectName = "";
	int m_condition = 0;
	uint16_t m_inputIndex = 0;
	uint8_t m_targetValue = 0;
	double m_step = 0;
	double m_currentValue = 0;
	bool m_active = false;
	RZUF3_EventsManager* m_objectEventsManager = nullptr;
	RZUF3_EventsManager* m_targetEventsManager = nullptr;

	_DECLARE_LISTENER(UIValueChange)
	_DECLARE_LISTENER(Update)
};