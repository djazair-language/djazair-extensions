#include "raylib_common.h"

DJAZAIR_FUNC(raylibIsColorNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_instance(args[0])) return djazair_bool(false);
    djazair_value class_name = djazair_instance_class_name(args[0]);
    if (djazair_is_string(class_name)) {
        return djazair_bool(strcmp(AS_CSTRING(class_name), "Color") == 0);
    }
    return djazair_bool(false);
}

static NativeMethod raylib_module_funcs[] = {
    /* Window & Core */
    {"init",            raylibInitNative,            3},
    {"close",           raylibCloseNative,           0},
    {"shouldClose",     raylibShouldCloseNative,     0},
    {"setTargetFPS",    raylibSetTargetFPSNative,    1},
    {"getFPS",          raylibGetFPSNative,          0},
    {"getFrameTime",    raylibGetFrameTimeNative,    0},
    {"getScreenWidth",  raylibGetScreenWidthNative,  0},
    {"getScreenHeight", raylibGetScreenHeightNative, 0},
    {"setWindowTitle",  raylibSetWindowTitleNative,  1},
    {"setWindowSize",   raylibSetWindowSizeNative,   2},
    /* Drawing */
    {"beginDrawing",            raylibBeginDrawingNative,           0},
    {"endDrawing",              raylibEndDrawingNative,             0},
    {"clearBackground",         raylibClearBackgroundNative,        1},
    {"drawText",                raylibDrawTextNative,               5},
    {"measureText",             raylibMeasureTextNative,            2},
    {"drawRectangle",           raylibDrawRectangleNative,          5},
    {"drawRectangleLines",      raylibDrawRectangleLinesNative,     5},
    {"drawRectangleGradientV",  raylibDrawRectangleGradientVNative, 6},
    {"drawRectangleRounded",    raylibDrawRectangleRoundedNative,   7},
    {"drawCircle",              raylibDrawCircleNative,             4},
    {"drawCircleLines",         raylibDrawCircleLinesNative,        4},
    {"drawEllipse",             raylibDrawEllipseNative,            5},
    {"drawLine",                raylibDrawLineNative,               5},
    {"drawLineEx",              raylibDrawLineExNative,             6},
    {"drawTriangle",            raylibDrawTriangleNative,           7},
    {"drawTriangleLines",       raylibDrawTriangleLinesNative,      7},
    {"drawPoly",                raylibDrawPolyNative,               6},
    /* Input: Keyboard */
    {"isKeyDown",       raylibIsKeyDownNative,       1},
    {"isKeyPressed",    raylibIsKeyPressedNative,    1},
    {"isKeyReleased",   raylibIsKeyReleasedNative,   1},
    {"isKeyUp",         raylibIsKeyUpNative,         1},
    {"getKeyPressed",   raylibGetKeyPressedNative,   0},
    {"getCharPressed",  raylibGetCharPressedNative,  0},
    /* Input: Mouse */
    {"getMouseX",             raylibGetMouseXNative,             0},
    {"getMouseY",             raylibGetMouseYNative,             0},
    {"isMouseButtonPressed",  raylibIsMouseButtonPressedNative,  1},
    {"isMouseButtonDown",     raylibIsMouseButtonDownNative,     1},
    {"isMouseButtonReleased", raylibIsMouseButtonReleasedNative, 1},
    {"isMouseButtonUp",       raylibIsMouseButtonUpNative,       1},
    {"getMouseWheelMove",     raylibGetMouseWheelMoveNative,     0},
    /* Collision */
    {"checkCollisionRecs",    raylibCheckCollisionRecsNative,    8},
    {"checkCollisionCircles", raylibCheckCollisionCirclesNative, 6},
    {"checkCollisionCircleRec",raylibCheckCollisionCircleRecNative, 7},
    {"checkCollisionPointRec", raylibCheckCollisionPointRecNative,  6},
    /* Utility */
    {"getRandomValue",    raylibGetRandomValueNative,   2},
    {"hideCursor",        raylibHideCursorNative,       0},
    {"showCursor",        raylibShowCursorNative,       0},
    {"getTime",           raylibGetTimeNative,          0},
    {"toggleFullscreen",  raylibToggleFullscreenNative, 0},
    {"isWindowResized",   raylibIsWindowResizedNative,  0},
    /* Textures */
    {"loadTexture",   raylibLoadTextureNative,   1},
    {"drawTexture",   raylibDrawTextureNative,   3},
    {"unloadTexture", raylibUnloadTextureNative, 1},
    {"drawTextureRec",raylibDrawTextureRecNative,8},
    {"drawTextureEx", raylibDrawTextureExNative, 6},
    /* Audio */
    {"initAudio",            raylibInitAudioNative,            0},
    {"closeAudio",           raylibCloseAudioNative,           0},
    {"loadSound",            raylibLoadSoundNative,            1},
    {"playSound",            raylibPlaySoundNative,            1},
    {"unloadSound",          raylibUnloadSoundNative,          1},
    {"loadMusicStream",      raylibLoadMusicStreamNative,      1},
    {"playMusicStream",      raylibPlayMusicStreamNative,      1},
    {"updateMusicStream",    raylibUpdateMusicStreamNative,    1},
    {"unloadMusicStream",    raylibUnloadMusicStreamNative,    1},
    {"stopMusicStream",      raylibStopMusicStreamNative,      1},
    {"pauseMusicStream",     raylibPauseMusicStreamNative,     1},
    {"resumeMusicStream",    raylibResumeMusicStreamNative,    1},
    {"isMusicStreamPlaying", raylibIsMusicStreamPlayingNative, 1},
    {"setMusicVolume",       raylibSetMusicVolumeNative,       2},
    /* Camera2D */
    {"beginMode2D",       raylibBeginMode2DNative, 1},
    {"endMode2D",         raylibEndMode2DNative,   0},
    {"getScreenToWorld2D",raylibGetScreenToWorld2DNative, 3},
    {"getWorldToScreen2D",raylibGetWorldToScreen2DNative, 3},
    /* Fonts */
    {"loadFont",    raylibLoadFontNative,    1},
    {"unloadFont",  raylibUnloadFontNative,  1},
    {"drawTextEx",  raylibDrawTextExNative,  7},
    /* Type helpers */
    {"isColor",     raylibIsColorNative,     1},
    {NULL, NULL, 0}
};

DJAZAIR_EXTENSION(raylib, raylib_module_funcs)
