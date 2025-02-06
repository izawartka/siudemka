#include "model_controller.h"
#include "world_draw_queue.h"
#include "../assets/model_def.h"
#include "../point_utils.h"

SGE_ModelController::SGE_ModelController(std::string filepath)
{
	mp_options.bmdFilepath = filepath;
}

SGE_ModelController::SGE_ModelController(SGE_ModelControllerOptions options)
{
	mp_options = options;
}

SGE_ModelController::~SGE_ModelController()
{

}

void SGE_ModelController::init()
{
	m_centerAtOrigin = mp_options.centerAtOrigin;
	m_layer = mp_options.layer;
	m_worldPosition = mp_options.worldPosition;
	setFilepath(mp_options.bmdFilepath);

	uint16_t rotBaseIndex = getInputIndex("rot_base");
	setInput(rotBaseIndex, mp_options.startRotBase);

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
	RZUF3_EventsManager* objEventsManager = this->m_object->getEventsManager();
	_ADD_LISTENER_CL(objEventsManager, SetModelInput, SGE);
	_ADD_LISTENER_CL(objEventsManager, SetModelAngleInput, SGE);
	_ADD_LISTENER_CL(objEventsManager, SetWorldPosition, SGE);
}

void SGE_ModelController::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);
	RZUF3_EventsManager* objEventsManager = this->m_object->getEventsManager();
	_REMOVE_LISTENER_CL(objEventsManager, SetModelInput, SGE);
	_REMOVE_LISTENER_CL(objEventsManager, SetModelAngleInput, SGE);
	_REMOVE_LISTENER_CL(objEventsManager, SetWorldPosition, SGE);

	removeModelDef();
	removeSubmodelControllers();

	m_bmdFile = nullptr;
}

void SGE_ModelController::setFilepath(std::string filepath)
{
	removeModelDef();
	removeSubmodelControllers();
	m_bmdFilepath = filepath;
	createModelDef();
	createSubmodelControllers();
}

uint16_t SGE_ModelController::getInputIndex(std::string name)
{
	if (m_bmdFile == nullptr) return 0xFFFF;
	if (name.empty()) return 0xFFFF;

	auto it = m_inputNames.find(name);
	if (it == m_inputNames.end()) {
		spdlog::error(
			"Input {} does not exist in the model {}",
			name,
			m_bmdFile->info.name
		);

		return 0xFFFF;
	}

	return it->second;
}

bool SGE_ModelController::setInput(uint16_t index, uint16_t value)
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

bool SGE_ModelController::setAngleInput(uint16_t index, double angle)
{
	if(m_bmdFile == nullptr) return false;
	int fullAngle = m_bmdFile->info.fullAngle;

	int value = angle / (2 * M_PI) * fullAngle;
	if (value < 0) value += fullAngle * (-value / fullAngle + 1);
	value %= fullAngle;

	return setInput(index, (uint16_t)value);
}

uint16_t SGE_ModelController::getInput(uint16_t index)
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

void SGE_ModelController::setLayer(int layer)
{
	m_layer = layer;
}

void SGE_ModelController::setPosition(SGE_Point position)
{
	m_worldPosition = position;
}

void SGE_ModelController::drawSubmodels()
{
	if (!m_bmdFile) return;

	uint16_t viewInputValue = getInput(m_bmdFile->views.indexByInputIndex);
	SGE_BMD_ViewDef* view = nullptr;
	uint8_t viewIndex = 0;
	for (viewIndex = 0; viewIndex < m_bmdFile->views.viewsCount; viewIndex++)
	{
		view = &m_bmdFile->views.views[viewIndex];
		if (view->endIndex >= viewInputValue) break;
	}

	if (view == nullptr) return;

	for (int i = 0; i < view->submodelsCount; i++)
	{
		uint16_t submodelIndex = view->submodels[i];
		drawSubmodel(submodelIndex);
	}
}

void SGE_ModelController::onDraw(RZUF3_DrawEvent* event)
{
	if (m_bmdFile == nullptr) return;
	if (g_sgeWorldDrawQueue == nullptr || !mp_options.useDrawQueue)
	{
		drawSubmodels();
		return;
	}

	SGE_Point screenPos = m_worldPosition;
	SGE_PointUtils::worldToScreen(screenPos);
	g_sgeWorldDrawQueue->addToQueue(this, m_layer, screenPos.z);
}

void SGE_ModelController::onSetModelInput(SGE_SetModelInputEvent* event)
{
	uint16_t inputIndex = getInputIndex(event->getName());
	setInput(inputIndex, event->getValue());
}

