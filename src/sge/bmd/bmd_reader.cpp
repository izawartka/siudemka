#include "bmd_reader.h"

// example:
// _READ(infoBlock.version);
// file.read(reinterpret_cast<char*>(&infoBlock.version), sizeof(infoBlock.version));
#define _READ(x) file.read(reinterpret_cast<char*>(&x), sizeof(x))

// example:
// _READ_STR(bmdFile.header, 6);
// file.read(bmdFile.header, 6);
#define _READ_STR(x, y) \
    x = new char[y + 1]; \
	x[y] = 0; \
    file.read(x, y)

// example:
// _READ_ARR(bmdFile.tilemaps.tilemaps, bmdFile.tilemaps.tilemapsCount);
// file.read(reinterpret_cast<char*>(viewDef.submodels), viewDef.submodelsCount * sizeof(uint16_t));
#define _READ_ARR(x, y) file.read(reinterpret_cast<char*>(x), y * sizeof(x[0]))

bool SGE_BMD_Reader::readFile(std::ifstream& file, SGE_BMD_File& bmdFile) {
    _READ_STR(bmdFile.header, 6);
    _READ(bmdFile.size);

    if (strcmp(bmdFile.header, "SGEBMD") != 0) {
		spdlog::error("BMD file: invalid header");
		return false;
	}

    uint32_t currentPos = 10;

    while (currentPos < bmdFile.size) {
        char header[5] = {0};
        file.read(header, 4);
        uint32_t blockSize = 0; 
        _READ(blockSize);

        if (strcmp(header, "INFO") == 0) {
            readInfoBlock(file, bmdFile.info);
            if (bmdFile.info.version != SGE_BMD_VERSION) {
                spdlog::error("BMD file {}: unsupported version {}", bmdFile.info.name, bmdFile.info.version);
                return false;
            }
        }
        else if (strcmp(header, "INPT") == 0) {
            readInputsBlock(file, bmdFile.inputs);
		} else if(strcmp(header, "TATL") == 0) {
			readAtlasesBlock(file, bmdFile.atlases);
        } else if(strcmp(header, "TSET") == 0) {
            readTextureSetsBlock(file, bmdFile.textureSets);
		} else if(strcmp(header, "SMOD") == 0) {
			readSubmodelsBlock(file, bmdFile.submodels);
		} else if(strcmp(header, "VIEW") == 0) {
			readViewsBlock(file, bmdFile.views);
		} else {
			spdlog::warn("BMD file {}: unknown block {}", bmdFile.info.name, header);
			file.seekg(blockSize, std::ios::cur);
		}

		currentPos += blockSize + 8;

        int32_t tellg = file.tellg();
        if (currentPos != tellg) {
            spdlog::error("BMD file {}: invalid block {}", bmdFile.info.name, header);
            return false;
		}
	}

    return true;
}

bool SGE_BMD_Reader::readInfoBlock(std::ifstream& file, SGE_BMD_InfoBlock& infoBlock) {
    _READ(infoBlock.version);
    _READ(infoBlock.nameLength);
    _READ_STR(infoBlock.name, infoBlock.nameLength);
    _READ(infoBlock.originX);
    _READ(infoBlock.originY);

    return true;
}

bool SGE_BMD_Reader::readInputsBlock(std::ifstream& file, SGE_BMD_InputsBlock& inputsBlock) {
	_READ(inputsBlock.inputsCount);
	inputsBlock.inputs = new SGE_BMD_InputDef[inputsBlock.inputsCount];

    for (int i = 0; i < inputsBlock.inputsCount; ++i) {
		readInputDef(file, inputsBlock.inputs[i]);
	}

	return true;
}

bool SGE_BMD_Reader::readInputDef(std::ifstream& file, SGE_BMD_InputDef& inputDef)
{
    _READ(inputDef.nameLength);
	_READ_STR(inputDef.name, inputDef.nameLength);

	return true;
}

