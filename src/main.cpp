﻿#include "rzuf3/rzuf.h"
#include "sge/sge.h"
#include "siudemka/siudemka.h"

RZUF3_Game* game = nullptr;

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
int main(int argc, char* argv[]) {
    #ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
    #else
        spdlog::set_level(spdlog::level::off);
    #endif // _DEBUG

    game = new RZUF3_Game();
    game->loadLanguage("assets/lang_pl.txt");
    game->initWindow(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT, false);
    game->setWindowTitle("window_title", true);
    game->setWindowIcon("assets/icon.png");
    game->setWindowMinimumSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    game->setWindowResizable(true);

    RZUF3_SceneDefinition* sceneDef = MainScene::getSceneDef();

    game->setScene(sceneDef);
    game->startGameLoop();

    delete game;
    MainScene::destroy();

    return 0;
}
