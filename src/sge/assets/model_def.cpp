#include "model_def.h"
#include "../bmd/bmd_reader.h"
#include "../bmd/bmd_destroyer.h"

SGE_ModelDef* SGE_ModelDef::getInstance(std::string filepath)
{
	SGE_ModelDef* instance = new SGE_ModelDef();
	instance->m_filepath = filepath;
	return instance;
}

bool SGE_ModelDef::load()
{
	std::ifstream file(m_filepath, std::ios::binary);

	if (!file.is_open())
	{
		spdlog::error("Failed to open BMD file: {}", m_filepath);
		return false;
	}

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
