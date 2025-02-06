#pragma once
#include "object_scripts.h"

class RZUF3_ObjectScript;

class FullscreenOnKey : public RZUF3_ObjectScript {
public:
	FullscreenOnKey(SDL_KeyCode keyCode);
	~FullscreenOnKey();

	void init();
	void deinit();

	void setKeyCode(SDL_KeyCode keyCode) { m_keyCode = keyCode; }

protected:
	void onKeyDown(RZUF3_KeyDownEvent* event);

	SDL_KeyCode mp_keyCode;

	SDL_KeyCode m_keyCode = SDLK_UNKNOWN;
	int m_prevWidth = 0;
	int m_prevHeight = 0;

	_DECLARE_LISTENER(KeyDown)
};