bool SGE_BMD_Reader::readAtlasesBlock(std::ifstream& file, SGE_BMD_AtlasesBlock& tilemapsBlock) {
    _READ(tilemapsBlock.atlasesCount);
    tilemapsBlock.atlases = new SGE_BMD_AtlasDef[tilemapsBlock.atlasesCount];

    for (int i = 0; i < tilemapsBlock.atlasesCount; ++i) {
        readAtlasDef(file, tilemapsBlock.atlases[i]);
    }

    return true;
}

bool SGE_BMD_Reader::readAtlasDef(std::ifstream& file, SGE_BMD_AtlasDef& atlasDef) {
    _READ(atlasDef.baseImageNameLength);
    _READ_STR(atlasDef.baseImageName, atlasDef.baseImageNameLength);

    return true;
}

bool SGE_BMD_Reader::readTextureSetsBlock(std::ifstream& file, SGE_BMD_TextureSetsBlock& tileSetsBlock)
{
    _READ(tileSetsBlock.textureSetsCount);
    tileSetsBlock.textureSets = new SGE_BMD_TextureSetDef[tileSetsBlock.textureSetsCount];

    for (int i = 0; i < tileSetsBlock.textureSetsCount; ++i) {
        readTextureSetDef(file, tileSetsBlock.textureSets[i]);
    }

    return true;
}

bool SGE_BMD_Reader::readTextureSetDef(std::ifstream& file, SGE_BMD_TextureSetDef& textureSetDef) {
    _READ(textureSetDef.atlasIndex);
    _READ(textureSetDef.texturesCount);
    textureSetDef.textures = new SGE_BMD_TextureDef[textureSetDef.texturesCount];

    for (int i = 0; i < textureSetDef.texturesCount; ++i) {
        readTextureDef(file, textureSetDef.textures[i]);
    }

    return true;
}

bool SGE_BMD_Reader::readTextureDef(std::ifstream& file, SGE_BMD_TextureDef& textureDef) {
    _READ(textureDef.index);
    _READ(textureDef.dstX);
    _READ(textureDef.dstY);
    _READ(textureDef.width);
    _READ(textureDef.height);
    _READ(textureDef.srcX);
    _READ(textureDef.srcY);

    return true;
}

bool SGE_BMD_Reader::readSubmodelsBlock(std::ifstream& file, SGE_BMD_SubmodelsBlock& submodelsBlock) {
    _READ(submodelsBlock.submodelsCount);
    submodelsBlock.submodels = new SGE_BMD_SubmodelDef[submodelsBlock.submodelsCount];

    for (int i = 0; i < submodelsBlock.submodelsCount; ++i) {
        readSubmodelDef(file, submodelsBlock.submodels[i]);
    }

    return true;
}

bool SGE_BMD_Reader::readSubmodelDef(std::ifstream& file, SGE_BMD_SubmodelDef& submodelDef) {
    _READ(submodelDef.nameLength);
    _READ_STR(submodelDef.name, submodelDef.nameLength);
    _READ(submodelDef.textureSetIndex);
    _READ(submodelDef.indexByInputIndex);
    _READ(submodelDef.indexRange);
    _READ(submodelDef.indexOffset);
    _READ(submodelDef.conditionInputIndex);
    _READ(submodelDef.conditionValue);
    _READ(submodelDef.rotByInputIndex);
    _READ(submodelDef.x);
    _READ(submodelDef.y);

    return true;
}

bool SGE_BMD_Reader::readViewsBlock(std::ifstream& file, SGE_BMD_ViewsBlock& viewsBlock) {
    _READ(viewsBlock.indexByInputIndex);
    _READ(viewsBlock.viewsCount);
    viewsBlock.views = new SGE_BMD_ViewDef[viewsBlock.viewsCount];

    for (int i = 0; i < viewsBlock.viewsCount; ++i) {
        readViewDef(file, viewsBlock.views[i]);
    }

    return true;
}

bool SGE_BMD_Reader::readViewDef(std::ifstream& file, SGE_BMD_ViewDef& viewDef) {
    _READ(viewDef.submodelsCount);
    viewDef.submodels = new uint16_t[viewDef.submodelsCount];
    _READ_ARR(viewDef.submodels, viewDef.submodelsCount);

    return true;
}