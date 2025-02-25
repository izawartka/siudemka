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
	m_options = mp_options;
	setFilepath(mp_options.bmdFilepath);
	setStartBaseRotation();

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

	removeCacheTexture();
	removeModelDef();
	removeSubmodelControllers();

	m_bmdFile = nullptr;
}

void SGE_ModelController::setFilepath(std::string filepath)
{
	removeModelDef();
	removeSubmodelControllers();
	m_options.bmdFilepath = filepath;
	createModelDef();
	createSubmodelControllers();

	m_cacheTextureNeedsUpdate = true;
}

bool SGE_ModelController::setInput(uint16_t index, uint16_t value)
{
	if (m_bmdFile == nullptr) return false;
	if (index == 0xFFFF) return true;

	if (index > m_bmdFile->inputs.inputsCount) {
		spdlog::error(
			"Input #{} does not exist in the model {}",
			index,
			m_options.bmdFilepath
		);

		return false;
	}

	if (m_inputs[index] != value) {
		m_cacheTextureNeedsUpdate = true;
	}

	m_inputs[index] = value;

	return true;
}

bool SGE_ModelController::setAngleInput(uint16_t index, double angle)
{
	if(m_bmdFile == nullptr) return false;
	int fullAngle = m_bmdFile->info.fullAngle;

	int value = std::round(angle / (2.0 * M_PI) * fullAngle);
	if (value < 0) value += fullAngle * (-value / fullAngle + 1);
	value %= fullAngle;

	return setInput(index, (uint16_t)value);
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
	m_options.layer = layer;
}

void SGE_ModelController::setPosition(SGE_Point position)
{
	m_options.worldPosition = position;
}

void SGE_ModelController::setUseCacheTexture(bool useCache)
{
	m_options.cacheTexture = useCache;
	m_cacheTextureNeedsUpdate = true;

	if(!useCache) removeCacheTexture();
}

void SGE_ModelController::draw()
{
	if (m_bmdFile == nullptr) return;

	if (m_options.cacheTexture) drawFromCache();
	else drawNoCache();
}

void SGE_ModelController::onDraw(RZUF3_DrawEvent* event)
{
	if (m_bmdFile == nullptr) return;
	if (g_sgeWorldDrawQueue == nullptr || !mp_options.useDrawQueue)
	{
		draw();
		return;
	}

	SGE_Point screenPos = m_options.worldPosition;
	SGE_PointUtils::worldToScreen(screenPos);
	g_sgeWorldDrawQueue->addToQueue(this, m_options.layer, screenPos.z);
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
	assetsManager->removeAsset(m_options.bmdFilepath);
	m_bmdFile = nullptr;
}

