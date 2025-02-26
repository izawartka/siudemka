#pragma once
#include "../events/events.h"
#include "object_scripts.h"

struct SGE_ModelClickableOptions {

};

class SGE_ModelClickable : public RZUF3_ObjectScript {
public:
	SGE_ModelClickable();
	SGE_ModelClickable(SGE_ModelClickableOptions options);
	~SGE_ModelClickable();

	void init();
	void deinit();

protected:
	void onMouseDown(RZUF3_MouseDownEvent* event);
	void onMouseUp(RZUF3_MouseUpEvent* event);

	void linkModelController();
	bool checkIfInsideModel(int x, int y);

	SGE_ModelClickableOptions mp_options;

	SGE_ModelClickableOptions m_options;

	SGE_ModelController* m_modelController = nullptr;
	RZUF3_EventsManager* m_objEventsManager = nullptr;

	_DECLARE_LISTENER(MouseDown)
	_DECLARE_LISTENER(MouseUp)
};