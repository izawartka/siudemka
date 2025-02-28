#pragma once
#include "../bmd/bmd_structs.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

struct SGE_TextureSetRendererOptions {
	std::string filepath;
	SGE_BMD_TextureSetDef* setDef;
	bool useSubpixel = false;
	float x = 0;
	float y = 0;
	uint16_t index = 0;
	bool useOnDraw = true;
	uint8_t opacity = 255;
	RZUF3_Align alignment = RZUF3_Align_TopLeft;
};

class SGE_TextureSetRenderer : public RZUF3_ObjectScript {
public:
	SGE_TextureSetRenderer(std::string filepath, SGE_BMD_TextureSetDef* setDef);
	SGE_TextureSetRenderer(SGE_TextureSetRendererOptions options);
	~SGE_TextureSetRenderer();

	void init();
	void deinit();

	void draw();

	void setFilepath(std::string filepath);
	void setTextureSet(SGE_BMD_TextureSetDef* setDef);
	void setDstPos(int x, int y);
	void setDstPos(float x, float y);
	void setIndex(uint16_t index, bool force = false);
	void setUseOnDraw(bool useOnDraw);
	void setUseSubpixel(bool useSubpixel) { m_options.useSubpixel = useSubpixel; }
	void setOpacity(uint8_t opacity) { m_options.opacity = opacity; }
	void setAlign(RZUF3_Align alignment) { m_options.alignment = alignment; }

	void getRect(SDL_Rect& rect);
	void getRect(SDL_FRect& rect);
	void getPosition(int& x, int& y) const;
	void getPosition(float& x, float& y) const;
	SDL_Texture* getTexture() const { return m_texture; }
	SDL_Rect getSrcRect() const;

protected:
	void onDraw(RZUF3_DrawEvent* event);

	void drawNonSubpixel();
	void drawSubpixel();
	void removeTexture();
	void updateTexture();
	void cacheTextureDefs();

	SGE_TextureSetRendererOptions mp_options;

	SGE_TextureSetRendererOptions m_options;
	SDL_Texture* m_texture = nullptr;
	bool m_hasOnDrawListener = false;
	std::map<uint16_t, SGE_BMD_TextureDef*> m_textureDefs;
	SGE_BMD_TextureDef* m_textureDef = nullptr;

	_DECLARE_LISTENER(Draw);
};