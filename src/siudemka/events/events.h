#pragma once
#include "../common.h"

enum User_EventType {
	User_EventType_SetAutorotate = RZUF3_EventType_User,
	User_EventType_SetRotAcceleration,
};

#include "set_autorotate.h"
#include "set_rot_acceleration.h"