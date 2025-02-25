#pragma once
#include "object_scripts.h"
#include "../point.h"

class RZUF3_ObjectScript;

class SGE_DrawQueueScript : public RZUF3_ObjectScript {
public:
	virtual void setLayer(int layer) = 0;
	virtual void setPosition(SGE_Point position) = 0;

	virtual uint16_t getLayer() const = 0;
	virtual SGE_Point getPosition() const = 0;

	virtual void draw() = 0;
};