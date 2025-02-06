#include "fullscreen_on_key.h"

FullscreenOnKey::FullscreenOnKey(SDL_KeyCode keyCode)
{
	mp_keyCode = keyCode;
}

FullscreenOnKey::~FullscreenOnKey()
{

}

void FullscreenOnKey::init()
{
	m_keyCode = mp_keyCode;

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, KeyDown);
}

void FullscreenOnKey::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, KeyDown);
}

void FullscreenOnKey::onKeyDown(RZUF3_KeyDownEvent* event)
{
	if (event->getKeyCode() != m_keyCode) return;

	bool fullscreen = g_game->getWindowFullscreen();

	if (fullscreen) {
		g_game->setWindowFullscreen(false);
		g_game->setWindowSize(m_prevWidth, m_prevHeight);
	}
	else {
		g_game->getWindowSize(&m_prevWidth, &m_prevHeight);
		int screenWidth, screenHeight;
		g_game->getScreenSize(&screenWidth, &screenHeight);
		g_game->setWindowSize(screenWidth, screenHeight);
		g_game->setWindowFullscreen(true);
	}

}
