#include "model_def.h"
#include "../bmd_reader.h"
#include "../bmd_destroyer.h"

SGE_ModelDef* SGE_ModelDef::getInstance(std::string filepath)
{
	SGE_ModelDef* instance = new SGE_ModelDef();
	instance->m_filepath = filepath;
	return instance;
}

bool SGE_ModelDef::load(RZUF3_AssetsManager* assetsManager)
{
	std::string directory = m_filepath.substr(0, m_filepath.find_last_of("/") + 1);

	std::ifstream file(m_filepath, std::ios::binary);

	if(!SGE_BMD_Reader::readFile(file, m_bmdFile))
	{
		return false;
	}

	spdlog::info("Loaded BMD file: {}", m_filepath);

	return true;
}

void SGE_ModelDef::unload()
{
	SGE_BMD_Destroyer::destroy(m_bmdFile);

	spdlog::info("Unloaded BMD file: {}", m_filepath);
}

void* SGE_ModelDef::getContent()
{
	return &m_bmdFile;
}
