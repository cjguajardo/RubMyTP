#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

static int file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0);
}

int audio_init(struct audio_pool *pool, const char *sounds_dir) {
    DIR *dir;
    struct dirent *entry;
    char path[512];

    memset(pool, 0, sizeof(*pool));
    pool->last_played = -1;

    if (!sounds_dir || strlen(sounds_dir) == 0) {
        return 0;
    }

    dir = opendir(sounds_dir);
    if (!dir) return -1;

    while ((entry = readdir(dir)) != NULL && pool->count < MAX_SOUNDS) {
        if (entry->d_type != DT_REG) continue;

        // Only audio files
        if (strstr(entry->d_name, ".wav") == NULL &&
            strstr(entry->d_name, ".mp3") == NULL &&
            strstr(entry->d_name, ".ogg") == NULL &&
            strstr(entry->d_name, ".flac") == NULL) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", sounds_dir, entry->d_name);
        if (file_exists(path)) {
            snprintf(pool->sounds[pool->count].path,
            sizeof(pool->sounds[pool->count].path),
                    "%s", path);
            pool->sounds[pool->count].used = 0;
            pool->count++;
        }
    }

    closedir(dir);
    srand((unsigned int)time(NULL));
    return pool->count;
}

void audio_play_random(struct audio_pool *pool) {
    char cmd[1024];
    FILE *fp;

    if (pool->count == 0) return;

    int idx = rand() % pool->count;

    // Use ffplay (portable, available on both Linux and Windows)
    // -nodisp: no display window
    // -autoexit: exit when playback finishes
    // -loglevel quiet: suppress output
    snprintf(cmd, sizeof(cmd), "ffplay -nodisp -autoexit -loglevel quiet \"%s\" 2>/dev/null",
 pool->sounds[idx].path);

    fp = popen(cmd, "r");
    if (fp) {
        pclose(fp);
    }
}

void audio_shutdown(struct audio_pool *pool) {
    memset(pool, 0, sizeof(*pool));
}
