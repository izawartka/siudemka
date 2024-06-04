#pragma once
#include "../common.h"
#include "../bmd_structs.h"

class SGE_ModelDef : public RZUF3_Asset {
public:
	static SGE_ModelDef* getInstance(std::string filepath);

	bool load(RZUF3_AssetsManager* assetsManager);
	void unload();
	void* getContent();

private:
	SGE_BMD_File m_bmdFile;
};