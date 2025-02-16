#include "rotate_model.h"

RotateModel::RotateModel(std::string rotInputName, bool autorotate)
{
	mp_rotInputName = rotInputName;
	mp_doAutorotate = autorotate;
}

RotateModel::~RotateModel()
{

}

void RotateModel::init()
{
	m_rotInputName = mp_rotInputName;
	m_doAutorotate = mp_doAutorotate;

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
	m_objectEventsManager = m_object->getEventsManager();
	_ADD_LISTENER_CL(m_objectEventsManager, SetAutorotate, User);
	_ADD_LISTENER_CL(m_objectEventsManager, SetRotAcceleration, User);
}

void RotateModel::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Update);
	_REMOVE_LISTENER_CL(m_objectEventsManager, SetAutorotate, User);
	_REMOVE_LISTENER_CL(m_objectEventsManager, SetRotAcceleration, User);
	m_objectEventsManager = nullptr;

	m_isAccSet = false;
	m_angle = 0;
	m_speed = AUTOROTATE_SPEED;
}

void RotateModel::onSetAutorotate(User_SetAutorotateEvent* event)
{
	m_doAutorotate = event->getValue();
}

void RotateModel::onSetRotAcceleration(User_SetRotAccelerationEvent* event)
{
	m_acc = event->getValue();
	m_isAccSet = true;
}

void RotateModel::onUpdate(RZUF3_UpdateEvent* event)
{
	if (m_doAutorotate && !m_isAccSet) {
		m_acc = AUTOROTATE_ACC;
	}

	if (m_acc == 0 && std::abs(m_speed) < ROTATE_MIN_SPEED) {
		m_speed = 0;
	}

	double oldSpeed = m_speed;

	m_speed += m_acc * event->getDt();
	m_speed -= m_speed * ROTATE_FRICTION;

	if (m_doAutorotate && oldSpeed <= AUTOROTATE_SPEED && m_speed > AUTOROTATE_SPEED && !m_isAccSet) {
		m_speed = AUTOROTATE_SPEED;
	}

	m_angle += m_speed * event->getDt();

	SGE_SetModelAngleInputEvent objEvent(m_rotInputName, m_angle);
	m_objectEventsManager->dispatchEvent(&objEvent);

	m_acc = 0;
	m_isAccSet = false;
}
