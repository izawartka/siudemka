#pragma once
#include "../bmd/bmd_structs.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;

struct SGE_TextureSetRendererOptions {
	std::string filepath;
	SGE_BMD_TextureSetDef* setDef;
	int x = 0;
	int y = 0;
	uint8_t index = 0;
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
	void setIndex(uint8_t index, bool force = false);
	void setUseOnDraw(bool useOnDraw);
	void setOpacity(uint8_t opacity) { m_options.opacity = opacity; }
	void setAlign(RZUF3_Align alignment) { m_options.alignment = alignment; }

protected:
	void onDraw(RZUF3_DrawEvent* event);

	void removeTexture();
	void updateTexture();
	void cacheTextureDefs();

	SGE_TextureSetRendererOptions mp_options;

	SGE_TextureSetRendererOptions m_options;
	SDL_Texture* m_texture = nullptr;
	bool m_hasOnDrawListener = false;
	std::map<uint8_t, SGE_BMD_TextureDef*> m_textureDefs;
	SGE_BMD_TextureDef* m_textureDef = nullptr;

	_DECLARE_LISTENER(Draw);
};