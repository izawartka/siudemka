﻿#include "rzuf3/rzuf.h"
#include "sge/sge.h"
#include "siudemka/siudemka.h"

RZUF3_Game* game = nullptr;

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
int SDL_main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);

    game = new RZUF3_Game();
    game->initWindow("siudemka", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 384, 528, false);

    RZUF3_SceneDefinition* sceneDef = MainScene::getSceneDef();

    game->setScene(sceneDef);
    game->startGameLoop();

    delete game;
    MainScene::destroy();

    return 0;
}
