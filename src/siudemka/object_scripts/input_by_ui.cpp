#include "input_by_ui.h"

InputByUI::InputByUI(std::string targetObjectName, uint16_t inputIndex, uint8_t range)
{
	mp_targetObjectName = targetObjectName;
	mp_inputIndex = inputIndex;
	mp_range = range;
}

InputByUI::~InputByUI()
{

}

void InputByUI::init()
{
	setTargetObject(mp_targetObjectName);
	m_inputIndex = mp_inputIndex;
	m_objectEventsManager = getObject()->getEventsManager();
	m_range = mp_range;

	_ADD_LISTENER(m_objectEventsManager, UIValueChange);
}

void InputByUI::deinit()
{
	_REMOVE_LISTENER(m_objectEventsManager, UIValueChange);
}

void InputByUI::setTargetObject(std::string targetObjectName)
{
	m_targetEventsManager = nullptr;
	m_targetObjectName = targetObjectName;
	RZUF3_Object* targetObject = getObject()->getScene()->getObject(m_targetObjectName);
	if (targetObject == nullptr)
	{
		spdlog::error("InputByUI target object not found: {}", m_targetObjectName);
		return;
	}
	m_targetEventsManager = targetObject->getEventsManager();
}

void InputByUI::onUIValueChange(RZUF3_UIValueChangeEvent* event)
{
	if (m_targetEventsManager == nullptr) return;
	int value = event->getValue();
	if (m_range != 0) {
		if (value < 0) value -= m_range * (value / m_range - 1);
		value %= m_range;
	}

	SGE_SetModelInputEvent setModelInputEvent(m_inputIndex, value);
	m_targetEventsManager->dispatchEvent(&setModelInputEvent);
}