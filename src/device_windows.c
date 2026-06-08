#include "device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <hidusage.h>

static RAWINPUTDEVICE rid;

int device_init(struct device *dev, const char *device_path) {
    memset(dev, 0, sizeof(*dev));

    // Windows: Register for raw input from HID devices (mouse)
    rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid.usUsage = HID_USAGE_GENERIC_MOUSE;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = GetConsoleWindow();

    if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
        return -1;
    }

    dev->handle = (void*)rid.hwndTarget;
    snprintf(dev->name, sizeof(dev->name), "%s", "HID Mouse (TrackPoint)");
    dev->is_trackpoint = 1;
    return 0;
}

int device_poll(struct device *dev) {
    MSG msg;
    RAWINPUT ri;
    UINT size = sizeof(RAWINPUT);

    // PeekMessage without blocking
    if (PeekMessage(&msg, NULL, WM_INPUT, WM_INPUT, PM_REMOVE)) {
        if (GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, &ri, &size, sizeof(RAWINPUTHEADER)) > 0) {
            if (ri.header.dwType == RIM_TYPEMOUSE) {
                USHORT flags = ri.data.mouse.usButtonFlags;
                if (flags& RI_MOUSE_BUTTON_1_DOWN) return 1;  // left button
                if (flags & RI_MOUSE_BUTTON_2_DOWN) return 1;  // right button
                if (flags & RI_MOUSE_BUTTON_3_DOWN) return 1;  // middle button
            }
        }
    }

    return 0;
}

void device_close(struct device *dev) {
    (void)dev;
    // No cleanup needed - OS releases resources on process exit
}

const char *device_find_trackpoint(void) {
    // On Windows, trackpoint detection is more complex (requires enumerating HID devices)
    // For now, return a placeholder - the device_init registers for all mouse input
    // A real implementation would enumerate PnP devices via SetupAPI
    static char path[256] = "HID\\TrackPoint";
    return path;
}
