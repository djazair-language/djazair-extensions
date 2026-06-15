#include "raylib_common.h"

Color getColor(djazairVM *vm, djazair_value val) {
    if (djazair_is_array(val)) {
        int count = djazair_array_len(vm, val);
        if (count >= 3) {
            unsigned char r = (unsigned char)AS_NUMBER(djazair_array_get(vm, val, 0));
            unsigned char g = (unsigned char)AS_NUMBER(djazair_array_get(vm, val, 1));
            unsigned char b = (unsigned char)AS_NUMBER(djazair_array_get(vm, val, 2));
            unsigned char a = (count >= 4) ? (unsigned char)AS_NUMBER(djazair_array_get(vm, val, 3)) : 255;
            return (Color){r, g, b, a};
        }
    }
    return BLACK;
}

float getInstanceFloat(djazairVM *vm, djazair_value inst, const char *key, float fallback) {
    djazair_value k = djazair_str(vm, key);
    djazair_value v = djazair_instance_get(vm, inst, k);
    return djazair_is_number(v) ? (float)AS_NUMBER(v) : fallback;
}
