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

	if (m_options.useSubpixel) {
		drawSubpixel();
	} else {
		drawNonSubpixel();
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

void SGE_TextureSetRenderer::setDstPos(float x, float y)
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

void SGE_TextureSetRenderer::getRect(SDL_Rect& rect)
{
	if (m_textureDef == nullptr) return;

	rect.x = (int)(m_options.x + 0.5f) + m_textureDef->dstX;
	rect.y = (int)(m_options.y + 0.5f) + m_textureDef->dstY;
	rect.w = m_textureDef->width;
	rect.h = m_textureDef->height;
}

void SGE_TextureSetRenderer::getRect(SDL_FRect& rect)
{
	if (m_textureDef == nullptr) return;

	rect.x = m_options.x + (float)m_textureDef->dstX;
	rect.y = m_options.y + (float)m_textureDef->dstY;
	rect.w = m_textureDef->width;
	rect.h = m_textureDef->height;
}

void SGE_TextureSetRenderer::getPosition(int& x, int& y) const
{
	x = (int)(m_options.x + 0.5f);
	y = (int)(m_options.y + 0.5f);
}

void SGE_TextureSetRenderer::getPosition(float& x, float& y) const
{
	x = m_options.x;
	y = m_options.y;
}

SDL_Rect SGE_TextureSetRenderer::getSrcRect() const
{
	SDL_Rect rect = { 0, 0, 0, 0 };
	if (m_textureDef == nullptr) return rect;

	rect.x = m_textureDef->srcX;
	rect.y = m_textureDef->srcY;
	rect.w = m_textureDef->width;
	rect.h = m_textureDef->height;

	return rect;
}

void SGE_TextureSetRenderer::onDraw(RZUF3_DrawEvent* event)
{
	if (m_options.useOnDraw) draw();
}

void SGE_TextureSetRenderer::drawNonSubpixel()
{
	SDL_Rect srcRect = getSrcRect();
	SDL_Rect dstRect;
	getRect(dstRect);

	g_renderer->setAlign(m_options.alignment);
	g_renderer->setOpacity(m_options.opacity);
	g_renderer->drawTexture(m_object, m_texture, &srcRect, dstRect);
	g_renderer->resetOpacity();
}

void SGE_TextureSetRenderer::drawSubpixel()
{
	SDL_Rect srcRect = getSrcRect();
	SDL_FRect dstRect{};
	getRect(dstRect);

	g_renderer->setAlign(m_options.alignment);
	g_renderer->setOpacity(m_options.opacity);
	g_renderer->drawTexture(m_object, m_texture, &srcRect, dstRect);
	g_renderer->resetOpacity();
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
