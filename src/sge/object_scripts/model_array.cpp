#include "model_array.h"
#include "../events/events.h"
#include "../point_utils.h"

SGE_ModelArray::SGE_ModelArray(SGE_ModelArrayOptions options)
{
	mp_options = options;
}

SGE_ModelArray::~SGE_ModelArray()
{

}

void SGE_ModelArray::init()
{
	m_options = mp_options;
	setFilepath(mp_options.bmdFilepath);
	setStartBaseRotations();

	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);

	RZUF3_EventsManager* objEventsManager = this->m_object->getEventsManager();
	_ADD_LISTENER_CL(objEventsManager, SetModelInput, SGE);
	_ADD_LISTENER_CL(objEventsManager, SetModelAngleInput, SGE);
	_ADD_LISTENER_CL(objEventsManager, SetWorldPosition, SGE);
}

void SGE_ModelArray::deinit()
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

void SGE_ModelArray::setFilepath(std::string filepath)
{
	removeModelDef();
	removeSubmodelControllers();
	m_options.bmdFilepath = filepath;
	createModelDef();
	createSubmodelControllers();

	m_cacheTextureNeedsUpdate = true;
}

bool SGE_ModelArray::setInput(int itemIndex, uint16_t inputIndex, uint16_t value)
{
	if (m_bmdFile == nullptr) return false;
	if (inputIndex == 0xFFFF) return true;

	if (!checkItemIndex(itemIndex)) return false;
	if (!checkInputIndex(inputIndex)) return false;
	uint16_t& input = m_items[itemIndex].inputs[inputIndex];

	if (input != value) {
		m_cacheTextureNeedsUpdate = true;
	}

	input = value;

	return true;
}

bool SGE_ModelArray::setAllItemsInput(uint16_t inputIndex, uint16_t value)
{
	if (m_bmdFile == nullptr) return false;
	if (inputIndex == 0xFFFF) return true;

	if(!checkInputIndex(inputIndex)) return false;

	for (int i = 0; i < m_items.size(); i++) {
		uint16_t& input = m_items[i].inputs[inputIndex];
		if (input != value) {
			m_cacheTextureNeedsUpdate = true;
		}

		input = value;
	}

	return true;
}

bool SGE_ModelArray::setAngleInput(int itemIndex, uint16_t inputIndex, double angle)
{
	if (m_bmdFile == nullptr) return false;
	int fullAngle = m_bmdFile->info.fullAngle;

	int value = std::round(angle / (2.0 * M_PI) * fullAngle);
	if (value < 0) value += fullAngle * (-value / fullAngle + 1);
	value %= fullAngle;

	return setInput(itemIndex, inputIndex, (uint16_t)value);
}

bool SGE_ModelArray::setAllItemsAngleInput(uint16_t inputIndex, double angle)
{
	if (m_bmdFile == nullptr) return false;
	int fullAngle = m_bmdFile->info.fullAngle;

	int value = std::round(angle / (2.0 * M_PI) * fullAngle);
	if (value < 0) value += fullAngle * (-value / fullAngle + 1);
	value %= fullAngle;

	return setAllItemsInput(inputIndex, (uint16_t)value);
}

bool SGE_ModelArray::setItemPosition(int itemIndex, SGE_Point relPosition)
{
	if (m_bmdFile == nullptr) return false;

	if (!checkItemIndex(itemIndex)) return false;
	SGE_ModelArrayItemOptions& itemOptions = m_options.items[itemIndex];

	if (itemOptions.worldRelPosition != relPosition) {
		m_cacheTextureNeedsUpdate = true;
	}

	itemOptions.worldRelPosition = relPosition;

	return true;
}

void SGE_ModelArray::setPosition(SGE_Point position)
{
	m_options.worldPosition = position;
}

void SGE_ModelArray::setLayer(int layer)
{
	if (m_bmdFile == nullptr) return;

	if (m_options.layer != layer) {
		m_cacheTextureNeedsUpdate = true;
	}

	m_options.layer = layer;
}

uint16_t SGE_ModelArray::getInputIndex(std::string name)
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

uint16_t SGE_ModelArray::getInput(int itemIndex, uint16_t inputIndex)
{
	if (m_bmdFile == nullptr) return 0;
	if (inputIndex == 0xFFFF) return 0;

	if (!checkItemIndex(itemIndex)) return 0;

	return m_items[itemIndex].inputs[inputIndex];
}

