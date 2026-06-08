# tp-sound

Hook the TrackPoint (or any mouse) button events and play random sounds.

## Build

### Linux

```bash
make linux
```

### Windows (cross-compile from Linux)

```bash
apt-get install gcc-mingw-w64
make windows CC=x86_64-w64-mingw32-gcc
```

### Docker

```bash
# Linux binary
docker build -f Dockerfile.linux -t tp-sound:linux .

# Windows binary (cross-compile)
docker build -f Dockerfile.windows -t tp-sound:windows .
docker cp $(docker create tp-sound:windows):/build/tp-sound.exe ./tp-sound.exe
```

## Usage

```bash
./tp-sound -s /path/to/sounds
```

Options:
- `-s<dir>` — sounds directory (required)
- `-d <device>` — device path (auto-detected if omitted)
- `-h` — help

## Sounds

Put `.wav`, `.mp3`, `.ogg`, or `.flac` files in a directory and pass it with `-s`.

## How it works

- **Linux**: opens `/dev/input/event*`, reads `input_event` structs, watches for `EV_KEY` button presses
- **Windows**: registers `Raw Input API` for HID mouse events, plays audio via `winmm.dll` / `PlaySound`
- On each button press, plays a random sound from the pool (round-robin without repeat)

## Exit

`Ctrl+C`
