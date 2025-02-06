#pragma once
#include "../common.h"

#define SGE_BMD_FLOAT_SCALE 20000
#define SGE_BMD_VERSION 7

struct SGE_BMD_InfoBlock {
	uint16_t version;
	uint8_t nameLength;
	char* name;
	uint32_t originX; // in 1/SGE_BMD_FLOAT_SCALE of pixel
	uint32_t originY; // in 1/SGE_BMD_FLOAT_SCALE of pixel
	uint16_t fullAngle;
};

struct SGE_BMD_InputDef {
	uint8_t nameLength;
	char* name;
};

struct SGE_BMD_InputsBlock {
	uint16_t inputsCount;
	SGE_BMD_InputDef* inputs;
};

struct SGE_BMD_TextureDef {
	uint16_t index;
	uint16_t dstX;
	uint16_t dstY;
	uint16_t width;
	uint16_t height;
	uint16_t srcX;
	uint16_t srcY;
};

struct SGE_BMD_TextureSetDef {
	uint16_t atlasIndex;
	uint16_t texturesCount;
	SGE_BMD_TextureDef* textures;
};

struct SGE_BMD_AtlasDef {
	uint8_t baseImagePathLength;
	char* baseImagePath;
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
	uint16_t indexRange;
	uint16_t indexOffset;
	uint16_t conditionInputIndex;
	uint16_t conditionValue;
	uint16_t rotByInputIndex;
	int32_t x; // in 1/SGE_BMD_FLOAT_SCALE of pixel
	int32_t y; // in 1/SGE_BMD_FLOAT_SCALE of pixel
};

struct SGE_BMD_SubmodelsBlock {
	uint16_t submodelsCount;
	SGE_BMD_SubmodelDef* submodels;
};

struct SGE_BMD_ViewDef {
	uint16_t endIndex;
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
	SGE_BMD_InputsBlock inputs;
	SGE_BMD_AtlasesBlock atlases;
	SGE_BMD_TextureSetsBlock textureSets;
	SGE_BMD_SubmodelsBlock submodels;
	SGE_BMD_ViewsBlock views;
};