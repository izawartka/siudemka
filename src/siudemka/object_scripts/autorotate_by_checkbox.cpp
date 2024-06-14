#include "autorotate_by_checkbox.h"
#include "../events/events.h"

AutorotateByCheckbox::AutorotateByCheckbox(std::string targetObjectName)
{
	mp_targetObjectName = targetObjectName;
}

AutorotateByCheckbox::~AutorotateByCheckbox()
{

}

void AutorotateByCheckbox::init()
{
	setTargetObject(mp_targetObjectName);

	RZUF3_EventsManager* objectEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objectEventsManager, UIValueChange);
}

void AutorotateByCheckbox::deinit()
{
	RZUF3_EventsManager* objectEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objectEventsManager, UIValueChange);

	m_targetEventsManager = nullptr;
}

void AutorotateByCheckbox::setTargetObject(std::string targetObjectName)
{
	m_targetEventsManager = nullptr;
	m_targetObjectName = targetObjectName;
	RZUF3_Object* targetObject = g_scene->getObject(m_targetObjectName);
	if (targetObject == nullptr)
	{
		spdlog::error("AutorotateByCheckbox target object not found: {}", m_targetObjectName);
		return;
	}
	m_targetEventsManager = targetObject->getEventsManager();
}

void AutorotateByCheckbox::setAutorotate(bool value)
{
	if (m_targetEventsManager == nullptr) return;

	User_SetAutorotateEvent setAutorotateEvent(value);
	m_targetEventsManager->dispatchEvent(&setAutorotateEvent);
}

void AutorotateByCheckbox::onUIValueChange(RZUF3_UIValueChangeEvent* event)
{
	setAutorotate(event->getValue() != 0);
}