void SGE_ModelController::onSetModelAngleInput(SGE_SetModelAngleInputEvent* event)
{
	uint16_t inputIndex = getInputIndex(event->getName());
	setAngleInput(inputIndex, event->getAngle());
}

void SGE_ModelController::onSetWorldPosition(SGE_SetWorldPositionEvent* event)
{
	setPosition(event->getPosition());
}

void SGE_ModelController::removeModelDef()
{
	if (m_bmdFile == nullptr) return;

	delete[] m_inputs;
	m_inputNames.clear();

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();
	assetsManager->removeAsset(m_bmdFilepath);
	m_bmdFile = nullptr;
}

void SGE_ModelController::createModelDef()
{
	if (this->m_object == nullptr) return;
	if (this->m_bmdFilepath.empty()) return;
	if (this->m_bmdFile != nullptr) return;

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();

	RZUF3_AssetDefinition assetDefinition;
	assetDefinition.filepath = m_bmdFilepath;
	assetDefinition.factory = SGE_ModelDef::getInstance;

	if (!assetsManager->addAsset(assetDefinition)) return;

	this->m_bmdFile = (SGE_BMD_File*)assetsManager->getAssetContent(m_bmdFilepath);

	if (this->m_bmdFile == nullptr)
	{
		spdlog::error("BMD File {} could not be loaded", this->m_bmdFilepath);
		return;
	}

	m_inputs = new uint16_t[this->m_bmdFile->inputs.inputsCount]{ 0 };

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
		m_object->removeScript(submodelRenderer);
		delete submodelRenderer;
	}

	m_submodelRenderers.clear();
}

void SGE_ModelController::createSubmodelControllers()
{
	if (m_object == nullptr) return;
	if (m_bmdFile == nullptr) return;
	if (m_submodelRenderers.size() > 0) return;

	for (int i = 0; i < m_bmdFile->submodels.submodelsCount; i++)
	{
		SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[i];
		SGE_BMD_TextureSetDef* textureSet = &m_bmdFile->textureSets.textureSets[submodel->textureSetIndex];
		SGE_BMD_AtlasDef* atlas = &m_bmdFile->atlases.atlases[textureSet->atlasIndex];

		std::string baseImageFilepath = SGE_TEXTURES_ROOT + std::string(atlas->baseImagePath);

		SGE_TextureSetRendererOptions options;
		options.filepath = baseImageFilepath;
		options.setDef = textureSet;
		options.useOnDraw = false;

		SGE_TextureSetRenderer* submodelRenderer = new SGE_TextureSetRenderer(options);
		m_object->addScript(submodelRenderer);
		m_submodelRenderers.push_back(submodelRenderer);
	}
}

const double yScale = cos(SGE_CAMERA_ISO_ANGLE);

void SGE_ModelController::drawSubmodel(uint16_t submodelIndex)
{
	SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[submodelIndex];
	SGE_TextureSetRenderer* submodelRenderer = m_submodelRenderers[submodelIndex];

	if (submodel->conditionInputIndex != 0) {
		uint16_t conditionInputValue = getInput(submodel->conditionInputIndex);

		if (submodel->conditionValue == 0xFFFF) {
			submodelRenderer->setOpacity(conditionInputValue);
		}
		else if (submodel->conditionValue != conditionInputValue) {
			return;
		}
	}

	int index = getInput(submodel->indexByInputIndex);
	uint16_t rot = getInput(submodel->rotByInputIndex);

	index += submodel->indexOffset;
	index += rot;
	index %= submodel->indexRange;

	submodelRenderer->setIndex((uint16_t)index);

	double rotRad = rot * M_PI / 60.0;
	SGE_Point pos = m_worldPosition;
	SGE_PointUtils::worldToScreen(pos);

	double offX = submodel->y * cos(rotRad) - submodel->x * sin(rotRad);
	double offY = submodel->y * sin(rotRad) + submodel->x * cos(rotRad);
	offY *= yScale;

	pos.x += offX / SGE_BMD_FLOAT_SCALE;
	pos.y += offY / SGE_BMD_FLOAT_SCALE;

	if (m_centerAtOrigin) {
		pos.x -= (double)m_bmdFile->info.originX / SGE_BMD_FLOAT_SCALE;
		pos.y -= (double)m_bmdFile->info.originY / SGE_BMD_FLOAT_SCALE;
	}

	submodelRenderer->setDstPos(
		std::round(pos.x),
		std::round(pos.y)
	);
	submodelRenderer->draw();
}