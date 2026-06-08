#include "device.h"
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
static HANDLE poll_thread;
static volatile int running = 1;

static DWORD WINAPI poll_loop(LPVOID arg) {
    struct device *dev = (struct device *)arg;
    while (running) {
        if (device_poll(dev)) {
            audio_play_random((struct audio_pool *)((char*)arg - sizeof(struct audio_pool)));
        }
        Sleep(10);
    }
    return 0;
}
#else
#include <unistd.h>
static volatile int running = 1;

static void signal_handler(int sig) {
    (void)sig;
    running = 0;
}
#endif

int main(int argc, char *argv[]) {
    struct device dev;
    struct audio_pool pool;
    const char *sounds_dir = NULL;
    const char *device_path = NULL;
    int i;

    // Simple argument parsing
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            device_path = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            sounds_dir = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [-d /dev/input/eventX] [-s sounds_dir]\n", argv[0]);
            printf("  -d  Device path (auto-detect if omitted)\n");
            printf("  -s  Sounds directory (required)\n");
            return 0;
        }
    }

    if (!sounds_dir) {
        fprintf(stderr, "Error: -s sounds_dir is required\n");
        return 1;
    }

    if (audio_init(&pool, sounds_dir) < 0) {
        fprintf(stderr, "Error: could not load sounds from %s\n", sounds_dir);
        return 1;
    }

    if (pool.count == 0) {
        fprintf(stderr, "Error: no audio files found in %s\n", sounds_dir);
        return 1;
    }

    printf("Loaded %d sound(s) from %s\n", pool.count, sounds_dir);

    if (!device_path) {
        device_path = device_find_trackpoint();
    }

    if (!device_path) {
        fprintf(stderr, "Error: could not find TrackPoint device\n");
        return 1;
    }

    printf("Using device: %s\n", device_path);

    if (device_init(&dev, device_path) < 0) {
        fprintf(stderr, "Error: could not open device %s\n", device_path);
        return 1;
    }

#ifdef _WIN32
    running = 1;
    poll_thread = CreateThread(NULL, 0, poll_loop, &dev, 0, NULL);
    if (!poll_thread) {
        fprintf(stderr, "Error: could not start poll thread\n");
        device_close(&dev);
        return 1;
    }

    printf("Listening... (Ctrl+C to stop)\n");
    while (running) {
        Sleep(100);
    }

    running = 0;
    WaitForSingleObject(poll_thread, INFINITE);
#else
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("Listening... (Ctrl+C to stop)\n");

    while (running) {
        if (device_poll(&dev)) {
            audio_play_random(&pool);
        }
        usleep(10000);  // 10ms
    }
#endif

    device_close(&dev);
    audio_shutdown(&pool);
    printf("Done.\n");
    return 0;
}
