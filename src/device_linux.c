#include "device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>

int device_init(struct device *dev, const char *device_path) {
    memset(dev, 0, sizeof(*dev));
    dev->fd = open(device_path, O_RDONLY);
    if (dev->fd < 0) {
        return -1;
    }
    snprintf(dev->name, sizeof(dev->name), "%s", device_path);
    dev->is_trackpoint = 1;
    return 0;
}

int device_poll(struct device *dev) {
    struct input_event ev;
    ssize_t n = read(dev->fd, &ev, sizeof(ev));
    if (n != sizeof(ev)) {
        return 0;
    }
    if (ev.type == EV_KEY && (ev.code == BTN_LEFT || ev.code == BTN_RIGHT || ev.code == BTN_MIDDLE)) {
        if (ev.value == 1) {
            return 1;  // button pressed
        }
    }
    return 0;
}

void device_close(struct device *dev) {
    if (dev->fd >= 0) {
        close(dev->fd);
        dev->fd = -1;
    }
}

const char *device_find_trackpoint(void) {
    static char path[512];
    DIR *dir;
    struct dirent *entry;
    FILE *f;
    char dev_path[512];
    char name[256];
    int found = 0;

    dir = opendir("/dev/input");
    if (!dir) return NULL;

    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "event", 5) != 0) continue;

        snprintf(dev_path, sizeof(dev_path), "/dev/input/%s", entry->d_name);

        f = fopen(dev_path, "r");
        if (!f) continue;

        if (fgets(name, sizeof(name), f) == NULL) {
            fclose(f);
            continue;
        }
        fclose(f);

        if (strstr(name, "TrackPoint") || strstr(name, "trackpoint") ||
            strstr(name, "Pointing Stick") || strstr(name, "TPPS/2")) {
            snprintf(path, sizeof(path), "%s", dev_path);
            found = 1;
            break;
        }
    }

    closedir(dir);

    if (!found) {
        // fallback: try to find any mouse device
        dir = opendir("/dev/input");
        if (!dir) return NULL;

        while ((entry = readdir(dir)) != NULL) {
            if (strncmp(entry->d_name, "event", 5) != 0) continue;

            snprintf(dev_path, sizeof(dev_path), "/dev/input/%s", entry->d_name);

            f = fopen(dev_path, "r");
            if (!f) continue;

            if (fgets(name, sizeof(name), f) == NULL) {
                fclose(f);
                continue;
            }
            fclose(f);

            if (strstr(name, "mouse") || strstr(name, "Mouse")) {
                snprintf(path, sizeof(path), "%s", dev_path);
                found = 1;
                break;
            }
        }
        closedir(dir);
    }

    return found ? path : NULL;
}
