#include "model_controller.h"
#include "../assets/model_def.h"

SGE_ModelController::SGE_ModelController(std::string filepath, std::string assetsDirpath)
{
	mp_bmdFilepath = filepath;
	mp_assetsDirpath = assetsDirpath;
}

SGE_ModelController::~SGE_ModelController()
{

}

void SGE_ModelController::init()
{
	m_bmdFilepath = mp_bmdFilepath;
	m_assetsDirpath = mp_assetsDirpath;

	updateModelDef();
	addSubmodelControllers();

	RZUF3_EventsManager* eventsManager = this->m_object->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
	RZUF3_EventsManager* objEventsManager = this->m_object->getEventsManager();
	_ADD_LISTENER_CL(objEventsManager, SetModelInput, SGE);
}

void SGE_ModelController::deinit()
{
	RZUF3_EventsManager* eventsManager = this->m_object->getScene()->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);
	RZUF3_EventsManager* objEventsManager = this->m_object->getEventsManager();
	_REMOVE_LISTENER_CL(objEventsManager, SetModelInput, SGE);

	removeModelDef();
	removeSubmodelControllers();

	m_bmdFile = nullptr;
}

void SGE_ModelController::setFilepath(std::string filepath)
{
	removeModelDef();
	removeSubmodelControllers();
	m_bmdFilepath = filepath;
	updateModelDef();
	addSubmodelControllers();
}

bool SGE_ModelController::setInput(uint16_t index, uint8_t value)
{
	if (m_bmdFile == nullptr) return false;
	if (index == 0xFFFF) return true;

	if (index > m_bmdFile->inputs.inputsCount) {
		spdlog::error(
			"Input #{} does not exist in the model {}",
			index,
			m_bmdFilepath
		);

		return false;
	}

	m_inputs[index] = value;

	return true;
}

bool SGE_ModelController::setInput(std::string name, uint8_t value)
{
	if (m_bmdFile == nullptr) return false;
	if (name.empty()) return false;

	auto it = m_inputNames.find(name);
	if (it == m_inputNames.end()) {
		spdlog::error(
			"Input {} does not exist in the model {}",
			name,
			m_bmdFile->info.name
		);

		return false;
	}

	uint16_t index = it->second;
	m_inputs[index] = value;

	return true;
}

uint8_t SGE_ModelController::getInput(uint16_t index)
{
	if (m_bmdFile == nullptr) return 0;
	if (index == 0xFFFF) return 0;

	if (index > m_bmdFile->inputs.inputsCount) {
		spdlog::error(
			"Input #{} does not exist in the model {}",
			index,
			m_bmdFile->info.name
		);

		return 0;
	}

	return m_inputs[index];
}

void SGE_ModelController::onDraw(RZUF3_DrawEvent* event)
{
	drawSubmodels();
}

void SGE_ModelController::onSetModelInput(SGE_SetModelInputEvent* event)
{
	setInput(event->getName(), event->getValue());
}

void SGE_ModelController::removeModelDef()
{
	if (m_bmdFile == nullptr) return;

	delete[] m_inputs;
	m_inputNames.clear();

	RZUF3_AssetsManager* assetsManager = this->m_object->getScene()->getAssetsManager();
	assetsManager->removeAsset(m_bmdFilepath);
	m_bmdFile = nullptr;
}

void SGE_ModelController::updateModelDef()
{

	if (this->m_object == nullptr) return;
	if (this->m_bmdFilepath == "") return;
	if (this->m_bmdFile != nullptr) return;

	RZUF3_AssetsManager* assetsManager = this->m_object->getScene()->getAssetsManager();

	RZUF3_AssetDefinition assetDefinition;
	assetDefinition.filepath = m_bmdFilepath;
	assetDefinition.factory = SGE_ModelDef::getInstance;

	if (!assetsManager->addAsset(assetDefinition)) return;

	this->m_bmdFile = (SGE_BMD_File*)assetsManager->getAssetContent(m_bmdFilepath);

	if (this->m_bmdFile == nullptr)
	{
		spdlog::error("BMD File {} not found", this->m_bmdFilepath);
		return;
	}

	m_inputs = new uint8_t[this->m_bmdFile->inputs.inputsCount]{ 0 };

	for (int i = 0; i < this->m_bmdFile->inputs.inputsCount; i++)
	{
		SGE_BMD_InputDef* input = &this->m_bmdFile->inputs.inputs[i];
		m_inputNames.insert(std::pair<std::string, uint16_t>(input->name, i));
	}
}

void SGE_ModelController::removeSubmodelControllers()
{
	for(auto& submodelRenderer : m_submodelRenderers)
	{
		getObject()->removeScript(submodelRenderer);
		delete submodelRenderer;
	}

	m_submodelRenderers.clear();
}

void SGE_ModelController::addSubmodelControllers()
{
	if (m_object == nullptr) return;
	if (m_bmdFile == nullptr) return;
	if (m_submodelRenderers.size() > 0) return;

	for (int i = 0; i < m_bmdFile->submodels.submodelsCount; i++)
	{
		SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[i];
		SGE_BMD_TextureSetDef* textureSet = &m_bmdFile->textureSets.textureSets[submodel->textureSetIndex];
		SGE_BMD_AtlasDef* atlas = &m_bmdFile->atlases.atlases[textureSet->atlasIndex];

		std::string baseImageFilepath = m_assetsDirpath + "/" + atlas->baseImageName;

		SGE_TextureSetRenderer* submodelRenderer = new SGE_TextureSetRenderer(
			baseImageFilepath,
			textureSet
		);

		getObject()->addScript(submodelRenderer);
		submodelRenderer->setUserDrawOnly(true);

		m_submodelRenderers.push_back(submodelRenderer);
	}
}

void SGE_ModelController::drawSubmodels()
{
	if(!m_bmdFile) return;

	uint8_t viewInputValue = getInput(m_bmdFile->views.indexByInputIndex);
	uint8_t viewIndex = (viewInputValue * m_bmdFile->views.viewsCount / 120) % m_bmdFile->views.viewsCount;
	SGE_BMD_ViewDef* view = &m_bmdFile->views.views[viewIndex];

	for(int i = 0; i < view->submodelsCount; i++)
	{
		uint16_t submodelIndex = view->submodels[i];
		drawSubmodel(submodelIndex);
	}
}

const double yScale = cos(75.0 * M_PI / 180.0);

void SGE_ModelController::drawSubmodel(uint16_t submodelIndex)
{
	SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[submodelIndex];
	SGE_TextureSetRenderer* submodelRenderer = m_submodelRenderers[submodelIndex];

	if (submodel->conditionInputIndex != 0) {
		uint8_t conditionInputValue = getInput(submodel->conditionInputIndex);

		if (submodel->conditionValue == 255) {
			submodelRenderer->setOpacity(conditionInputValue);
		}
		else if (submodel->conditionValue != conditionInputValue) {
			return;
		}
	}

	uint16_t index = getInput(submodel->indexByInputIndex);
	uint8_t rot = getInput(submodel->rotByInputIndex);

	index += submodel->indexOffset;
	index += rot;
	index %= submodel->indexRange;

	submodelRenderer->setIndex((uint8_t)index);

	double rotRad = rot * M_PI / 60.0;
	double x = submodel->y * cos(rotRad) - submodel->x * sin(rotRad);
	double y = submodel->y * sin(rotRad) + submodel->x * cos(rotRad);

	submodelRenderer->setDstPos(
		x / SGE_BMD_FLOAT_SCALE,
		y * yScale / SGE_BMD_FLOAT_SCALE
	);
	submodelRenderer->userDraw();
}