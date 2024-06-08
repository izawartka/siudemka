#include "rotate_by_drag.h"

RotateByDrag::RotateByDrag(SDL_Rect rect)
{
	mp_rect = rect;
}

RotateByDrag::~RotateByDrag()
{
}

void RotateByDrag::init()
{
	m_clickable = new RZUF3_Clickable(mp_rect);
	m_object->addScript(m_clickable);

	m_clickable->setOnHoverCursor(SDL_SYSTEM_CURSOR_SIZEWE);

	m_objectEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(m_objectEventsManager, MouseDown)
	_ADD_LISTENER(m_objectEventsManager, MousePressed);
}

void RotateByDrag::deinit()
{
	_REMOVE_LISTENER(m_objectEventsManager, MouseDown);
	_REMOVE_LISTENER(m_objectEventsManager, MousePressed);
	m_objectEventsManager = nullptr;

	m_object->removeScript(m_clickable);
	delete m_clickable;
	m_clickable = nullptr;
}

void RotateByDrag::setRect(SDL_Rect rect)
{
	m_clickable->setRect(rect);
}

void RotateByDrag::onMouseDown(RZUF3_MouseDownEvent* event)
{
	if (event->getButton() != SDL_BUTTON_LEFT) return;
	m_lastX = event->getX();
}

void RotateByDrag::onMousePressed(RZUF3_MousePressedEvent* event)
{
	if (event->getButton() != SDL_BUTTON_LEFT) return;

	int moveX = event->getX() - m_lastX;
	double acceleration = moveX * DRAG_ACC_MULTIPLIER;

	User_SetRotAccelerationEvent SetRotAccelerationEvent(acceleration);
	m_object->getEventsManager()->dispatchEvent(&SetRotAccelerationEvent);

	m_lastX = event->getX();
}