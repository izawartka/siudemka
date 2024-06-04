#include "input_if_ui.h"

InputIfUI::InputIfUI(std::string targetObjectName, int condition, uint16_t inputIndex, uint8_t valueIfTrue)
{
	mp_targetObjectName = targetObjectName;
	mp_condition = condition;
	mp_inputIndex = inputIndex;
	mp_valueIfTrue = valueIfTrue;
}

InputIfUI::~InputIfUI()
{

}

void InputIfUI::init()
{
	setTargetObject(mp_targetObjectName);
	m_inputIndex = mp_inputIndex;
	m_condition = mp_condition;
	m_valueIfTrue = mp_valueIfTrue;
	m_objectEventsManager = getObject()->getEventsManager();

	_ADD_LISTENER(m_objectEventsManager, UIValueChange);
}

void InputIfUI::deinit()
{
	_REMOVE_LISTENER(m_objectEventsManager, UIValueChange);
}

void InputIfUI::setTargetObject(std::string targetObjectName)
{
	m_targetEventsManager = nullptr;
	m_targetObjectName = targetObjectName;
	RZUF3_Object* targetObject = getObject()->getScene()->getObject(m_targetObjectName);
	if (targetObject == nullptr)
	{
		spdlog::error("InputIfUI target object not found: {}", m_targetObjectName);
		return;
	}
	m_targetEventsManager = targetObject->getEventsManager();
}

void InputIfUI::onUIValueChange(RZUF3_UIValueChangeEvent* event)
{
	if (m_targetEventsManager == nullptr) return;

	uint8_t value = event->getValue() == m_condition ? m_valueIfTrue : 0;

	SGE_SetModelInputEvent setModelInputEvent(m_inputIndex, value);
	m_targetEventsManager->dispatchEvent(&setModelInputEvent);
}