#include "bmd_destroyer.h"

void SGE_BMD_Destroyer::destroy(SGE_BMD_File& bmdFile)
{
	// header
	delete[] bmdFile.header;

	// info block
	delete[] bmdFile.info.name;

	// inputs block
	for (int i = 0; i < bmdFile.inputs.inputsCount; i++)
	{
		delete[] bmdFile.inputs.inputs[i].name;
	}
	delete[] bmdFile.inputs.inputs;

	// atlases block
	for (int i = 0; i < bmdFile.atlases.atlasesCount; i++)
	{
		delete[] bmdFile.atlases.atlases[i].baseImagePath;
	}
	delete[] bmdFile.atlases.atlases;

	// texture sets block
	for (int i = 0; i < bmdFile.textureSets.textureSetsCount; i++)
	{
		delete[] bmdFile.textureSets.textureSets[i].textures;
	}
	delete[] bmdFile.textureSets.textureSets;

	// submodels block
	for (int i = 0; i < bmdFile.submodels.submodelsCount; i++)
	{
		delete[] bmdFile.submodels.submodels[i].name;
	}
	delete[] bmdFile.submodels.submodels;

	// views block
	for (int i = 0; i < bmdFile.views.viewsCount; i++)
	{
		delete[] bmdFile.views.views[i].submodels;
	}
	delete[] bmdFile.views.views;
}
