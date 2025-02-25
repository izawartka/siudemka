#pragma once
#include "../assets/model_def.h"
#include "../events/events.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class SGE_TextureSetRenderer;

struct SGE_ModelControllerOptions {
	std::string bmdFilepath;
	bool centerAtOrigin = true;
	uint16_t startBaseRotation = 0;
	int layer = 0;
	SGE_Point worldPosition = { 0, 0, 0 };
	bool useDrawQueue = true;
	bool cacheTexture = true;
};

class SGE_ModelController : public SGE_DrawQueueScript {
public:
	SGE_ModelController(std::string filepath);
	SGE_ModelController(SGE_ModelControllerOptions options);
	~SGE_ModelController();

	void init();
	void deinit();

	void setFilepath(std::string filepath);
	bool setInput(uint16_t index, uint16_t value);
	bool setAngleInput(uint16_t index, double angle);
	void setLayer(int layer);
	void setPosition(SGE_Point position);
	void setUseCacheTexture(bool useCache);

	uint16_t getInputIndex(std::string name);
	uint16_t getInput(uint16_t index);
	uint16_t getLayer() const { return m_options.layer; }
	SGE_Point getPosition() const { return m_options.worldPosition; }

	void draw();

protected:
	void onDraw(RZUF3_DrawEvent* event);
	void onSetModelInput(SGE_SetModelInputEvent* event);
	void onSetModelAngleInput(SGE_SetModelAngleInputEvent* event);
	void onSetWorldPosition(SGE_SetWorldPositionEvent* event);

	void removeModelDef();
	void createModelDef();
	void removeSubmodelControllers();
	void createSubmodelControllers();
	void removeCacheTexture();
	void createCacheTexture();
	void drawNoCache();
	void drawFromCache();
	void updateSubmodel(uint16_t submodelIndex, SDL_Rect* rect);
	void drawSubmodel(uint16_t submodelIndex, bool forCache);
	SGE_BMD_ViewDef* getCurrentView();

	SGE_ModelControllerOptions mp_options;

	SGE_ModelControllerOptions m_options;

	SGE_BMD_File* m_bmdFile = nullptr;
	uint16_t* m_inputs = nullptr;
	SDL_Texture* m_cacheTexture = nullptr;
	bool m_cacheTextureNeedsUpdate = true;
	SDL_Rect m_cacheTextureBounds = { 0, 0, 0, 0 };
	std::map<std::string, uint16_t> m_inputNames;
	std::vector<SGE_TextureSetRenderer*> m_submodelRenderers;

	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(SetModelInput)
	_DECLARE_LISTENER(SetModelAngleInput)
	_DECLARE_LISTENER(SetWorldPosition)
};