void SGE_ModelController::createModelDef()
{
	if (this->m_object == nullptr) return;
	if (this->m_bmdFile != nullptr) return;

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();

	RZUF3_AssetDefinition assetDefinition;
	assetDefinition.filepath = m_options.bmdFilepath;
	assetDefinition.factory = SGE_ModelDef::getInstance;

	if (!assetsManager->addAsset(assetDefinition)) return;

	this->m_bmdFile = (SGE_BMD_File*)assetsManager->getAssetContent(m_options.bmdFilepath);

	if (this->m_bmdFile == nullptr)
	{
		spdlog::error("BMD File {} could not be loaded", m_options.bmdFilepath);
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

void SGE_ModelController::removeCacheTexture()
{
	m_cacheTextureNeedsUpdate = true;
	m_cacheTextureBounds = { 0, 0, 0, 0 };

	if(!m_cacheTexture) return;
	SDL_DestroyTexture(m_cacheTexture);
	m_cacheTexture = nullptr;
}

void SGE_ModelController::createCacheTexture()
{
	if(m_cacheTexture) return;
	m_cacheTextureNeedsUpdate = false;
	m_cacheTextureBounds = { 0, 0, 0, 0 };

	SGE_BMD_ViewDef* view = getCurrentView();
	if (view == nullptr) return;

	SDL_Rect& bounds = m_cacheTextureBounds;
	bounds = { SGE_INT_MAX, SGE_INT_MAX, SGE_INT_MIN, SGE_INT_MIN };
	SDL_Rect tempRect{};

	for (int i = 0; i < view->submodelsCount; i++)
	{
		uint16_t submodelIndex = view->submodels[i];
		updateSubmodel(submodelIndex, &tempRect);

		if(tempRect.w == 0 || tempRect.h == 0) continue;

		if (tempRect.x < bounds.x) bounds.x = tempRect.x;
		if (tempRect.y < bounds.y) bounds.y = tempRect.y;
		if (tempRect.x + tempRect.w > bounds.w) bounds.w = tempRect.x + tempRect.w;
		if (tempRect.y + tempRect.h > bounds.h) bounds.h = tempRect.y + tempRect.h;
	}

	// convert x1, y1, x2, y2 to x, y, w, h
	bounds.w -= bounds.x;
	bounds.h -= bounds.y;

	if (bounds.w == 0 || bounds.h == 0) return;

	g_renderer->createCacheTexture(m_cacheTexture, bounds.w, bounds.h, [this, view]() {
		for (int i = 0; i < view->submodelsCount; i++)
		{
			uint16_t submodelIndex = view->submodels[i];
			drawSubmodel(submodelIndex, true);
		}
	});
}

void SGE_ModelController::drawNoCache()
{
	SGE_BMD_ViewDef* view = getCurrentView();
	if (view == nullptr) return;

	for (int i = 0; i < view->submodelsCount; i++)
	{
		uint16_t submodelIndex = view->submodels[i];
		updateSubmodel(submodelIndex, nullptr);
		drawSubmodel(submodelIndex, false);
	}
}

void SGE_ModelController::drawFromCache()
{
	if (m_cacheTextureNeedsUpdate)
	{
		removeCacheTexture();
		createCacheTexture();
	}

	if (!m_cacheTexture) return;

	g_renderer->setUseObjectPos(true);
	g_renderer->setAlign(RZUF3_Align_TopLeft);

	SGE_Point screenPos = m_options.worldPosition;
	SGE_PointUtils::worldToScreen(screenPos);

	SDL_Rect dstRect = {
		m_cacheTextureBounds.x + (int)(screenPos.x + 0.5),
		m_cacheTextureBounds.y + (int)(screenPos.y + 0.5),
		m_cacheTextureBounds.w,
		m_cacheTextureBounds.h
	};

	g_renderer->drawTexture(m_object, m_cacheTexture, nullptr, dstRect);
}

void SGE_ModelController::updateSubmodel(uint16_t submodelIndex, SDL_Rect* rect)
{
	SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[submodelIndex];
	SGE_TextureSetRenderer* submodelRenderer = m_submodelRenderers[submodelIndex];

	if (submodel->conditionInputIndex != 0) {
		uint16_t conditionInputValue = getInput(submodel->conditionInputIndex);

		if (submodel->conditionValue == 0xFFFF) {
			submodelRenderer->setOpacity(conditionInputValue);
		}
		else {
			bool conditionMet = submodel->conditionValue == conditionInputValue;
			submodelRenderer->setOpacity(conditionMet ? 0xFF : 0x00);

			if (!conditionMet) {
				if(rect != nullptr) *rect = { 0, 0, 0, 0 };
				return;
			}
		}
	}

	int index = getInput(submodel->indexByInputIndex);
	int rot = getInput(submodel->rotByInputIndex);

	index += rot;
	index /= submodel->indexDivisor;
	index += submodel->indexOffset;
	index %= submodel->indexRange;

	submodelRenderer->setIndex((uint16_t)index);

	double rotRad = ((double)rot / m_bmdFile->info.fullAngle) * 2.0 * M_PI;

	double submodelX = (double)submodel->x / SGE_BMD_FLOAT_SCALE;
	double submodelY = (double)submodel->y / SGE_BMD_FLOAT_SCALE;

	SGE_Point pos;
	pos.x += submodelY * cos(rotRad) - submodelX * sin(rotRad);
	pos.y += (submodelY * sin(rotRad) + submodelX * cos(rotRad)) * SGE_Y_SCALE;

	if (m_options.centerAtOrigin) {
		pos.x -= ((double)m_bmdFile->info.originX / SGE_BMD_FLOAT_SCALE);
		pos.y -= ((double)m_bmdFile->info.originY / SGE_BMD_FLOAT_SCALE);
	}

	submodelRenderer->setDstPos(
		std::round(pos.x),
		std::round(pos.y)
	);

	if (rect != nullptr) submodelRenderer->getRect(*rect);
}

void SGE_ModelController::drawSubmodel(uint16_t submodelIndex, bool forCache)
{
	SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[submodelIndex];
	SGE_TextureSetRenderer* submodelRenderer = m_submodelRenderers[submodelIndex];

	int x, y;
	submodelRenderer->getPosition(x, y);

	// if drawing for cache, subtract the cache texture bounds
	if (forCache) {
		x -= m_cacheTextureBounds.x;
		y -= m_cacheTextureBounds.y;

		submodelRenderer->setDstPos(x, y);
	}
	// if drawing normally, add the world position
	else {
		SGE_Point screenPos = m_options.worldPosition;
		SGE_PointUtils::worldToScreen(screenPos);

		// round to prevent submodels from woubleing
		submodelRenderer->setDstPos(
			x + (int)(screenPos.x + 0.5),
			y + (int)(screenPos.y + 0.5)
		);
	}

	submodelRenderer->draw();
}

void SGE_ModelController::setStartBaseRotation()
{
	if (m_bmdFile == nullptr) return;

	uint16_t baseRotationIndex = getInputIndex(SGE_BASE_ROTATION_NAME);
	if (m_options.startBaseRotationRad != 0.0) {
		setAngleInput(baseRotationIndex, m_options.startBaseRotationRad);
	}
	else {
		setInput(baseRotationIndex, m_options.startBaseRotation);
	}
}

SGE_BMD_ViewDef* SGE_ModelController::getCurrentView()
{
	if (m_bmdFile == nullptr) return nullptr;

	uint16_t viewInputValue = getInput(m_bmdFile->views.indexByInputIndex);
	SGE_BMD_ViewDef* view = nullptr;
	uint8_t viewIndex = 0;
	for (viewIndex = 0; viewIndex < m_bmdFile->views.viewsCount; viewIndex++)
	{
		view = &m_bmdFile->views.views[viewIndex];
		if (view->endIndex >= viewInputValue) break;
	}

	return view;
}
