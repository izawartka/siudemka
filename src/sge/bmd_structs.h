#pragma once
#include "common.h"

struct SGE_BMD_InfoBlock {
	uint16_t version;
	uint16_t inputsCount;
	uint8_t nameLength;
	char* name;
};

struct SGE_BMD_TextureDef {
	uint8_t index;
	uint16_t dstX;
	uint16_t dstY;
	uint16_t width;
	uint16_t height;
	uint16_t srcX;
	uint16_t srcY;
};

struct SGE_BMD_TextureSetDef {
	uint16_t atlasIndex;
	uint8_t texturesCount;
	SGE_BMD_TextureDef* textures;
};

struct SGE_BMD_AtlasDef {
	uint8_t baseImageNameLength;
	char* baseImageName;
};

struct SGE_BMD_AtlasesBlock {
	uint16_t atlasesCount;
	SGE_BMD_AtlasDef* atlases;
};

struct SGE_BMD_TextureSetsBlock {
	uint16_t textureSetsCount;
	SGE_BMD_TextureSetDef* textureSets;
};

struct SGE_BMD_SubmodelDef {
	uint8_t nameLength;
	char* name;
	uint16_t textureSetIndex;
	uint16_t indexByInputIndex;
	uint8_t indexRange;
	uint8_t indexOffset;
	uint16_t conditionInputIndex;
	uint8_t conditionValue;
	uint16_t rotByInputIndex;
	int32_t x; // in 1/20000 of pixel
	int32_t y; // in 1/20000 of pixel
};

struct SGE_BMD_SubmodelsBlock {
	uint16_t submodelsCount;
	SGE_BMD_SubmodelDef* submodels;
};

struct SGE_BMD_ViewDef {
	uint16_t submodelsCount;
	uint16_t* submodels;
};

struct SGE_BMD_ViewsBlock {
	uint16_t indexByInputIndex;
	uint8_t viewsCount;
	SGE_BMD_ViewDef* views;
};

struct SGE_BMD_File {
	char* header;
	uint32_t size;
	SGE_BMD_InfoBlock info;
	SGE_BMD_AtlasesBlock atlases;
	SGE_BMD_TextureSetsBlock textureSets;
	SGE_BMD_SubmodelsBlock submodels;
	SGE_BMD_ViewsBlock views;
};