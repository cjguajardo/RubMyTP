#ifndef AUDIO_H
#define AUDIO_H

#define MAX_SOUNDS 64

struct sound {
    char path[512];
    int used;
};

struct audio_pool {
    struct sound sounds[MAX_SOUNDS];
    int count;
    int last_played;
};

int audio_init(struct audio_pool *pool, const char *sounds_dir);
void audio_play_random(struct audio_pool *pool);
void audio_shutdown(struct audio_pool *pool);

#endif // AUDIO_H
