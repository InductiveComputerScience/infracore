#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

void DrawPixel(double *pixels, double w, double x, double y, double r, double g, double b){
	pixels[(int)(3*(y*w + x) + 0)] = r;
	pixels[(int)(3*(y*w + x) + 1)] = g;
	pixels[(int)(3*(y*w + x) + 2)] = b;
}

void Program(ScreenStructure *screen1, KeyboardStructure *keyboard1){
	double w, h, d;
	double *pixels;

	double *history;
	double length;
	bool *state;
	double characters, controlKeys, historyMaxLength;

	_Bool done;
	int counter;

	// Screen
	ScreenSpecs(screen1, &w, &h, &d);
	pixels = malloc(3 * w * h * sizeof(double));
	memset(pixels, 0, 3 * w * h * sizeof(double));

	// Keyaboard
	GetKeyboardSpecs(keyboard1, &characters, &controlKeys, &historyMaxLength);
	history = malloc(historyMaxLength*sizeof(double));
	state = malloc(historyMaxLength*sizeof(bool));

	// Main loop
	done = false;
	for(counter = 0; !done; counter++){
		// Get Keyboard State
		GetKeyboardState(keyboard1, history, state, &length);

		// Exit if ESC pressed
		for(int j = 0; j < length; j++){
			if(history[(int)j] == -43 && state[(int)j]){
				done = true;
			}
		}

		// Draw Pixel
		DrawPixel(pixels, w, fmod(counter * 7, w), fmod(counter *11, h), 1, 1, 1);

		// Display Image
		Display(screen1, pixels);
		Synchronize(screen1);
	}

	// Free
	free(pixels);
	free(history);
	free(state);
}


