# infracore.io
Device interfaces, drivers and compositions.

## Devices
 * screen - [interface](devices/screen/Screen.h)
 * audio - [interface](devices/audio/Audio.h)
 * clock - [interface](devices/clock/Clock.h)
 * disk - [interface](devices/disk/Disk.h)

## Drivers
 * screen
   * [SDL Driver](drivers/screen-linux-sdl)
 * audio
   * [ALSA Driver](drivers/audio-linux-alsa/)
 * clock
   * [C11 Driver](drivers/clock-c11/)
   * [Linux Driver](drivers/clock-linux/)



## Examples
 * screen
   * [Demo1](examples/screen-demo1) -- Drawing pixels to a screen.
 * audio
   * [Demo1](examples/audio-demo1) -- Sending a pure C sine wave to an audio device.
 * clock
   * [Demo1](examples/clock-demo1) -- Prints the current time on screen, waits a second, and writes the time again.

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

