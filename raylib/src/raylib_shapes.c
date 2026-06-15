#include "raylib_common.h"

DJAZAIR_FUNC(raylibDrawRectangleNative) {
    djazair_check_args(5, argCount);
    DrawRectangle((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
                  (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3),
                  getColor(vm, args[4]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawRectangleLinesNative) {
    djazair_check_args(5, argCount);
    DrawRectangleLines((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
                       (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3),
                       getColor(vm, args[4]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawRectangleGradientVNative) {
    djazair_check_args(6, argCount);
    DrawRectangleGradientV((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
                           (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3),
                           getColor(vm, args[4]), getColor(vm, args[5]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawCircleNative) {
    djazair_check_args(4, argCount);
    DrawCircle((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
               (float)djazair_get_num(args, 2), getColor(vm, args[3]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawCircleLinesNative) {
    djazair_check_args(4, argCount);
    DrawCircleLines((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
                    (float)djazair_get_num(args, 2), getColor(vm, args[3]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawLineNative) {
    djazair_check_args(5, argCount);
    DrawLine((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
             (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3),
             getColor(vm, args[4]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawLineExNative) {
    djazair_check_args(6, argCount);
    DrawLineEx(
        (Vector2){(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1)},
        (Vector2){(float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3)},
        (float)djazair_get_num(args, 4), getColor(vm, args[5]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawTriangleNative) {
    djazair_check_args(7, argCount);
    DrawTriangle(
        (Vector2){(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1)},
        (Vector2){(float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3)},
        (Vector2){(float)djazair_get_num(args, 4), (float)djazair_get_num(args, 5)},
        getColor(vm, args[6]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawTriangleLinesNative) {
    djazair_check_args(7, argCount);
    DrawTriangleLines(
        (Vector2){(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1)},
        (Vector2){(float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3)},
        (Vector2){(float)djazair_get_num(args, 4), (float)djazair_get_num(args, 5)},
        getColor(vm, args[6]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawPolyNative) {
    djazair_check_args(6, argCount);
    DrawPoly(
        (Vector2){(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1)},
        (int)djazair_get_num(args, 2), (float)djazair_get_num(args, 3),
        (float)djazair_get_num(args, 4), getColor(vm, args[5]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawRectangleRoundedNative) {
    djazair_check_args(7, argCount);
    Rectangle rec = {(float)djazair_get_num(args, 0), (float)djazair_get_num(args, 1),
                     (float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3)};
    DrawRectangleRounded(rec, (float)djazair_get_num(args, 4), (int)djazair_get_num(args, 5), getColor(vm, args[6]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawEllipseNative) {
    djazair_check_args(5, argCount);
    DrawEllipse((int)djazair_get_num(args, 0), (int)djazair_get_num(args, 1),
                (float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3),
                getColor(vm, args[4]));
    return djazair_null();
}
