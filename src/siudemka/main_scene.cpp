#include "main_scene.h"
#include "siudemka.h"

RZUF3_SceneDefinition* MainScene::m_sceneDef = nullptr;

RZUF3_SceneDefinition* MainScene::getSceneDef()
{
    if (m_sceneDef != nullptr) return m_sceneDef;

    m_sceneDef = new RZUF3_SceneDefinition();
    m_sceneDef->name = "siudemka";

    // text renderer options //
    RZUF3_TextRendererOptions textOptions;
    textOptions.style.fontFilepath = MAIN_FONT;

    // the model //
    SGE_ModelControllerOptions modelOptions;
    modelOptions.bmdFilepath = MODEL_PATH;
    modelOptions.centerAtOrigin = false;
    modelOptions.useSubpixelDrawing = false;

    RZUF3_ObjectDefinition objModel;
    objModel.name = MODEL_OBJ_NAME;
    objModel.scripts = {
        new SGE_ModelController(modelOptions),
        new RotateModel("rot_base", true),
        new RotateByDrag({0, 0, MODEL_WIDTH, MODEL_HEIGHT}),
        new RZUF3_WindowAnchor({0.5, 0.5, -MODEL_WIDTH/2, -ALL_HEIGHT/2})
    };
    m_sceneDef->objects.push_back(objModel);

    // UI root //
    RZUF3_ObjectDefinition objUIRoot;
    objUIRoot.name = "ui_root";
    objUIRoot.scripts = {
        new ModelOnlyOnKey(SDLK_LSHIFT, MODEL_OBJ_NAME, "ui_root"),
        new FullscreenOnKey(SDLK_F11),
        new RZUF3_WindowAnchor({0.5, 0.5, -MODEL_WIDTH/2, -UI_OFFCENTER/2})
    };
    m_sceneDef->objects.push_back(objUIRoot);

    // autorotate checkbox //
    RZUF3_CheckboxOptions autorotateOptions;
    autorotateOptions.style.textStyle.fontFilepath = MAIN_FONT;
    autorotateOptions.style.defaultColors();
    autorotateOptions.labelText = "autorotate_label";
    autorotateOptions.checked = true;

    RZUF3_ObjectDefinition objAutorotate;
    objAutorotate.name = "autorotate_check";
    objAutorotate.parentName = "ui_root";
    objAutorotate.pos = RZUF3_Pos(16, 16);
    objAutorotate.scripts = {
        new RZUF3_Checkbox(autorotateOptions),
        new AutorotateByCheckbox(MODEL_OBJ_NAME)
    };
    m_sceneDef->objects.push_back(objAutorotate);

    // lights //
    std::string lightSideNames[] = { "f", "r" };
    std::string lightNames[] = { "l", "t", "r" };
    std::string lightSideDisplayNames[] = { "light_front", "light_rear" };
    std::string lightDisplayNames[] = { "light_l", "light_t", "light_r" };
    int lightInputs[] = { 10, 2, 0, 3, 9, 1, 8, 5, 0, 6, 7, 4};

    for (int j = 0; j < 2; j++)
    {
        int sliderY = 56 * j + 72;

        textOptions.text = lightSideDisplayNames[j];
        textOptions.dstRect = { 0, 0 };

        RZUF3_ObjectDefinition objLightSideLabel;
        objLightSideLabel.name = "light_" + lightSideNames[j] + "_label";
        objLightSideLabel.parentName = "ui_root";
        objLightSideLabel.pos = RZUF3_Pos(16, sliderY - 24);
        objLightSideLabel.scripts = {
			new RZUF3_TextRenderer(textOptions)
		};
        m_sceneDef->objects.push_back(objLightSideLabel);

        for (int i = 0; i < 3; i++)
        {
            int sliderX = 16 + 126 * i + 47;
            std::string lightName = lightSideNames[j] + "_" + lightNames[i];
            int sliderMin = i == 1 ? 0 : -1;
            int inputIndex = j * 6 + i * 2;

            textOptions.text = lightDisplayNames[i];
            textOptions.dstRect = { -47, -3 };

            RZUF3_ObjectDefinition objLightSlider;
            objLightSlider.name = "light_" + lightName + "_slider";
            objLightSlider.parentName = "ui_root";
            objLightSlider.pos = RZUF3_Pos(sliderX, sliderY);
            objLightSlider.scripts = {
                new RZUF3_Slider(sliderMin, 1, 0),
                new InputIfUISoft(MODEL_OBJ_NAME, "signal_" + lightName, -1, 255, 0.75),
                new InputIfUISoft(MODEL_OBJ_NAME, "light_" + lightName, 1, 255, 0.75),
                new RZUF3_TextRenderer(textOptions)
            };

            m_sceneDef->objects.push_back(objLightSlider);
        }
    }

    // pantograph A //
    textOptions.text = "pant_a_label";
    textOptions.dstRect = { -112, -3 };

    RZUF3_ObjectDefinition objPantographASlider;
    objPantographASlider.name = "pantograph_a_slider";
    objPantographASlider.parentName = "ui_root";
    objPantographASlider.pos = RZUF3_Pos(16 + 112, 168);
    objPantographASlider.scripts = {
		new RZUF3_Slider(0, 1, 0),
		new InputIfUISoft(MODEL_OBJ_NAME, "pant_f", 1, 7, 0.001),
        new RZUF3_TextRenderer(textOptions)
	};
    m_sceneDef->objects.push_back(objPantographASlider);

    // pantograph B //
    textOptions.text = "pant_b_label";
    textOptions.dstRect = { -112, -3 };

    RZUF3_ObjectDefinition objPantographBSlider;
	objPantographBSlider.name = "pantograph_b_slider";
    objPantographBSlider.parentName = "ui_root";
    objPantographBSlider.pos = RZUF3_Pos(16 + 112 + 187, 168);
    objPantographBSlider.scripts = {
        new RZUF3_Slider(0, 1, 0),
        new InputIfUISoft(MODEL_OBJ_NAME, "pant_r", 1, 7, 0.001),
        new RZUF3_TextRenderer(textOptions)
    };
    m_sceneDef->objects.push_back(objPantographBSlider);

    RZUF3_SliderStyle bogieSliderStyle;
    bogieSliderStyle.width = 235;

    // door A //
    textOptions.text = "door_a_label";
    textOptions.dstRect = { -112, -3 };

    RZUF3_ObjectDefinition objDoorASlider;
    objDoorASlider.name = "door_a_slider";
    objDoorASlider.parentName = "ui_root";
    objDoorASlider.pos = RZUF3_Pos(16 + 112, 200);
    objDoorASlider.scripts = {
        new RZUF3_Slider(0, 1, 0),
        new InputIfUISoft(MODEL_OBJ_NAME, "door_f", 1, 6, 0.01),
        new RZUF3_TextRenderer(textOptions)
    };
    m_sceneDef->objects.push_back(objDoorASlider);

    // door B //
    textOptions.text = "door_b_label";
    textOptions.dstRect = { -112, -3 };

    RZUF3_ObjectDefinition objDoorBSlider;
    objDoorBSlider.name = "door_b_slider";
    objDoorBSlider.parentName = "ui_root";
    objDoorBSlider.pos = RZUF3_Pos(16 + 112 + 187, 200);
    objDoorBSlider.scripts = {
        new RZUF3_Slider(0, 1, 0),
        new InputIfUISoft(MODEL_OBJ_NAME, "door_r", 1, 6, 0.01),
        new RZUF3_TextRenderer(textOptions)
    };
    m_sceneDef->objects.push_back(objDoorBSlider);

    // front bogie //
    textOptions.text = "bogie_f_label";
    textOptions.dstRect = { -112, -3 };

    RZUF3_ObjectDefinition objFrontBogieSlider;
    objFrontBogieSlider.name = "front_bogie_slider";
    objFrontBogieSlider.parentName = "ui_root";
    objFrontBogieSlider.pos = RZUF3_Pos(16 + 112, 232);
    objFrontBogieSlider.scripts = {
        new RZUF3_Slider({-30, 30, 0, bogieSliderStyle}),
        new InputByUI(MODEL_OBJ_NAME, "rot_bogie_f", 360),
		new RZUF3_TextRenderer(textOptions)
	};
	m_sceneDef->objects.push_back(objFrontBogieSlider);

    // rear bogie //
    textOptions.text = "bogie_r_label";
    textOptions.dstRect = { -112, -3 };

    RZUF3_ObjectDefinition objRearBogieSlider;
	objRearBogieSlider.name = "rear_bogie_slider";
    objRearBogieSlider.parentName = "ui_root";
    objRearBogieSlider.pos = RZUF3_Pos(16 + 112, 264);
    objRearBogieSlider.scripts = {
        new RZUF3_Slider({-30, 30, 0, bogieSliderStyle}),
        new InputByUI(MODEL_OBJ_NAME, "rot_bogie_r", 360),
        new RZUF3_TextRenderer(textOptions)
    };
    m_sceneDef->objects.push_back(objRearBogieSlider);

    // author //
    RZUF3_ObjectDefinition objAuthor;
	objAuthor.name = "author";

    RZUF3_TextRendererOptions authorOptions;
    authorOptions.style.fontFilepath = MAIN_FONT;
    authorOptions.text = "masuo 2024.06";
    authorOptions.useLangFile = false;
    authorOptions.alignment = RZUF3_Align_BottomRight;

    objAuthor.scripts = {
        new RZUF3_TextRenderer(authorOptions),
        new RZUF3_WindowAnchor({1, 1, -16, -16})
    };
    m_sceneDef->objects.push_back(objAuthor);

    return m_sceneDef;
}

void MainScene::destroy()
{
    if (m_sceneDef == nullptr) return;

    for (auto& obj : m_sceneDef->objects)
    {
        for (auto& script : obj.scripts)
        {
            delete script;
        }
    }

    delete m_sceneDef;
    m_sceneDef = nullptr;
}