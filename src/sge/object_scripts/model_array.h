#pragma once
#include "../assets/model_def.h"
#include "../events/events.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class SGE_TextureSetRenderer;

struct SGE_ModelArrayItemOptions {
	uint16_t startBaseRotation = 0;
	double startBaseRotationRad = 0.0;
	SGE_Point worldRelPosition = { 0, 0, 0 };
};

struct SGE_ModelArrayItem {
	uint16_t* inputs = nullptr;
};

struct SGE_ModelArrayOptions {
	std::string bmdFilepath;
	bool centerAtOrigin = true;
	int layer = 0;
	bool useDrawQueue = true;
	bool useSubpixelDrawing = true;
	SGE_Point worldPosition = { 0, 0, 0 };
	std::vector<SGE_ModelArrayItemOptions> items;
};

class SGE_ModelArray : public SGE_DrawQueueScript {
public:
	SGE_ModelArray(SGE_ModelArrayOptions options);
	~SGE_ModelArray();

	void init();
	void deinit();

	void setFilepath(std::string filepath);
	bool setInput(int itemIndex, uint16_t inputIndex, uint16_t value);
	bool setAllItemsInput(uint16_t inputIndex, uint16_t value);
	bool setAngleInput(int itemIndex, uint16_t inputIndex, double angle);
	bool setAllItemsAngleInput(uint16_t inputIndex, double angle);
	bool setItemPosition(int itemIndex, SGE_Point relPosition);
	void setPosition(SGE_Point position);
	void setLayer(int layer);

	uint16_t getInputIndex(std::string name);
	uint16_t getInput(int itemIndex, uint16_t inputIndex);
	SGE_Point getPosition() const { return m_options.worldPosition; }
	uint16_t getLayer() const { return m_options.layer; }

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
	void updateSubmodelsForItem(int itemIndex, SDL_FRect& bounds);
	void drawSubmodelsForItem(int itemIndex);
	void updateSubmodel(int itemIndex, uint16_t submodelIndex, SDL_FRect* rect);
	void drawSubmodel(uint16_t submodelIndex);

	void setStartBaseRotations();
	void setStartBaseRotation(int itemIndex);
	SGE_BMD_ViewDef* getCurrentView(int itemIndex);
	bool checkInputIndex(uint16_t inputIndex);
	bool checkItemIndex(int itemIndex);

	SGE_ModelArrayOptions mp_options;

	SGE_ModelArrayOptions m_options;

	SGE_BMD_File* m_bmdFile = nullptr;
	std::vector<SGE_ModelArrayItem> m_items;
	SDL_Texture* m_cacheTexture = nullptr;
	bool m_cacheTextureNeedsUpdate = true;
	SDL_FRect m_cacheTextureBounds = { 0, 0, 0, 0 };
	std::map<std::string, uint16_t> m_inputNames;
	std::vector<SGE_TextureSetRenderer*> m_submodelRenderers;

	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(SetModelInput)
	_DECLARE_LISTENER(SetModelAngleInput)
	_DECLARE_LISTENER(SetWorldPosition)
};