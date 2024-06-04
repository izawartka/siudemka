#pragma once
#include "../common.h"
#include "../bmd_structs.h"

class RZUF3_ObjectScript;

struct SGE_TextureSetRendererOptions {
	std::string filepath;
	SGE_BMD_TextureSetDef* setDef;
	int x = 0;
	int y = 0;
	uint8_t index = 0;
	bool userDrawOnly = false;
	uint8_t opacity = 255;

};

class SGE_TextureSetRenderer : public RZUF3_ObjectScript {
public:
	SGE_TextureSetRenderer(std::string filepath, SGE_BMD_TextureSetDef* setDef);
	SGE_TextureSetRenderer(SGE_TextureSetRendererOptions options);
	~SGE_TextureSetRenderer();

	void init();
	void deinit();

	void userDraw();

	void setFilepath(std::string filepath);
	void setTextureSet(SGE_BMD_TextureSetDef* setDef);
	void setDstPos(int x, int y);
	void setIndex(uint8_t index, bool force = false);
	void setUserDrawOnly(bool userDrawOnly) { m_userDrawOnly = userDrawOnly; }
	void setOpacity(uint8_t opacity) { m_opacity = opacity; }

protected:
	void onDraw(RZUF3_DrawEvent* event);

	void removeTexture();
	void updateTexture();
	void cacheTextureDefs();

	SGE_TextureSetRendererOptions mp_options;

	std::string m_textureFilepath;
	SDL_Texture* m_texture = nullptr;
	SGE_BMD_TextureSetDef* m_setDef = nullptr;
	int m_x = 0;
	int m_y = 0;
	uint8_t m_index = 0;
	bool m_userDrawOnly = false;
	uint8_t m_opacity = 255;

	RZUF3_Renderer* m_renderer = nullptr;
	std::map<uint8_t, SGE_BMD_TextureDef*> m_textureDefs;
	SGE_BMD_TextureDef* m_textureDef = nullptr;

	_DECLARE_LISTENER(Draw);
};