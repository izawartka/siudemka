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
        new RotateModel(0, true),
        new RotateByDrag({0, 0, 384, 192})
    };
    m_sceneDef->objects.push_back(objSiudemka);

    // UI root //
    RZUF3_ObjectDefinition objUIRoot;
    objUIRoot.name = "UIRoot";
    objUIRoot.pos = RZUF3_Pos(0, 208);
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
    std::string lightSideNames[] = { "F", "R" };
    std::string lightNames[] = { "L", "T", "R" };
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
            std::string lightName = "light" + lightSideNames[j] + lightNames[i];
            int sliderMin = i == 1 ? 0 : -1;
            int inputIndex = j * 6 + i * 2;

            RZUF3_ObjectDefinition objLightSlider;
            objLightSlider.name = lightName + "Slider";
            objLightSlider.parentName = "UIRoot";
            objLightSlider.pos = RZUF3_Pos(sliderX, sliderY);
            objLightSlider.scripts = {
                new RZUF3_Slider(sliderMin, 1, 0),
                new InputIfUISoft("siudemka", -1, lightInputs[inputIndex], 255, 0.75),
                new InputIfUISoft("siudemka", 1,  lightInputs[inputIndex + 1], 255, 0.75),
                new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", lightDisplayNames[i], -47, -3})
            };

            m_sceneDef->objects.push_back(objLightSlider);
        }
    }

    // pantograph A //
    RZUF3_ObjectDefinition objPantographASlider;
    objPantographASlider.name = "pantographASlider";
    objPantographASlider.parentName = "UIRoot";
    objPantographASlider.pos = RZUF3_Pos(16 + 96, 168);
    objPantographASlider.scripts = {
		new RZUF3_Slider(0, 1, 0),
		new InputIfUISoft("siudemka", 1, 13, 6, 0.001),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "pantograf A", -96, -3})
	};
    m_sceneDef->objects.push_back(objPantographASlider);

    // pantograph B //
    RZUF3_ObjectDefinition objPantographBSlider;
	objPantographBSlider.name = "pantographBSlider";
    objPantographBSlider.parentName = "UIRoot";
    objPantographBSlider.pos = RZUF3_Pos(16 + 96 + 203, 168);
    objPantographBSlider.scripts = {
        new RZUF3_Slider(0, 1, 0),
        new InputIfUISoft("siudemka", 1, 14, 6, 0.001),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "pantograf B", -96, -3})
    };
    m_sceneDef->objects.push_back(objPantographBSlider);

    RZUF3_SliderStyle bogieSliderStyle;
    bogieSliderStyle.width = 235;

    // front bogie //
    RZUF3_ObjectDefinition objFrontBogieSlider;
    objFrontBogieSlider.name = "frontBogieSlider";
    objFrontBogieSlider.parentName = "UIRoot";
    objFrontBogieSlider.pos = RZUF3_Pos(16 + 112, 208);
    objFrontBogieSlider.scripts = {
        new RZUF3_Slider({-30, 30, 0, bogieSliderStyle}),
		new InputByUI("siudemka", 11, 120),
		new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "wozek przedni", -112, -3})
	};
	m_sceneDef->objects.push_back(objFrontBogieSlider);

    // rear bogie //
    RZUF3_ObjectDefinition objRearBogieSlider;
	objRearBogieSlider.name = "rearBogieSlider";
    objRearBogieSlider.parentName = "UIRoot";
    objRearBogieSlider.pos = RZUF3_Pos(16 + 112, 240);
    objRearBogieSlider.scripts = {
        new RZUF3_Slider({-30, 30, 0, bogieSliderStyle}),
        new InputByUI("siudemka", 12, 120),
        new RZUF3_TextRenderer({"assets/fonts/roboto-regular.ttf", "wozek tylny", -112, -3})
    };
    m_sceneDef->objects.push_back(objRearBogieSlider);

    // author //
    RZUF3_ObjectDefinition objAuthor;
	objAuthor.name = "author";
    objAuthor.parentName = "UIRoot";
    objAuthor.pos = RZUF3_Pos(384 - 128, 288);
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