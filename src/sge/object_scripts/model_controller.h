#pragma once
#include "../assets/model_def.h"
#include "../events/set_model_input.h"
#include "object_scripts.h"

class RZUF3_ObjectScript;
class SGE_TextureSetRenderer;

struct SGE_ModelControllerOptions {
	std::string bmdFilepath;
	std::string assetsDirpath = "";
	bool centerAtOrigin = true;
};

class SGE_ModelController : public RZUF3_ObjectScript {
public:
	SGE_ModelController(std::string filepath);
	SGE_ModelController(SGE_ModelControllerOptions options);
	~SGE_ModelController();

	void init();
	void deinit();

	void setFilepath(std::string filepath, std::string assetsDirpath = "");
	bool setInput(uint16_t index, uint8_t value);
	bool setInput(std::string name, uint8_t value);
	bool setAngleInput(uint16_t index, double angle);
	uint8_t getInput(uint16_t index);

protected:
	void onDraw(RZUF3_DrawEvent* event);
	void onSetModelInput(SGE_SetModelInputEvent* event);

	void removeModelDef();
	void createModelDef();
	void removeSubmodelControllers();
	void createSubmodelControllers();
	void drawSubmodels();
	void drawSubmodel(uint16_t index);

	SGE_ModelControllerOptions mp_options;

	bool m_centerAtOrigin = true;
	std::string m_bmdFilepath;
	std::string m_assetsDirpath;
	SGE_BMD_File* m_bmdFile = nullptr;
	uint8_t* m_inputs = nullptr;
	std::map<std::string, uint16_t> m_inputNames;
	std::vector<SGE_TextureSetRenderer*> m_submodelRenderers;

	double m_angle = 0.0;
	_DECLARE_LISTENER(Draw)
	_DECLARE_LISTENER(SetModelInput)
};