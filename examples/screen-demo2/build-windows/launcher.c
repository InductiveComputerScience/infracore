#include <Screen.h>

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

  //success = CreateScreenWinAPI(&screen1, rInstance, 320, 200, 1920/0.508); // Classic DOS games
  success = CreateScreenWinAPI(&screen1, rInstance, 640, 480, 1920/0.508);
  //success = CreateScreenWinAPI(&screen1, rInstance, 1280, 720, 1920/0.508); // 720p
  //success = CreateScreenWinAPI(&screen1, rInstance, 1920, 1080, 1920/0.508); // 1080p

  if(success){
    Program(screen1);

    CloseScreenWinAPI(screen1);
  }

  if(success){
    returnCode = 0;
  }else{
    returnCode = 1;
  }

  return returnCode;
}



