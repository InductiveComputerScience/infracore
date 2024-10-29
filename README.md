# infracore.io
A minimalist hardware abstraction layer.

Device interfaces, drivers and compositions.

## Devices
 * screen - [interface](devices/screen/Screen.h)
 * audio - [interface](devices/audio/Audio.h)
 * clock - [interface](devices/clock/Clock.h)
 * disk - [interface](devices/disk/Disk.h)
 * processing unit - [interface](devices/pu/ProcessingUnit.h)

## Drivers
 * screen and keyboard
   * [SDL Driver](drivers/screen-linux-sdl)
 * audio
   * [ALSA Driver](drivers/audio-linux-alsa/)
 * clock
   * [C11 Driver](drivers/clock-c11/)
   * [Linux Driver](drivers/clock-linux/)
 * disk
   * [C89 File Driver](drivers/disk-c89-file/)
   * [Linux File Driver](drivers/disk-linux-file/)
 * processing unit (PU)
   * [Linux TCP Driver](drivers/pu-linux-tcp/)
 * processing unit controller (PUC)
   * [Linux Processes Driver](drivers/puc-linux-processes/)


## Examples
 * screen and keyboard
   * [Demo1](examples/screen-demo1/program.c) -- Drawing pixels to a screen.
 * audio
   * [Demo1](examples/audio-demo1/program.c) -- Sending a pure C sine wave to an audio device.
 * clock
   * [Demo1](examples/clock-demo1/program.c) -- Prints the current time on screen, waits a second, and prints the time again.
 * disk
   * [Demo1](examples/disk-demo1/program.c) -- Read a string from disk and print it.
 * processing unit (PU)
   * [Server Demo1](examples/pu-server-demo1/program.c) -- Echoes the request in upper case.
   * [Client Demo1](examples/pu-client-demo1/program.c) -- Sends the message "Hello, world!" to a server.
 * processing unit controller (PUC)
   * [Demo1](examples/puc-demo1/program.c) -- Run two programs in two processing units.
   * [Demo2](examples/puc-demo2/program.c) -- Restart on exception.
   * [Demo3](examples/puc-demo3/program.c) -- Running multiple programs but not using more than one CPU.

## How to
An infracore program consist of a function that takes the devices it uses as input. For example, a game needs a screen to display the game, an audio device to play sound, a clock to keep track of time and a disk for the game data:

```
void Program(ScreenStructure *screen1, KeyboardStructure *keyboard1, AudioStructure *audio1, ClockStructure *clock1, DiskStructure *disk1){
  ...
}
```

## Book
The code here is based on the contents of the book [Foundations of Computer Science](https://www.amazon.com/dp/B0B3N58GT7/).

![Foundations of Computer Science](bookcover.png)

