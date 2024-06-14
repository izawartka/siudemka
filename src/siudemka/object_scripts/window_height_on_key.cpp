#include "window_height_on_key.h"

WindowHeightOnKey::WindowHeightOnKey(SDL_KeyCode keyCode, int hiddenHeight, int shownHeight)
{
	mp_keyCode = keyCode;
	mp_hiddenHeight = hiddenHeight;
	mp_shownHeight = shownHeight;
}

WindowHeightOnKey::~WindowHeightOnKey()
{

}

void WindowHeightOnKey::init()
{
	m_keyCode = mp_keyCode;

	m_eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(m_eventsManager, KeyDown);
}

void WindowHeightOnKey::deinit()
{
	_REMOVE_LISTENER(m_eventsManager, KeyDown);

	m_eventsManager = nullptr;
	m_isHidden = false;
}

void WindowHeightOnKey::onKeyDown(RZUF3_KeyDownEvent* event)
{
	if (event->getKeyCode() != m_keyCode) return;

	m_isHidden = !m_isHidden;
	g_game->setWindowSize(0, m_isHidden ? mp_hiddenHeight : mp_shownHeight);
}