void SGE_ModelArray::draw()
{
	if (m_bmdFile == nullptr) return;

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

void SGE_ModelArray::onDraw(RZUF3_DrawEvent* event)
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

void SGE_ModelArray::onSetModelInput(SGE_SetModelInputEvent* event)
{
	uint16_t inputIndex = getInputIndex(event->getName());
	setAllItemsInput(inputIndex, event->getValue());
}

void SGE_ModelArray::onSetModelAngleInput(SGE_SetModelAngleInputEvent* event)
{
	uint16_t inputIndex = getInputIndex(event->getName());
	setAllItemsAngleInput(inputIndex, event->getAngle());
}

void SGE_ModelArray::onSetWorldPosition(SGE_SetWorldPositionEvent* event)
{
	setPosition(event->getPosition());
}

void SGE_ModelArray::removeModelDef()
{
	if (m_bmdFile == nullptr) return;

	for (int i = 0; i < m_items.size(); i++) {
		delete[] m_items[i].inputs;
	}

	m_items.clear();
	m_inputNames.clear();

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();
	assetsManager->removeAsset(m_options.bmdFilepath);
	m_bmdFile = nullptr;
}

void SGE_ModelArray::createModelDef()
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

	m_items.resize(m_options.items.size());
	for (int i = 0; i < m_items.size(); i++) {
		m_items[i].inputs = new uint16_t[m_bmdFile->inputs.inputsCount]{ 0 };
	}

	for (int i = 0; i < this->m_bmdFile->inputs.inputsCount; i++)
	{
		SGE_BMD_InputDef* input = &this->m_bmdFile->inputs.inputs[i];
		m_inputNames.insert(std::pair<std::string, uint16_t>(input->name, i));
	}
}

void SGE_ModelArray::removeSubmodelControllers()
{
	for (auto& submodelRenderer : m_submodelRenderers)
	{
		m_object->removeScript(submodelRenderer);
		delete submodelRenderer;
	}

	m_submodelRenderers.clear();
}

void SGE_ModelArray::createSubmodelControllers()
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

void SGE_ModelArray::removeCacheTexture()
{
	m_cacheTextureNeedsUpdate = true;
	m_cacheTextureBounds = { 0, 0, 0, 0 };

	if (!m_cacheTexture) return;
	SDL_DestroyTexture(m_cacheTexture);
	m_cacheTexture = nullptr;
}

void SGE_ModelArray::createCacheTexture()
{
	if (m_cacheTexture) return;

	m_cacheTextureNeedsUpdate = false;
	m_cacheTextureBounds = { 0, 0, 0, 0 };
	SDL_Rect& bounds = m_cacheTextureBounds;
	bounds = { SGE_INT_MAX, SGE_INT_MAX, SGE_INT_MIN, SGE_INT_MIN };

	for (int i = 0; i < m_items.size(); i++)
	{
		updateSubmodelsForItem(i, bounds);
	}

	// convert x1, y1, x2, y2 to x, y, w, h
	bounds.w -= bounds.x;
	bounds.h -= bounds.y;

	if (bounds.w == 0 || bounds.h == 0) return;

	g_renderer->createCacheTexture(m_cacheTexture, bounds.w, bounds.h, [this, bounds]() {
		for (int i = 0; i < m_items.size(); i++)
		{
			drawSubmodelsForItem(i);
		}
	});
}

void SGE_ModelArray::updateSubmodelsForItem(int itemIndex, SDL_Rect& bounds)
{
	SGE_BMD_ViewDef* view = getCurrentView(itemIndex);
	if (view == nullptr) return;

	SDL_Rect tempRect{};

	for (int i = 0; i < view->submodelsCount; i++)
	{
		uint16_t submodelIndex = view->submodels[i];
		updateSubmodel(itemIndex, submodelIndex, &tempRect);

		if (tempRect.w == 0 || tempRect.h == 0) continue;

		if (tempRect.x < bounds.x) bounds.x = tempRect.x;
		if (tempRect.y < bounds.y) bounds.y = tempRect.y;
		if (tempRect.x + tempRect.w > bounds.w) bounds.w = tempRect.x + tempRect.w;
		if (tempRect.y + tempRect.h > bounds.h) bounds.h = tempRect.y + tempRect.h;
	}
}

