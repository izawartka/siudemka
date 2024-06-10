#include "bmd_destroyer.h"

// example:
// _DEL_ARR(bmdFile.inputs.inputs[i], name, nameLength);
// delete[bmdFile.inputs.inputs[i].nameLength] bmdFile.inputs.inputs[i].name;
#define _DEL_ARR(x, y, z) delete[x##.##z] x##.##y

void SGE_BMD_Destroyer::destroy(SGE_BMD_File& bmdFile)
{
	// header
	delete[6] bmdFile.header;

	// info block
	_DEL_ARR(bmdFile.info, name, nameLength);

	// inputs block
	for (int i = 0; i < bmdFile.inputs.inputsCount; i++)
	{
		_DEL_ARR(bmdFile.inputs.inputs[i], name, nameLength);
	}
	_DEL_ARR(bmdFile.inputs, inputs, inputsCount);

	// atlases block
	for (int i = 0; i < bmdFile.atlases.atlasesCount; i++)
	{
		_DEL_ARR(bmdFile.atlases.atlases[i], baseImageName, baseImageNameLength);
	}
	_DEL_ARR(bmdFile.atlases, atlases, atlasesCount);

	// texture sets block
	for (int i = 0; i < bmdFile.textureSets.textureSetsCount; i++)
	{
		_DEL_ARR(bmdFile.textureSets.textureSets[i], textures, texturesCount);
	}
	_DEL_ARR(bmdFile.textureSets, textureSets, textureSetsCount);

	// submodels block
	for (int i = 0; i < bmdFile.submodels.submodelsCount; i++)
	{
		_DEL_ARR(bmdFile.submodels.submodels[i], name, nameLength);
	}
	_DEL_ARR(bmdFile.submodels, submodels, submodelsCount);

	// views block
	for (int i = 0; i < bmdFile.views.viewsCount; i++)
	{
		_DEL_ARR(bmdFile.views.views[i], submodels, submodelsCount);
	}
	_DEL_ARR(bmdFile.views, views, viewsCount);
}
