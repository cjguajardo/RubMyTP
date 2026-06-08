#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>

#pragma comment(lib, "winmm.lib")

static int playsound(const char *path) {
    // Try PlaySound (simplest, built-in)
    if (PlaySound(path, NULL, SND_FILENAME | SND_ASYNC)) {
        return 0;
    }
    return -1;
}

int audio_init(struct audio_pool *pool, const char *sounds_dir) {
    WIN32_FIND_DATAA find_data;
    HANDLE hFind;
    char search_path[512];
    char full_path[512];

    memset(pool, 0, sizeof(*pool));
    pool->last_played = -1;

    if (!sounds_dir || strlen(sounds_dir) == 0) {
        return 0;
    }

    snprintf(search_path, sizeof(search_path), "%s\\*.*", sounds_dir);

    hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) return -1;

    do {
        if (find_data.dwFileAttributes& FILE_ATTRIBUTE_DIRECTORY) continue;

        if (strstr(find_data.cFileName, ".wav") == NULL &&
            strstr(find_data.cFileName, ".mp3") == NULL &&
            strstr(find_data.cFileName, ".ogg") == NULL &&
            strstr(find_data.cFileName, ".flac") == NULL) {
            continue;
        }

        if (pool->count >= MAX_SOUNDS) break;

        snprintf(full_path, sizeof(full_path), "%s\\%s", sounds_dir, find_data.cFileName);
        snprintf(pool->sounds[pool->count].path,
                sizeof(pool->sounds[pool->count].path),
                "%s", full_path);
        pool->sounds[pool->count].used = 0;
        pool->count++;

    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);
    srand((unsigned int)time(NULL));
    return pool->count;
}

void audio_play_random(struct audio_pool *pool) {
    if (pool->count == 0) return;
    int idx = rand() % pool->count;
    playsound(pool->sounds[idx].path);
}

void audio_shutdown(struct audio_pool *pool) {
    memset(pool, 0, sizeof(*pool));
}
