#include "program.h"

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

void Program(AudioStructure *audio1){
  double i, samplesps, mindelay, sw;
  int counter, cur, size;

  // Audio
  AudioSamplesMinDelay(audio1, &samplesps, &mindelay);

  size = samplesps * 2;
  double *audio = malloc(sizeof(double) * size);

  for(i = 0; i < size; i++){
    double hz = 261.626;

    audio[(int)i] = sin(i/samplesps*(2*M_PI) * hz);
  }

  // Main loop
  cur = 0;
  while(cur < size){

    // Write next audio samles
    double delay = CurrentDelay(audio1);
    if(delay < mindelay * 1.5){
      WriteSamples(audio1, audio + cur, fmin(mindelay, size - cur), &sw);
      cur += sw;
    }
  }

  // Free
  free(audio);
}


