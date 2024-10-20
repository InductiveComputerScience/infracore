#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

void Program(AudioStructure *audio1){
	double w, h, d;
	double *pixels, i;

	double *history;
	double length;
	_Bool *state;
	double characters, controlKeys, historyMaxLength;

	_Bool done;
	int counter;
	int cur = 0;
	int size;

	// Audio
	double samplesps, mindelay, sw;
	AudioSamplesMinDelay(audio1, &samplesps, &mindelay);

	size = samplesps * 2;
	double *audio = malloc(sizeof(double) * size);

	for(i = 0; i < size; i++){
		audio[(int)i] = sin(i/15);
	}

	// Main loop
	done = false;
	for(counter = 0; !done && cur < size; counter++){

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


