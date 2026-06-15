#include "raylib_common.h"
#include <stdlib.h>

// Textures
DJAZAIR_FUNC(raylibLoadTextureNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_string(args[0])) return djazair_null();
    char resolved[4096];
    djazair_resolve_path(vm, djazair_get_str(args, 0), resolved, sizeof(resolved));
    Texture2D *texture = malloc(sizeof(Texture2D));
    if (!texture) return djazair_null();
    *texture = LoadTexture(resolved);
    return djazair_new_resource(vm, texture, "Texture");
}

DJAZAIR_FUNC(raylibDrawTextureNative) {
    djazair_check_args(3, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Texture2D *texture = (Texture2D *)djazair_get_resource(vm, args[0]);
    if (!texture) return djazair_null();
    DrawTexture(*texture, (int)djazair_get_num(args, 1),
                (int)djazair_get_num(args, 2), WHITE);
    return djazair_null();
}

DJAZAIR_FUNC(raylibUnloadTextureNative) {
    djazair_check_args(1, argCount);
    if (djazair_is_resource(args[0])) {
        Texture2D *texture = (Texture2D *)djazair_get_resource(vm, args[0]);
        if (texture) { UnloadTexture(*texture); free(texture); }
    }
    return djazair_null();
}

// Audio
DJAZAIR_FUNC(raylibInitAudioNative) {
    (void)argCount; (void)args;
    InitAudioDevice();
    return djazair_null();
}

DJAZAIR_FUNC(raylibCloseAudioNative) {
    (void)argCount; (void)args;
    CloseAudioDevice();
    return djazair_null();
}

DJAZAIR_FUNC(raylibLoadSoundNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_string(args[0])) return djazair_null();
    char resolved[4096];
    djazair_resolve_path(vm, djazair_get_str(args, 0), resolved, sizeof(resolved));
    Sound *sound = malloc(sizeof(Sound));
    if (!sound) return djazair_null();
    *sound = LoadSound(resolved);
    return djazair_new_resource(vm, sound, "Sound");
}

DJAZAIR_FUNC(raylibPlaySoundNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Sound *sound = (Sound *)djazair_get_resource(vm, args[0]);
    if (sound) PlaySound(*sound);
    return djazair_null();
}

DJAZAIR_FUNC(raylibUnloadSoundNative) {
    djazair_check_args(1, argCount);
    if (djazair_is_resource(args[0])) {
        Sound *sound = (Sound *)djazair_get_resource(vm, args[0]);
        if (sound) { UnloadSound(*sound); free(sound); }
    }
    return djazair_null();
}

// Advanced Textures
DJAZAIR_FUNC(raylibDrawTextureRecNative) {
    djazair_check_args(8, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Texture2D *texture = (Texture2D *)djazair_get_resource(vm, args[0]);
    if (!texture) return djazair_null();
    Rectangle source = {(float)djazair_get_num(args, 1), (float)djazair_get_num(args, 2),
                        (float)djazair_get_num(args, 3), (float)djazair_get_num(args, 4)};
    Vector2 position = {(float)djazair_get_num(args, 5), (float)djazair_get_num(args, 6)};
    DrawTextureRec(*texture, source, position, getColor(vm, args[7]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawTextureExNative) {
    djazair_check_args(6, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Texture2D *texture = (Texture2D *)djazair_get_resource(vm, args[0]);
    if (!texture) return djazair_null();
    Vector2 position = {(float)djazair_get_num(args, 1), (float)djazair_get_num(args, 2)};
    DrawTextureEx(*texture, position, (float)djazair_get_num(args, 3), (float)djazair_get_num(args, 4), getColor(vm, args[5]));
    return djazair_null();
}

// Fonts
DJAZAIR_FUNC(raylibLoadFontNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_string(args[0])) return djazair_null();
    char resolved[4096];
    djazair_resolve_path(vm, djazair_get_str(args, 0), resolved, sizeof(resolved));
    Font *font = malloc(sizeof(Font));
    if (!font) return djazair_null();
    *font = LoadFont(resolved);
    return djazair_new_resource(vm, font, "Font");
}

DJAZAIR_FUNC(raylibUnloadFontNative) {
    djazair_check_args(1, argCount);
    if (djazair_is_resource(args[0])) {
        Font *font = (Font *)djazair_get_resource(vm, args[0]);
        if (font) { UnloadFont(*font); free(font); }
    }
    return djazair_null();
}

DJAZAIR_FUNC(raylibDrawTextExNative) {
    djazair_check_args(7, argCount);
    if (!djazair_is_resource(args[0]) || !djazair_is_string(args[1])) return djazair_null();
    Font *font = (Font *)djazair_get_resource(vm, args[0]);
    if (!font) return djazair_null();
    Vector2 position = {(float)djazair_get_num(args, 2), (float)djazair_get_num(args, 3)};
    DrawTextEx(*font, djazair_get_str(args, 1), position, (float)djazair_get_num(args, 4), (float)djazair_get_num(args, 5), getColor(vm, args[6]));
    return djazair_null();
}

DJAZAIR_FUNC(raylibLoadMusicStreamNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_string(args[0])) return djazair_null();
    char resolved[4096];
    djazair_resolve_path(vm, djazair_get_str(args, 0), resolved, sizeof(resolved));
    Music *music = malloc(sizeof(Music));
    if (!music) return djazair_null();
    *music = LoadMusicStream(resolved);
    return djazair_new_resource(vm, music, "Music");
}

DJAZAIR_FUNC(raylibPlayMusicStreamNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Music *music = (Music *)djazair_get_resource(vm, args[0]);
    if (music) PlayMusicStream(*music);
    return djazair_null();
}

DJAZAIR_FUNC(raylibUpdateMusicStreamNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Music *music = (Music *)djazair_get_resource(vm, args[0]);
    if (music) UpdateMusicStream(*music);
    return djazair_null();
}

DJAZAIR_FUNC(raylibUnloadMusicStreamNative) {
    djazair_check_args(1, argCount);
    if (djazair_is_resource(args[0])) {
        Music *music = (Music *)djazair_get_resource(vm, args[0]);
        if (music) { UnloadMusicStream(*music); free(music); }
    }
    return djazair_null();
}

DJAZAIR_FUNC(raylibStopMusicStreamNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Music *music = (Music *)djazair_get_resource(vm, args[0]);
    if (music) StopMusicStream(*music);
    return djazair_null();
}

DJAZAIR_FUNC(raylibPauseMusicStreamNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Music *music = (Music *)djazair_get_resource(vm, args[0]);
    if (music) PauseMusicStream(*music);
    return djazair_null();
}

DJAZAIR_FUNC(raylibResumeMusicStreamNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Music *music = (Music *)djazair_get_resource(vm, args[0]);
    if (music) ResumeMusicStream(*music);
    return djazair_null();
}

DJAZAIR_FUNC(raylibIsMusicStreamPlayingNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_bool(false);
    Music *music = (Music *)djazair_get_resource(vm, args[0]);
    return djazair_bool(music ? IsMusicStreamPlaying(*music) : false);
}

DJAZAIR_FUNC(raylibSetMusicVolumeNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    Music *music = (Music *)djazair_get_resource(vm, args[0]);
    if (music) SetMusicVolume(*music, (float)djazair_get_num(args, 1));
    return djazair_null();
}
