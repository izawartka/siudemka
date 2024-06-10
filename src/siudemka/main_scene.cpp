#include "main_scene.h"
#include "siudemka.h"

RZUF3_SceneDefinition* MainScene::m_sceneDef = nullptr;

RZUF3_SceneDefinition* MainScene::getSceneDef()
{
    if (m_sceneDef != nullptr) return m_sceneDef;

    m_sceneDef = new RZUF3_SceneDefinition();
    m_sceneDef->name = "siudemka";

    // the model //
    RZUF3_ObjectDefinition objSiudemka;
    objSiudemka.name = "siudemka";
    objSiudemka.scripts = {
        new SGE_ModelController("assets/siudemka/model.bmd", "assets/siudemka"),
        new RotateModel("rot_base", true),
        new RotateByDrag({0, 0, 384, 208})
    };
    m_sceneDef->objects.push_back(objSiudemka);

    // UI root //
    RZUF3_ObjectDefinition objUIRoot;
    objUIRoot.name = "UIRoot";
    objUIRoot.pos = RZUF3_Pos(0, 208);
    objUIRoot.scripts = {
        new WindowHeightOnKey(SDLK_LSHIFT, 208, 544)
    };
    m_sceneDef->objects.push_back(objUIRoot);

    // autorotate checkbox //
    RZUF3_ObjectDefinition objAutorotate;
    objAutorotate.name = "autorotateCheck";
    objAutorotate.parentName = "UIRoot";
    objAutorotate.pos = RZUF3_Pos(16, 16);
    objAutorotate.scripts = {
        new RZUF3_Checkbox({"assets/fonts/roboto-regular.ttf", "obrot", true}),
        new AutorotateByCheckbox("siudemka")
    };
    m_sceneDef->objects.push_back(objAutorotate);

    // lights //
    std::string lightSideNames[] = { "f", "r" };
    std::string lightNames[] = { "l", "t", "r" };
    std::string lightSideDisplayNames[] = { "oswietlenie przednie:", "oswietlenie tylne:" };
    std::string lightDisplayNames[] = { "  lewe", "gorne", "prawe" };
    int lightInputs[] = { 10, 2, 0, 3, 9, 1, 8, 5, 0, 6, 7, 4};

    for (int j = 0; j < 2; j++)
    {
        int sliderY = 56 * j + 72;

        RZUF3_ObjectDefinition objLightSideLabel;
        objLightSideLabel.name = "light" + lightSideNames[j] + "Label";
        objLightSideLabel.parentName = "UIRoot";
        objLightSideLabel.pos = RZUF3_Pos(16, sliderY - 24);
        objLightSideLabel.scripts = {
			new RZUF3_TextRenderer("assets/fonts/roboto-regular.ttf", lightSideDisplayNames[j])
		};
        m_sceneDef->objects.push_back(objLightSideLabel);

        for (int i = 0; i < 3; i++)
        {
            int sliderX = 16 + 126 * i + 47;
            std::string lightName = lightSideNames[j] + "_" + lightNames[i];
            int sliderMin = i == 1 ? 0 : -1;
            int inputIndex = j * 6 + i * 2;

            RZUF3_ObjectDefinition objLightSlider;
            objLightSlider.name = "light_" + lightName + "Slider";
            objLightSlider.parentName = "UIRoot";
            objLightSlider.pos = RZUF3_Pos(sliderX, sliderY);
            objLightSlider.scripts = {
                new RZUF3_Slider(sliderMin, 1, 0),
                new InputIfUISoft("siudemka", "signal_" + lightName, -1, 255, 0.75),
                new InputIfUISoft("siudemka", "light_" + lightName, 1, 255, 0.75),
                new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", lightDisplayNames[i], -47, -3})
            };

            m_sceneDef->objects.push_back(objLightSlider);
        }
    }

    // pantograph A //
    RZUF3_ObjectDefinition objPantographASlider;
    objPantographASlider.name = "pantographASlider";
    objPantographASlider.parentName = "UIRoot";
    objPantographASlider.pos = RZUF3_Pos(16 + 112, 168);
    objPantographASlider.scripts = {
		new RZUF3_Slider(0, 1, 0),
		new InputIfUISoft("siudemka", "pant_f", 1, 6, 0.001),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "pantograf A", -112, -3})
	};
    m_sceneDef->objects.push_back(objPantographASlider);

    // pantograph B //
    RZUF3_ObjectDefinition objPantographBSlider;
	objPantographBSlider.name = "pantographBSlider";
    objPantographBSlider.parentName = "UIRoot";
    objPantographBSlider.pos = RZUF3_Pos(16 + 112 + 187, 168);
    objPantographBSlider.scripts = {
        new RZUF3_Slider(0, 1, 0),
        new InputIfUISoft("siudemka", "pant_r", 1, 6, 0.001),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "pantograf B", -112, -3})
    };
    m_sceneDef->objects.push_back(objPantographBSlider);

    RZUF3_SliderStyle bogieSliderStyle;
    bogieSliderStyle.width = 235;

    // door A //
    RZUF3_ObjectDefinition objDoorASlider;
    objDoorASlider.name = "doorASlider";
    objDoorASlider.parentName = "UIRoot";
    objDoorASlider.pos = RZUF3_Pos(16 + 112, 200);
    objDoorASlider.scripts = {
        new RZUF3_Slider(0, 1, 0),
        new InputIfUISoft("siudemka", "door_f", 1, 6, 0.01),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "drzwi kabiny A", -112, -3})
    };
    m_sceneDef->objects.push_back(objDoorASlider);

    // door B //
    RZUF3_ObjectDefinition objDoorBSlider;
    objDoorBSlider.name = "doorBSlider";
    objDoorBSlider.parentName = "UIRoot";
    objDoorBSlider.pos = RZUF3_Pos(16 + 112 + 187, 200);
    objDoorBSlider.scripts = {
        new RZUF3_Slider(0, 1, 0),
        new InputIfUISoft("siudemka", "door_r", 1, 6, 0.01),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "drzwi kabiny B", -112, -3})
    };
    m_sceneDef->objects.push_back(objDoorBSlider);

    // front bogie //
    RZUF3_ObjectDefinition objFrontBogieSlider;
    objFrontBogieSlider.name = "frontBogieSlider";
    objFrontBogieSlider.parentName = "UIRoot";
    objFrontBogieSlider.pos = RZUF3_Pos(16 + 112, 232);
    objFrontBogieSlider.scripts = {
        new RZUF3_Slider({-30, 30, 0, bogieSliderStyle}),
        new InputByUI("siudemka", "rot_bogie_f", 120),
		new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "wozek przedni", -112, -3})
	};
	m_sceneDef->objects.push_back(objFrontBogieSlider);

    // rear bogie //
    RZUF3_ObjectDefinition objRearBogieSlider;
	objRearBogieSlider.name = "rearBogieSlider";
    objRearBogieSlider.parentName = "UIRoot";
    objRearBogieSlider.pos = RZUF3_Pos(16 + 112, 264);
    objRearBogieSlider.scripts = {
        new RZUF3_Slider({-30, 30, 0, bogieSliderStyle}),
        new InputByUI("siudemka", "rot_bogie_r", 120),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "wozek tylny", -112, -3})
    };
    m_sceneDef->objects.push_back(objRearBogieSlider);

    // author //
    RZUF3_ObjectDefinition objAuthor;
	objAuthor.name = "author";
    objAuthor.parentName = "UIRoot";
    objAuthor.pos = RZUF3_Pos(384 - 128, 304);
    objAuthor.scripts = {
        new RZUF3_TextRenderer("assets/fonts/roboto-regular.ttf", "masuo 2024.06")
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