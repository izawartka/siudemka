#pragma once
#include "object_scripts.h"

class RZUF3_ObjectScript;

class CacheByCheckbox : public RZUF3_ObjectScript {
public:
	CacheByCheckbox(SGE_ModelController* modelController);
	~CacheByCheckbox();

	void init();
	void deinit();

	void setCache(bool cache);

protected:
	void onUIValueChange(RZUF3_UIValueChangeEvent* event);

	SGE_ModelController* mp_modelController = nullptr;

	_DECLARE_LISTENER(UIValueChange)
};