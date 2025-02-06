#include "input_if_ui.h"

InputIfUI::InputIfUI(std::string targetObjectName, std::string inputName, int condition, uint16_t valueIfTrue)
{
	mp_targetObjectName = targetObjectName;
	mp_inputName = inputName;
	mp_condition = condition;
	mp_valueIfTrue = valueIfTrue;
}

InputIfUI::~InputIfUI()
{

}

void InputIfUI::init()
{
	m_inputName = mp_inputName;
	m_condition = mp_condition;
	m_valueIfTrue = mp_valueIfTrue;
	setTargetObject(mp_targetObjectName);

	m_objectEventsManager = m_object->getEventsManager();

	_ADD_LISTENER(m_objectEventsManager, UIValueChange);
}

void InputIfUI::deinit()
{
	_REMOVE_LISTENER(m_objectEventsManager, UIValueChange);

	m_objectEventsManager = nullptr;
}

void InputIfUI::setTargetObject(std::string targetObjectName)
{
	m_targetEventsManager = nullptr;
	m_targetObjectName = targetObjectName;
	RZUF3_Object* targetObject = g_scene->getObject(m_targetObjectName);
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
	if (event->getTypeIndex() != typeid(int)) return;
	if (event->getValue() == nullptr) return;

	int uiValue = *static_cast<int*>(event->getValue());
	uint16_t value = uiValue == m_condition ? m_valueIfTrue : 0;

	SGE_SetModelInputEvent setModelInputEvent(m_inputName, value);
	m_targetEventsManager->dispatchEvent(&setModelInputEvent);
}