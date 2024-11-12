#include <Screen.h>
#include <Keyboard.h>

#include "../../../drivers/screen-windows-winapi/screen-windows-winapi.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int WINAPI WinMain(HINSTANCE rInstance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow) {
  int success;
  int returnCode;
  ScreenStructure *screen1;
  KeyboardStructure *keyboard1;

  success = CreateScreenWinAPI(&screen1, rInstance, 640, 480, 1920/0.508);

  if(success){
    CreateKeyboardWinAPI(&keyboard1, screen1);
  }

  if(success){
    Program(screen1, keyboard1);

    CloseScreenWinAPI(screen1);
    CloseKeyboardWinAPI(keyboard1);
  }

  if(success){
    returnCode = 0;
  }else{
    returnCode = 1;
  }

  return returnCode;
}



