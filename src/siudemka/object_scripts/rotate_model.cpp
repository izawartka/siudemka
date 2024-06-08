#include "rotate_model.h"

RotateModel::RotateModel(uint16_t rotInputIndex, bool autorotate)
{
	mp_rotInputIndex = rotInputIndex;
	mp_doAutorotate = autorotate;
}

RotateModel::~RotateModel()
{

}

void RotateModel::init()
{
	m_rotInputIndex = mp_rotInputIndex;
	m_doAutorotate = mp_doAutorotate;

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Update);
	m_objectEventsManager = getObject()->getEventsManager();
	_ADD_LISTENER_CL(m_objectEventsManager, SetAutorotate, User);
	_ADD_LISTENER_CL(m_objectEventsManager, SetRotAcceleration, User);
}

void RotateModel::deinit()
{
	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
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

	m_speed += m_acc * event->getDt();
	m_speed -= m_speed * ROTATE_FRICTION * event->getDt();

	m_angle += m_speed * event->getDt();

	int value = int(m_angle);
	if(value < 0) value += 120 * (-value / 120 + 1);

	value %= 120;

	SGE_SetModelInputEvent setModelInputEvent(m_rotInputIndex, value);
	m_objectEventsManager->dispatchEvent(&setModelInputEvent);

	m_acc = 0;
	m_isAccSet = false;
}
