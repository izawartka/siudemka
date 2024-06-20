#pragma once
#include "../events/events.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

#define AUTOROTATE_SPEED 0.01
#define AUTOROTATE_ACC 0.00001f
#define ROTATE_FRICTION 0.001f
#define ROTATE_MIN_SPEED 0.001f

class RotateModel : public RZUF3_ObjectScript {
public:
	RotateModel(std::string rotInputName, bool autorotate = false);
	~RotateModel();

	void init();
	void deinit();

	void setRotInputName(std::string name) { m_rotInputName = name; }
	void setAutorotate(bool value) { m_doAutorotate = value; }

protected:
	void onSetAutorotate(User_SetAutorotateEvent* event);
	void onSetRotAcceleration(User_SetRotAccelerationEvent* event);
	void onUpdate(RZUF3_UpdateEvent* event);

	std::string mp_rotInputName;
	bool mp_doAutorotate;

	std::string m_rotInputName;
	bool m_doAutorotate = false;
	bool m_isAccSet = false;
	double m_angle = 0;
	double m_speed = AUTOROTATE_SPEED;
	double m_acc = AUTOROTATE_ACC;
	RZUF3_EventsManager* m_objectEventsManager = nullptr;

	_DECLARE_LISTENER(SetAutorotate)
	_DECLARE_LISTENER(SetRotAcceleration)
	_DECLARE_LISTENER(Update)
};