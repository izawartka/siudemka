#pragma once
#include "common.h"
#include "bmd_structs.h"

class SGE_BMD_Reader {
public:
	static bool readFile(std::ifstream& file, SGE_BMD_File& bmdFile);

	static bool readInfoBlock(std::ifstream& file, SGE_BMD_InfoBlock& infoBlock);
	static bool readInputsBlock(std::ifstream& file, SGE_BMD_InputsBlock& inputsBlock);
	static bool readInputDef(std::ifstream& file, SGE_BMD_InputDef& inputDef);
	static bool readAtlasesBlock(std::ifstream& file, SGE_BMD_AtlasesBlock& tilemapsBlock);
	static bool readAtlasDef(std::ifstream& file, SGE_BMD_AtlasDef& tilemapDef);
	static bool readTextureSetsBlock(std::ifstream& file, SGE_BMD_TextureSetsBlock& tileSetsBlock);
	static bool readTextureSetDef(std::ifstream& file, SGE_BMD_TextureSetDef& tileSetDef);
	static bool readTextureDef(std::ifstream& file, SGE_BMD_TextureDef& tileDef);
	static bool readSubmodelsBlock(std::ifstream& file, SGE_BMD_SubmodelsBlock& submodelsBlock);
	static bool readSubmodelDef(std::ifstream& file, SGE_BMD_SubmodelDef& submodelDef);
	static bool readViewsBlock(std::ifstream& file, SGE_BMD_ViewsBlock& viewsBlock);
	static bool readViewDef(std::ifstream& file, SGE_BMD_ViewDef& viewDef);
};