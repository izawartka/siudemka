#pragma once
#include "../events/events.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

#define DRAG_ACC_MULTIPLIER -0.000002f

class RotateByDrag : public RZUF3_ObjectScript {
public:
	RotateByDrag(SDL_Rect rect);
	~RotateByDrag();

	void init();
	void deinit();

	void setRect(SDL_Rect rect);

protected:
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMousePressed(RZUF3_MousePressedEvent* event);

	SDL_Rect mp_rect;

	RZUF3_EventsManager* m_objectEventsManager = nullptr;
	RZUF3_Clickable* m_clickable = nullptr;
	bool m_isDragging = false;
	int m_lastX = 0;

	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MousePressed)
};