void SGE_ModelArray::drawSubmodelsForItem(int itemIndex)
{
	SGE_BMD_ViewDef* view = getCurrentView(itemIndex);
	if (view == nullptr) return;

	for (int i = 0; i < view->submodelsCount; i++)
	{
		uint16_t submodelIndex = view->submodels[i];

		updateSubmodel(itemIndex, submodelIndex, nullptr);
		drawSubmodel(submodelIndex);
	}
}

void SGE_ModelArray::updateSubmodel(int itemIndex, uint16_t submodelIndex, SDL_Rect* rect)
{
	SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[submodelIndex];
	SGE_TextureSetRenderer* submodelRenderer = m_submodelRenderers[submodelIndex];

	if (submodel->conditionInputIndex != 0) {
		uint16_t conditionInputValue = getInput(itemIndex, submodel->conditionInputIndex);

		if (submodel->conditionValue == 0xFFFF) {
			submodelRenderer->setOpacity(conditionInputValue);
		}
		else {
			bool conditionMet = submodel->conditionValue == conditionInputValue;
			submodelRenderer->setOpacity(conditionMet ? 0xFF : 0x00);

			if (!conditionMet) {
				if (rect != nullptr) *rect = { 0, 0, 0, 0 };
				return;
			}
		}
	}

	int index = getInput(itemIndex, submodel->indexByInputIndex);
	int rot = getInput(itemIndex, submodel->rotByInputIndex);

	index += rot;
	index /= submodel->indexDivisor;
	index += submodel->indexOffset;
	index %= submodel->indexRange;

	submodelRenderer->setIndex((uint16_t)index);

	double rotRad = ((double)rot / m_bmdFile->info.fullAngle) * 2.0 * M_PI;

	double submodelX = (double)submodel->x / SGE_BMD_FLOAT_SCALE;
	double submodelY = (double)submodel->y / SGE_BMD_FLOAT_SCALE;

	SGE_Point pos = m_options.items[itemIndex].worldRelPosition;
	SGE_PointUtils::worldToScreen(pos);

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

void SGE_ModelArray::drawSubmodel(uint16_t submodelIndex)
{
	SGE_BMD_SubmodelDef* submodel = &m_bmdFile->submodels.submodels[submodelIndex];
	SGE_TextureSetRenderer* submodelRenderer = m_submodelRenderers[submodelIndex];

	int x, y;
	submodelRenderer->getPosition(x, y);

	x -= m_cacheTextureBounds.x;
	y -= m_cacheTextureBounds.y;

	submodelRenderer->setDstPos(x, y);

	submodelRenderer->draw();
}

void SGE_ModelArray::setStartBaseRotations()
{
	if (m_bmdFile == nullptr) return;

	for (int i = 0; i < m_items.size(); i++) {
		setStartBaseRotation(i);
	}
}

void SGE_ModelArray::setStartBaseRotation(int itemIndex)
{
	SGE_ModelArrayItemOptions& itemOptions = m_options.items[itemIndex];

	uint16_t baseRotationIndex = getInputIndex(SGE_BASE_ROTATION_NAME);
	if (itemOptions.startBaseRotationRad != 0.0) {
		setAngleInput(itemIndex, baseRotationIndex, itemOptions.startBaseRotationRad);
	}
	else {
		setInput(itemIndex, baseRotationIndex, itemOptions.startBaseRotation);
	}
}

SGE_BMD_ViewDef* SGE_ModelArray::getCurrentView(int itemIndex)
{
	if (m_bmdFile == nullptr) return nullptr;

	uint16_t viewInputValue = getInput(itemIndex, m_bmdFile->views.indexByInputIndex);
	SGE_BMD_ViewDef* view = nullptr;
	uint8_t viewIndex = 0;
	for (viewIndex = 0; viewIndex < m_bmdFile->views.viewsCount; viewIndex++)
	{
		view = &m_bmdFile->views.views[viewIndex];
		if (view->endIndex >= viewInputValue) break;
	}

	return view;
}

bool SGE_ModelArray::checkInputIndex(uint16_t inputIndex)
{
	if (inputIndex >= m_bmdFile->inputs.inputsCount) {
		spdlog::error(
			"Input #{} does not exist in the model {}",
			inputIndex,
			m_options.bmdFilepath
		);

		return false;
	}

	return true;
}

bool SGE_ModelArray::checkItemIndex(int itemIndex)
{
	if (itemIndex < 0 || itemIndex >= m_items.size()) {
		spdlog::error(
			"Item #{} does not exist in the model {}",
			itemIndex,
			m_options.bmdFilepath
		);

		return false;
	}

	return true;
}
