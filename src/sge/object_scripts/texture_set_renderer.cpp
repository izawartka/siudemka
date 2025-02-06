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
	m_options = mp_options;

	updateTexture();
	cacheTextureDefs();
	setIndex(m_options.index, true);

	setUseOnDraw(m_options.useOnDraw);
}

void SGE_TextureSetRenderer::deinit()
{
	setUseOnDraw(false);

	removeTexture();

	m_textureDefs.clear();
	m_textureDef = nullptr;
}

void SGE_TextureSetRenderer::draw()
{
	if (m_object == nullptr) return;
	if (m_texture == nullptr) return;
	if (m_textureDef == nullptr) return;
	if (m_options.opacity == 0) return;

	SDL_Rect srcRect = {
		m_textureDef->srcX,
		m_textureDef->srcY,
		m_textureDef->width,
		m_textureDef->height
	};

	SDL_Rect dstRect = {
		m_options.x + m_textureDef->dstX,
		m_options.y + m_textureDef->dstY,
		m_textureDef->width,
		m_textureDef->height
	};

	g_renderer->setAlign(m_options.alignment);
	if (m_options.opacity != 255) {
		g_renderer->drawTextureOpaque(m_object, m_texture, &srcRect, dstRect, m_options.opacity);
	}
	else {
		g_renderer->drawTexture(m_object, m_texture, &srcRect, dstRect);
	}
}

void SGE_TextureSetRenderer::setFilepath(std::string filepath)
{
	removeTexture();
	m_options.filepath = filepath;
	updateTexture();
}

void SGE_TextureSetRenderer::setTextureSet(SGE_BMD_TextureSetDef* setDef)
{
	m_options.setDef = setDef;
	cacheTextureDefs();
	setIndex(0, true);
}

void SGE_TextureSetRenderer::setDstPos(int x, int y)
{
	m_options.x = x;
	m_options.y = y;
}

void SGE_TextureSetRenderer::setIndex(uint16_t index, bool force)
{
	if(m_options.index == index && !force) return;

	m_options.index = index;
	m_textureDef = nullptr;
	if (m_options.setDef == nullptr) return;

	auto it = m_textureDefs.find(m_options.index);
	if (it == m_textureDefs.end()) return;

	m_textureDef = it->second;
}

void SGE_TextureSetRenderer::setUseOnDraw(bool useOnDraw)
{
	m_options.useOnDraw = useOnDraw;
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();

	if (useOnDraw && !m_hasOnDrawListener)
	{
		_ADD_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = true;
	}
	else if (!useOnDraw && m_hasOnDrawListener)
	{
		_REMOVE_LISTENER(eventsManager, Draw);
		m_hasOnDrawListener = false;
	}
}

void SGE_TextureSetRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_options.useOnDraw) draw();
}

void SGE_TextureSetRenderer::removeTexture()
{
	if (m_texture == nullptr) return;

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();

	assetsManager->removeAsset(m_options.filepath);
	m_texture = nullptr;
}

void SGE_TextureSetRenderer::updateTexture()
{
	if (this->m_object == nullptr) return;
	if (this->m_options.filepath == "") return;

	RZUF3_AssetsManager* assetsManager = g_scene->getAssetsManager();

	RZUF3_AssetDefinition assetDefinition;
	assetDefinition.filepath = m_options.filepath;
	assetDefinition.factory = RZUF3_Texture::getInstance;

	if (!assetsManager->addAsset(assetDefinition)) return;

	this->m_texture = (SDL_Texture*)assetsManager->getAssetContent(m_options.filepath);

	if (this->m_texture == nullptr)
	{
		spdlog::error("Texture {} not found", this->m_options.filepath);
		return;
	}
}

void SGE_TextureSetRenderer::cacheTextureDefs()
{
	m_textureDefs.clear();

	if (m_options.setDef == nullptr) return;

	for(int i = 0; i < m_options.setDef->texturesCount; i++)
	{
		SGE_BMD_TextureDef* textureDef = &m_options.setDef->textures[i];
		m_textureDefs.insert(std::pair<uint16_t, SGE_BMD_TextureDef*>(textureDef->index, textureDef));
	}
}
