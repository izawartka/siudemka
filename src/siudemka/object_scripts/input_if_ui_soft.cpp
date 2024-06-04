#include "input_if_ui_soft.h"

InputIfUISoft::InputIfUISoft(std::string targetObjectName, int condition, uint16_t inputIndex, uint8_t targetValue, double step)
{
	mp_targetObjectName = targetObjectName;
	mp_condition = condition;
	mp_inputIndex = inputIndex;
	mp_targetValue = targetValue;
	mp_step = step;
}

InputIfUISoft::~InputIfUISoft()
{

}

void InputIfUISoft::init()
{
	setTargetObject(mp_targetObjectName);
	m_inputIndex = mp_inputIndex;
	m_condition = mp_condition;
	m_targetValue = mp_targetValue;
	m_step = mp_step;
	m_currentValue = 0;
	m_active = 0;

	m_objectEventsManager = getObject()->getEventsManager();

	_ADD_LISTENER(m_objectEventsManager, UIValueChange);

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
}

void InputIfUISoft::deinit()
{
	_REMOVE_LISTENER(m_objectEventsManager, UIValueChange);

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);
}

void InputIfUISoft::setTargetObject(std::string targetObjectName)
{
	m_targetEventsManager = nullptr;
	m_targetObjectName = targetObjectName;
	RZUF3_Object* targetObject = getObject()->getScene()->getObject(m_targetObjectName);
	if (targetObject == nullptr)
	{
		spdlog::error("InputIfUISoft target object not found: {}", m_targetObjectName);
		return;
	}
	m_targetEventsManager = targetObject->getEventsManager();
}

void InputIfUISoft::onUIValueChange(RZUF3_UIValueChangeEvent* event)
{
	m_active = event->getValue() == m_condition;
}

void InputIfUISoft::onUpdate(RZUF3_UpdateEvent* event)
{
	if (m_targetEventsManager == nullptr) return;

	if (m_currentValue == 0 && !m_active) return;
	if (m_currentValue == m_targetValue && m_active) return;

	m_currentValue += (m_active ? m_step : -m_step) * event->getDt();
	if (m_currentValue < 0) m_currentValue = 0;
	if (m_currentValue > m_targetValue) m_currentValue = m_targetValue;

	SGE_SetModelInputEvent* setModelInputEvent = new SGE_SetModelInputEvent(m_inputIndex, m_currentValue);
	m_targetEventsManager->dispatchEvent(setModelInputEvent);
}
