#pragma once
#include "object_scripts.h"

class SGE_ModelController;

class SGE_WorldDrawQueue : public RZUF3_ObjectScript {
public:
	SGE_WorldDrawQueue();
	~SGE_WorldDrawQueue();

	void init();
	void deinit();

	void addToQueue(SGE_ModelController* modelController, int layer, double z);
protected:
	void onDraw(RZUF3_DrawEvent* event);

	std::map<int, std::map<double, std::vector<SGE_ModelController*>>> m_queue;

	_DECLARE_LISTENER(Draw);
};

extern SGE_WorldDrawQueue* g_sgeWorldDrawQueue;