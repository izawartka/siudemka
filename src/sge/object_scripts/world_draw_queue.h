#pragma once
#include "object_scripts.h"

class SGE_DrawQueueScript;

typedef std::map<int, std::map<double, std::vector<SGE_DrawQueueScript*>>> SGE_DrawQueue;

class SGE_WorldDrawQueue : public RZUF3_ObjectScript {
public:
	SGE_WorldDrawQueue();
	~SGE_WorldDrawQueue();

	void init();
	void deinit();

	void addToQueue(SGE_DrawQueueScript* modelController, int layer, double z);
protected:
	void onDraw(RZUF3_DrawEvent* event);

	SGE_DrawQueue m_queue;

	_DECLARE_LISTENER(Draw);
};

extern SGE_WorldDrawQueue* g_sgeWorldDrawQueue;