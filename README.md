# infracore.io
A minimalist hardware abstraction layer. 

It separates hardware into seven different devices with only a few API functions each. Everything the computer can do should be possible with just these API functions. Experience the joy of programming with an easily underatandable API.

This repo contains device interfaces, drivers and example programs.

## Devices
 * screen - [interface](devices/screen/Screen.h)
 * keyboard - [interface](devices/screen/Keyboard.h)
 * audio - [interface](devices/audio/Audio.h)
 * clock - [interface](devices/clock/Clock.h)
 * disk - [interface](devices/disk/Disk.h)
 * processing unit (PU) - [interface](devices/pu/ProcessingUnit.h)
 * processing unit controller (PUC) - [interface](devices/puc/ProcessingUnitController.h)

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

## API Functions

### Screen

 * [ScreenSpecs](https://www.progsbase.com/isa/spc/)
 * [Display](https://www.progsbase.com/isa/sdsp/)
 * [Synchronize](https://www.progsbase.com/isa/syn/)

### Keyboard

 * [GetKeyboardSpecs](https://www.progsbase.com/isa/ksp/)
 * [GetKeyboardState](https://www.progsbase.com/isa/kst/)

### Clock

 * [ClockSpecs](https://www.progsbase.com/isa/csp/)
 * [CurrentTime](https://www.progsbase.com/isa/ctm/)

### Disk

 * [Entries](https://www.progsbase.com/isa/dsp/)
 * [Write](https://www.progsbase.com/isa/dwr/)
 * [Read](https://www.progsbase.com/isa/drd/)

### Processing Unit (PU)

 * [Send](https://www.progsbase.com/isa/snd/) | [ServerSend](https://www.progsbase.com/isa/srv-snd/)
 * [CheckSend](https://www.progsbase.com/isa/chs/) | [ServerCheckSend](https://www.progsbase.com/isa/srv-chs/)
 * [Receive](https://www.progsbase.com/isa/rcv/) | [ServerReceive](https://www.progsbase.com/isa/srv-rcv/)
 * [Check](https://www.progsbase.com/isa/chk/) | [ServerCheck](https://www.progsbase.com/isa/srv-chk/)

### Processing Unit Controller (PUC)

 * [GetProcessingUnitsAndPrograms](https://www.progsbase.com/isa/psp/)
 * [Start](https://www.progsbase.com/isa/psta/)
 * [Stop](https://www.progsbase.com/isa/pstp/)
 * [SetProgram](https://www.progsbase.com/isa/pse/)
 * [WasException](https://www.progsbase.com/isa/pex/)

## How to

You can clone this repository and try out any of the examples. Build by running `make.sh` and then run `./program` to run the demo program.

A typical program has the following parts: A launcher, a program and a build script.

### Launcher -- Creates the Devices
Initialize all the devices that your program will use. Each device is created using a device driver. In this example the SDL driver is used for the screen and a file driver is used for the disk. The screen is 640x480 and the disk is 1024 bytes, two blocks of 512 bytes, and stored in the file `disk.dat`.

```
CreateScreenLinuxSDL(&screen1, 640, 480, 1920/0.508);
CreateC89File(&disk1, "../disk.dat", 2, 512);

Program(screen1, disk1);

CloseScreenLinuxSDL(screen1);
CloseC89File(disk1);
```

### Program -- Your Program

An infracore program consist of a function that takes the devices it uses as input. For example, this program can use the screen and read data from and write data to the disk.

```
void Program(ScreenStructure *screen1, DiskStructure *disk1){
  ...
}
```

Another example would be a game. It needs a screen to display the game, an audio device to play sound, a clock to keep track of time and a disk for the game data. Then you would pass the following devices to the program: screen, audio, clock and disk.

### Build Script -- Builds the Program
Finally, the program needs to be built.

```
# Drivers:
gcc -c infracore/drivers/screen-linux-sdl/screen-linux-sdl.c -I infracore/devices/screen/
gcc -c infracore/drivers/disk-c89-file/disk-c89-file.c -std=c89 -I infracore/devices/disk/

# Launcher:
gcc -c launcher.c -I infracore/devices/screen -I infracore/devices/disk

# Program:
gcc -c program.c -I infracore/devices/screen -I infracore/devices/disk

# Make executable:
gcc -o program program.o launcher.o screen-linux-sdl.o disk-c89-file.o -lm -lpthread -lSDL2 -lrt
```

## Book
The code here is based on the contents of the book [Foundations of Computer Science](https://www.amazon.com/dp/B0B3N58GT7/).

![Foundations of Computer Science](bookcover.png)

