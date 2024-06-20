#pragma once
#include "object_scripts.h"

class RZUF3_ObjectScript;

class WindowHeightOnKey : public RZUF3_ObjectScript {
public:
	WindowHeightOnKey(SDL_KeyCode keyCode, int hiddenHeight, int shownHeight);
	~WindowHeightOnKey();

	void init();
	void deinit();

	void setKeyCode(SDL_KeyCode keyCode) { m_keyCode = keyCode; }

protected:
	void onKeyDown(RZUF3_KeyDownEvent* event);

	SDL_KeyCode mp_keyCode;
	int mp_hiddenHeight;
	int mp_shownHeight;

	SDL_KeyCode m_keyCode = SDLK_UNKNOWN;
	bool m_isHidden = false;
	RZUF3_EventsManager* m_eventsManager = nullptr;

	_DECLARE_LISTENER(KeyDown)
};