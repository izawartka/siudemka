#include "cache_by_checkbox.h"

CacheByCheckbox::CacheByCheckbox(SGE_ModelController* modelController)
{
	mp_modelController = modelController;
}

CacheByCheckbox::~CacheByCheckbox()
{
}

void CacheByCheckbox::init()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_ADD_LISTENER(objEventsManager, UIValueChange);
}

void CacheByCheckbox::deinit()
{
	RZUF3_EventsManager* objEventsManager = m_object->getEventsManager();
	_REMOVE_LISTENER(objEventsManager, UIValueChange);
}

void CacheByCheckbox::setCache(bool cache)
{
	mp_modelController->setUseCacheTexture(cache);
}

void CacheByCheckbox::onUIValueChange(RZUF3_UIValueChangeEvent* event)
{
	if (event->getTypeIndex() != typeid(bool)) return;
	if (event->getValue() == nullptr) return;

	bool uiValue = *static_cast<bool*>(event->getValue());
	setCache(uiValue);
}
