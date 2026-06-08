CC = gcc
CFLAGS = -Wall -Wextra -O2 -DNDEBUG
LDFLAGS =

# Linux
TARGET_LINUX = tp-sound
SRC_LINUX = src/main.c src/device_linux.c src/audio_ffmpeg.c

# Windows (MinGW)
TARGET_WINDOWS = tp-sound.exe
SRC_WINDOWS = src/main.c src/device_windows.c src/audio_winmm.c

.PHONY: all linux windows clean

all: linux windows

linux: $(TARGET_LINUX)

$(TARGET_LINUX): $(SRC_LINUX)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

windows: $(TARGET_WINDOWS)

$(TARGET_WINDOWS): $(SRC_WINDOWS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lwinmm -luser32 -lhid

clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS)
