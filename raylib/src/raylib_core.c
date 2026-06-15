#include "raylib_common.h"

// Window & Core
DJAZAIR_FUNC(raylibInitNative) {
    djazair_check_args(3, argCount);
    if (!djazair_is_number(args[0]) || !djazair_is_number(args[1]) || !djazair_is_string(args[2])) {
        djazair_error("raylib.init() expects (Number, Number, String)");
        return djazair_null();
    }
    InitWindow((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
               djazair_get_str(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(raylibCloseNative) {
    (void)argCount; (void)args;
    CloseWindow();
    return djazair_null();
}

DJAZAIR_FUNC(raylibShouldCloseNative) {
    (void)argCount; (void)args;
    return djazair_bool(WindowShouldClose());
}

DJAZAIR_FUNC(raylibSetTargetFPSNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_null();
    SetTargetFPS((int)djazair_get_num(args, 0));
    return djazair_null();
}

DJAZAIR_FUNC(raylibGetFPSNative) {
    (void)argCount; (void)args;
    return djazair_num(GetFPS());
}

DJAZAIR_FUNC(raylibGetFrameTimeNative) {
    (void)argCount; (void)args;
    return djazair_num(GetFrameTime());
}

DJAZAIR_FUNC(raylibGetScreenWidthNative) {
    (void)argCount; (void)args;
    return djazair_num(GetScreenWidth());
}

DJAZAIR_FUNC(raylibGetScreenHeightNative) {
    (void)argCount; (void)args;
    return djazair_num(GetScreenHeight());
}

DJAZAIR_FUNC(raylibSetWindowTitleNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_string(args[0])) return djazair_null();
    SetWindowTitle(djazair_get_str(args, 0));
    return djazair_null();
}

DJAZAIR_FUNC(raylibSetWindowSizeNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_number(args[0]) || !djazair_is_number(args[1])) return djazair_null();
    SetWindowSize((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1));
    return djazair_null();
}

// Drawing
DJAZAIR_FUNC(raylibBeginDrawingNative) {
    (void)argCount; (void)args;
    BeginDrawing();
    return djazair_null();
}

DJAZAIR_FUNC(raylibEndDrawingNative) {
    (void)argCount; (void)args;
    EndDrawing();
    return djazair_null();
}

DJAZAIR_FUNC(raylibClearBackgroundNative) {
    djazair_check_args(1, argCount);
    ClearBackground(getColor(vm, args[0]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawTextNative) {
    djazair_check_args(5, argCount);
    if (!djazair_is_string(args[0])) return djazair_null();
    DrawText(djazair_get_str(args, 0),
             (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2),
             (int)djazair_get_num(args, 3), getColor(vm, args[4]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibMeasureTextNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_string(args[0]) || !djazair_is_number(args[1])) return djazair_num(0);
    return djazair_num(MeasureText(djazair_get_str(args, 0), (int)djazair_get_num(args, 1)));
}

// Input: Keyboard
DJAZAIR_FUNC(raylibIsKeyDownNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsKeyDown((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibIsKeyPressedNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsKeyPressed((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibIsKeyReleasedNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsKeyReleased((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibIsKeyUpNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsKeyUp((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibGetKeyPressedNative) {
    (void)argCount; (void)args;
    return djazair_num(GetKeyPressed());
}

DJAZAIR_FUNC(raylibGetCharPressedNative) {
    (void)argCount; (void)args;
    return djazair_num(GetCharPressed());
}

// Input: Mouse
DJAZAIR_FUNC(raylibGetMouseXNative) {
    (void)argCount; (void)args;
    return djazair_num(GetMouseX());
}

DJAZAIR_FUNC(raylibGetMouseYNative) {
    (void)argCount; (void)args;
    return djazair_num(GetMouseY());
}

DJAZAIR_FUNC(raylibIsMouseButtonPressedNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsMouseButtonPressed((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibIsMouseButtonDownNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsMouseButtonDown((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibIsMouseButtonReleasedNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsMouseButtonReleased((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibIsMouseButtonUpNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_number(args[0])) return djazair_bool(false);
    return djazair_bool(IsMouseButtonUp((int)djazair_get_num(args, 0)));
}

DJAZAIR_FUNC(raylibGetMouseWheelMoveNative) {
    (void)argCount; (void)args;
    return djazair_num(GetMouseWheelMove());
}

// Collision Detection
DJAZAIR_FUNC(raylibCheckCollisionRecsNative) {
    djazair_check_args(8, argCount);
    Rectangle r1 = {(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1),
                    (float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3)};
    Rectangle r2 = {(float)djazair_get_num(args, 4), (float)djazair_get_num(args, 5),
                    (float)djazair_get_num(args, 6), (float)djazair_get_num(args, 7)};
    return djazair_bool(CheckCollisionRecs(r1, r2));
}

DJAZAIR_FUNC(raylibCheckCollisionCirclesNative) {
    djazair_check_args(6, argCount);
    Vector2 p1 = {(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1)};
    Vector2 p2 = {(float)djazair_get_num(args, 3), (float)djazair_get_num(args, 4)};
    return djazair_bool(CheckCollisionCircles(p1, (float)djazair_get_num(args, 2),
                                              p2, (float)djazair_get_num(args, 5)));
}

DJAZAIR_FUNC(raylibCheckCollisionCircleRecNative) {
    djazair_check_args(7, argCount);
    Vector2 center = {(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1)};
    float radius = (float)djazair_get_num(args, 2);
    Rectangle rec = {(float)djazair_get_num(args, 3), (float)djazair_get_num(args, 4),
                     (float)djazair_get_num(args, 5), (float)djazair_get_num(args, 6)};
    return djazair_bool(CheckCollisionCircleRec(center, radius, rec));
}

DJAZAIR_FUNC(raylibCheckCollisionPointRecNative) {
    djazair_check_args(6, argCount);
    Vector2 point = {(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1)};
    Rectangle rec = {(float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3),
                     (float)djazair_get_num(args, 4), (float)djazair_get_num(args, 5)};
    return djazair_bool(CheckCollisionPointRec(point, rec));
}

// Utility
DJAZAIR_FUNC(raylibGetRandomValueNative) {
    djazair_check_args(2, argCount);
    return djazair_num(GetRandomValue((int)djazair_get_num(args, 0),
                                     (int)djazair_get_num(args, 1)));
}

DJAZAIR_FUNC(raylibHideCursorNative) {
    (void)argCount; (void)args;
    HideCursor();
    return djazair_null();
}

DJAZAIR_FUNC(raylibShowCursorNative) {
    (void)argCount; (void)args;
    ShowCursor();
    return djazair_null();
}

DJAZAIR_FUNC(raylibGetTimeNative) {
    (void)argCount; (void)args;
    return djazair_num(GetTime());
}

DJAZAIR_FUNC(raylibToggleFullscreenNative) {
    (void)argCount; (void)args;
    ToggleFullscreen();
    return djazair_null();
}

DJAZAIR_FUNC(raylibIsWindowResizedNative) {
    (void)argCount; (void)args;
    return djazair_bool(IsWindowResized());
}
