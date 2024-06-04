#pragma once
#include "../common.h"
#include "../events/events.h"

class RZUF3_ObjectScript;

#define AUTOROTATE_SPEED 0.01
#define AUTOROTATE_ACC 0.00001f
#define ROTATE_FRICTION 0.001f
#define ROTATE_MIN_SPEED 0.001f

class RotateModel : public RZUF3_ObjectScript {
public:
	RotateModel(uint16_t rotInputIndex, bool autorotate = false);
	~RotateModel();

	void init();
	void deinit();

	void setRotInputIndex(uint16_t rotInputIndex) { m_rotInputIndex = rotInputIndex; }
	void setAutorotate(bool value) { m_doAutorotate = value; }

protected:
	void onSetAutorotate(User_SetAutorotateEvent* event);
	void onSetRotAcceleration(User_SetRotAccelerationEvent* event);
	void onUpdate(RZUF3_UpdateEvent* event);

	uint16_t mp_rotInputIndex;
	bool mp_doAutorotate;

	uint16_t m_rotInputIndex = 0;
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