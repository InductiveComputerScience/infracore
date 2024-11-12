#include <Audio.h>

#include "../../../drivers/audio-windows-waveform/audio-windows-waveform.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int main() {
  int success;
  int returnCode;
  AudioStructure *audio1;

  success = CreateAudioWindowsWaveform(&audio1, 48000);

  if(success){
    Program(audio1);

    CloseAudioWindowsWaveform(audio1);
  }

  if(success){
    returnCode = 0;
  }else{
    returnCode = 1;
  }

  return returnCode;
}


