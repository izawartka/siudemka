#pragma once
#include "object_scripts.h"

class RZUF3_ObjectScript;

class ModelOnlyOnKey : public RZUF3_ObjectScript {
public:
	ModelOnlyOnKey(SDL_KeyCode keyCode, std::string modelObjName, std::string uiObjName);
	~ModelOnlyOnKey();

	void init();
	void deinit();

	void setKeyCode(SDL_KeyCode keyCode) { m_keyCode = keyCode; }

protected:
	void onKeyDown(RZUF3_KeyDownEvent* event);

	SDL_KeyCode mp_keyCode;
	std::string mp_modelObjName;
	std::string mp_uiObjName;

	SDL_KeyCode m_keyCode = SDLK_UNKNOWN;
	RZUF3_Object* m_modelObj = nullptr;
	RZUF3_Object* m_uiObj = nullptr;
	RZUF3_Anchor m_modelDefAnchor;
	RZUF3_Anchor m_uiDefAnchor;
	bool m_isHidden = false;
	RZUF3_EventsManager* m_eventsManager = nullptr;

	_DECLARE_LISTENER(KeyDown)
};