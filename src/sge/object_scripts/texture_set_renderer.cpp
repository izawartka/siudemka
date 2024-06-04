#include "texture_set_renderer.h"

SGE_TextureSetRenderer::SGE_TextureSetRenderer(std::string filepath, SGE_BMD_TextureSetDef* setDef)
{
	mp_options.filepath = filepath;
	mp_options.setDef = setDef;
}

SGE_TextureSetRenderer::SGE_TextureSetRenderer(SGE_TextureSetRendererOptions options)
{
	mp_options = options;
}

SGE_TextureSetRenderer::~SGE_TextureSetRenderer()
{

}

void SGE_TextureSetRenderer::init()
{
	m_textureFilepath = mp_options.filepath;
	m_texture = nullptr;
	m_setDef = mp_options.setDef;
	m_x = mp_options.x;
	m_y = mp_options.y;
	m_index = mp_options.index;
	m_userDrawOnly = mp_options.userDrawOnly;
	m_renderer = getObject()->getScene()->getRenderer();

	updateTexture();
	cacheTextureDefs();
	setIndex(m_index, true);

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);
}

void SGE_TextureSetRenderer::deinit()
{
	removeTexture();

	RZUF3_EventsManager* eventsManager = getObject()->getScene()->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);
}

void SGE_TextureSetRenderer::userDraw()
{
	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;
	if (m_textureDef == nullptr) return;

	SDL_Rect srcRect = {
		m_textureDef->srcX,
		m_textureDef->srcY,
		m_textureDef->width,
		m_textureDef->height
	};

	SDL_Rect dstRect = {
		m_x + m_textureDef->dstX,
		m_y + m_textureDef->dstY,
		m_textureDef->width,
		m_textureDef->height
	};

	m_renderer->drawTexture(m_object, m_texture, &srcRect, dstRect);
}

void SGE_TextureSetRenderer::setFilepath(std::string filepath)
{
	removeTexture();
	m_textureFilepath = filepath;
	updateTexture();
}

void SGE_TextureSetRenderer::setTextureSet(SGE_BMD_TextureSetDef* setDef)
{
	m_setDef = setDef;
	cacheTextureDefs();
	setIndex(0, true);
}

void SGE_TextureSetRenderer::setDstPos(int x, int y)
{
	m_x = x;
	m_y = y;
}

void SGE_TextureSetRenderer::setIndex(uint8_t index, bool force)
{
	if(m_index == index && !force) return;

	m_index = index;
	m_textureDef = nullptr;
	if (m_setDef == nullptr) return;

	auto it = m_textureDefs.find(m_index);
	if (it == m_textureDefs.end()) return;

	m_textureDef = it->second;
}

void SGE_TextureSetRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_userDrawOnly) return;
	userDraw();
}

void SGE_TextureSetRenderer::removeTexture()
{
	if (m_texture == nullptr) return;

	RZUF3_AssetsManager* assetsManager = this->m_object->getScene()->getAssetsManager();

	assetsManager->removeAsset(m_textureFilepath);
	m_texture = nullptr;
}

void SGE_TextureSetRenderer::updateTexture()
{
	if (this->m_object == nullptr) return;
	if (this->m_textureFilepath == "") return;

	RZUF3_AssetsManager* assetsManager = this->m_object->getScene()->getAssetsManager();

	RZUF3_AssetDefinition assetDefinition;
	assetDefinition.filepath = m_textureFilepath;
	assetDefinition.factory = RZUF3_Texture::getInstance;

	if (!assetsManager->addAsset(assetDefinition)) return;

	this->m_texture = (SDL_Texture*)assetsManager->getAssetContent(m_textureFilepath);

	if (this->m_texture == nullptr)
	{
		spdlog::error("Texture {} not found", this->m_textureFilepath);
		return;
	}
}

void SGE_TextureSetRenderer::cacheTextureDefs()
{
	m_textureDefs.clear();

	if (m_setDef == nullptr) return;

	for(int i = 0; i < m_setDef->texturesCount; i++)
	{
		SGE_BMD_TextureDef* textureDef = &m_setDef->textures[i];
		m_textureDefs.insert(std::pair<uint8_t, SGE_BMD_TextureDef*>(textureDef->index, textureDef));
	}
}
