#include "model_clickable.h"

SGE_ModelClickable::SGE_ModelClickable()
{
}

SGE_ModelClickable::SGE_ModelClickable(SGE_ModelClickableOptions options)
{
	mp_options = options;
}

SGE_ModelClickable::~SGE_ModelClickable()
{

}

void SGE_ModelClickable::init()
{
	m_options = mp_options;
	m_objEventsManager = m_object->getEventsManager();
	linkModelController();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, MouseDown);
	_ADD_LISTENER(eventsManager, MouseUp);
}

void SGE_ModelClickable::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, MouseDown);
	_REMOVE_LISTENER(eventsManager, MouseUp);

	m_modelController = nullptr;
	m_objEventsManager = nullptr;
}

void SGE_ModelClickable::onMouseDown(RZUF3_MouseDownEvent* event)
{
	int x = event->getX();
	int y = event->getY();
	if (!checkIfInsideModel(x, y)) return;

	spdlog::debug("Mouse Down on Model");
	m_objEventsManager->dispatchEvent(event);
}

void SGE_ModelClickable::onMouseUp(RZUF3_MouseUpEvent* event)
{
	int x = event->getX();
	int y = event->getY();
	if (!checkIfInsideModel(x, y)) return;

	spdlog::debug("Mouse Up on Model");
	m_objEventsManager->dispatchEvent(event);
}

void SGE_ModelClickable::linkModelController()
{
	SGE_GetModelControllerEvent objEvent;
	m_objEventsManager->dispatchEvent(&objEvent);
	m_modelController = objEvent.getController();
}

bool SGE_ModelClickable::checkIfInsideModel(int x, int y)
{
	if(!m_modelController) return false;
	return m_modelController->isXYInModel(x, y, false);
}
