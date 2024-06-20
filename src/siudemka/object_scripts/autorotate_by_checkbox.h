#pragma once
#include "object_scripts.h"

class RZUF3_ObjectScript;

class AutorotateByCheckbox : public RZUF3_ObjectScript {
public:
	AutorotateByCheckbox(std::string targetObjectName);
	~AutorotateByCheckbox();

	void init();
	void deinit();

	void setTargetObject(std::string targetObjectName);
	void setAutorotate(bool value);

protected:
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);

	std::string mp_targetObjectName;

	std::string m_targetObjectName;
	RZUF3_EventsManager* m_targetEventsManager = nullptr;

	_DECLARE_LISTENER(UIValueChange)
};