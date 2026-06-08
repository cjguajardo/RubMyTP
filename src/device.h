#ifndef DEVICE_H
#define DEVICE_H

#include <stddef.h>

struct device {
    int fd;                  // Linux: file descriptor; Windows: handle
    void *handle;            // Windows: HANDLE
    char name[256];
    int is_trackpoint;
};

int device_init(struct device *dev, const char *device_path);
int device_poll(struct device *dev);
void device_close(struct device *dev);
const char *device_find_trackpoint(void);

#endif // DEVICE_H
