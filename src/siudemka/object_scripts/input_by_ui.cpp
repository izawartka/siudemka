#include "input_by_ui.h"

InputByUI::InputByUI(std::string targetObjectName, std::string inputName, uint8_t range)
{
	mp_targetObjectName = targetObjectName;
	mp_inputName = inputName;
	mp_range = range;
}

InputByUI::~InputByUI()
{

}

void InputByUI::init()
{
	m_inputName = mp_inputName;
	m_range = mp_range;
	setTargetObject(mp_targetObjectName);

	m_objectEventsManager = m_object->getEventsManager();

	_ADD_LISTENER(m_objectEventsManager, UIValueChange);
}

void InputByUI::deinit()
{
	_REMOVE_LISTENER(m_objectEventsManager, UIValueChange);

	m_objectEventsManager = nullptr;
}

void InputByUI::setTargetObject(std::string targetObjectName)
{
	m_targetEventsManager = nullptr;
	m_targetObjectName = targetObjectName;
	RZUF3_Object* targetObject = g_scene->getObject(m_targetObjectName);
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

	SGE_SetModelInputEvent setModelInputEvent(m_inputName, value);
	m_targetEventsManager->dispatchEvent(&setModelInputEvent);
}