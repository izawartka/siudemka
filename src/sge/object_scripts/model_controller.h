#pragma once
#include "../assets/model_def.h"
#include "../events/set_model_input.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class SGE_TextureSetRenderer;
class SGE_WorldTransform;

struct SGE_ModelControllerOptions {
	std::string bmdFilepath;
	bool centerAtOrigin = true;
	uint8_t startRotBase = 0;
	int layer = 0;
	SGE_Point worldPosition = { 0, 0, 0 };
	bool useDrawQueue = true;
};

class SGE_ModelController : public RZUF3_ObjectScript {
public:
	SGE_ModelController(std::string filepath);
	SGE_ModelController(SGE_ModelControllerOptions options);
	~SGE_ModelController();

	void init();
	void deinit();

	void setFilepath(std::string filepath);
	uint16_t getInputIndex(std::string name);
	bool setInput(uint16_t index, uint8_t value);
	bool setAngleInput(uint16_t index, double angle);
	uint8_t getInput(uint16_t index);
	void setLayer(int layer);
	uint16_t getLayer() const { return mp_options.layer; }
	SGE_Point getPosition() const { return m_worldPosition; }
	void setPosition(SGE_Point position);

	void drawSubmodels();

protected:
	void onDraw(RZUF3_DrawEvent* event);
	void onSetModelInput(SGE_SetModelInputEvent* event);
	void onSetWorldPosition(SGE_SetWorldPositionEvent* event);

	void removeModelDef();
	void createModelDef();
	void removeSubmodelControllers();
	void createSubmodelControllers();
	void drawSubmodel(uint16_t index);

	SGE_ModelControllerOptions mp_options;

	bool m_centerAtOrigin = true;
	std::string m_bmdFilepath;
	SGE_BMD_File* m_bmdFile = nullptr;
	uint8_t* m_inputs = nullptr;
	int m_layer = 0;
	SGE_Point m_worldPosition = { 0, 0, 0 };
	std::map<std::string, uint16_t> m_inputNames;
	std::vector<SGE_TextureSetRenderer*> m_submodelRenderers;

	double m_angle = 0.0;
	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(SetModelInput)
	_DECLARE_LISTENER(SetWorldPosition)
};