#include <Screen.h>
#include <Keyboard.h>

#include "../../../drivers/screen-linux-sdl/screen-linux-sdl.h"
#include "../../../drivers/screen-linux-sdl/keyboard-linux-sdl.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int main(){
  int success;
  int returnCode;
  ScreenStructure *screen1;
  KeyboardStructure *keyboard1;

  //success = CreateScreenLinuxSDL(&screen1, 320, 200, 1920/0.508); // Classic DOS games
  success = CreateScreenLinuxSDL(&screen1, 640, 480, 1920/0.508);
  //success = CreateScreenLinuxSDL(&screen1, 1280, 720, 1920/0.508); // 720p
  //success = CreateScreenLinuxSDL(&screen1, 1920, 1080, 1920/0.508); // 1080p

  if(success){
    success = CreateKeyboardLinuxSDL(&keyboard1, screen1);
  }

  if(success){
    Program(screen1, keyboard1);

    CloseScreenLinuxSDL(screen1);
    CloseKeyboardLinuxSDL(keyboard1);
  }

  if(success){
    returnCode = 0;
  }else{
    returnCode = 1;
  }

  return returnCode;
}



