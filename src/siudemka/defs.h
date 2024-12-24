#pragma once

#define MODEL_WIDTH 384
#define MODEL_HEIGHT 216
#define UI_HEIGHT 320
#define AUTHOR_HEIGHT 48 // 182 cm ;)

#define ALL_HEIGHT (MODEL_HEIGHT + UI_HEIGHT)
#define UI_OFFCENTER (UI_HEIGHT - MODEL_HEIGHT)

#define WINDOW_MIN_WIDTH MODEL_WIDTH
#define WINDOW_MIN_HEIGHT (ALL_HEIGHT + AUTHOR_HEIGHT)
#define WINDOW_MO_HEIGHT (MODEL_HEIGHT + AUTHOR_HEIGHT)

#define MAIN_FONT "assets/fonts/roboto-regular.ttf"
#define MODEL_PATH "assets/models/siudemka.bmd"
#define MODEL_OBJ_NAME "model"
