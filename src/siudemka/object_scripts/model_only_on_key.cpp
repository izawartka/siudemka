#include "model_only_on_key.h"

ModelOnlyOnKey::ModelOnlyOnKey(SDL_KeyCode keyCode, std::string modelObjName, std::string uiObjName)
{
	mp_keyCode = keyCode;
	mp_modelObjName = modelObjName;
	mp_uiObjName = uiObjName;
}

ModelOnlyOnKey::~ModelOnlyOnKey()
{

}

void ModelOnlyOnKey::init()
{
	m_keyCode = mp_keyCode;

	m_modelObj = g_scene->getObject(mp_modelObjName);
	m_uiObj = g_scene->getObject(mp_uiObjName);

	m_eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(m_eventsManager, KeyDown);
}

void ModelOnlyOnKey::deinit()
{
	_REMOVE_LISTENER(m_eventsManager, KeyDown);

	m_modelObj = nullptr;
	m_uiObj = nullptr;

	m_eventsManager = nullptr;
	m_isHidden = false;
}

void ModelOnlyOnKey::onKeyDown(RZUF3_KeyDownEvent* event)
{
	if (event->getKeyCode() != m_keyCode) return;
	if (!m_modelObj || !m_uiObj) return;

	m_isHidden = !m_isHidden;

	RZUF3_EventsManager* modelEventsManager = m_modelObj->getEventsManager();
	RZUF3_EventsManager* uiEventsManager = m_uiObj->getEventsManager();

	if (m_isHidden) {
		RZUF3_GetAnchorEvent modelGetAnchorEvent;
		modelEventsManager->dispatchEvent(&modelGetAnchorEvent);
		m_modelDefAnchor = modelGetAnchorEvent.getAnchor();

		RZUF3_GetAnchorEvent uiGetAnchorEvent;
		uiEventsManager->dispatchEvent(&uiGetAnchorEvent);
		m_uiDefAnchor = uiGetAnchorEvent.getAnchor();
	}

	g_game->setWindowMinimumSize(0, m_isHidden ? WINDOW_MO_HEIGHT : WINDOW_MIN_HEIGHT);
	// g_game->setWindowSize(0, m_isHidden ? WINDOW_MO_HEIGHT : WINDOW_MIN_HEIGHT);

	RZUF3_SetAnchorEvent modelSetAnchorEvent(m_isHidden ? 
		RZUF3_Anchor(0.5, 0.5, -MODEL_WIDTH/2, -MODEL_HEIGHT/2) :
		m_modelDefAnchor);
	modelEventsManager->dispatchEvent(&modelSetAnchorEvent);

	RZUF3_SetAnchorEvent uiSetAnchorEvent(m_isHidden ? 
		RZUF3_Anchor(2.0, 2.0) :
		m_uiDefAnchor);
	uiEventsManager->dispatchEvent(&uiSetAnchorEvent);
}
