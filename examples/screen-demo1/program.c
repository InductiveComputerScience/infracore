#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

void DrawPixel(uint32_t *pixels, int w, int x, int y, int r, int g, int b){
	pixels[y*w + x] = r << 16 | g << 8 | b;
}

void Program(ScreenStructure *screen, KeyboardStructure *keyboard){
	double w, h, d;
	uint32_t *pixels;

	double *history;
	double length;
	bool *state;
	double characters, controlKeys, historyMaxLength;

	_Bool done;
	int counter;

	// Screen
	ScreenSpecs(screen, &w, &h, &d);
	pixels = malloc(4 * w * h);
	memset(pixels, 0, 4 * w * h);

	// Keyaboard
	GetKeyboardSpecs(keyboard, &characters, &controlKeys, &historyMaxLength);
	history = malloc(historyMaxLength*sizeof(double));
	state = malloc(historyMaxLength*sizeof(bool));

	// Main loop
	done = false;
	for(counter = 0; !done; counter++){
		// Get Keyboard State
		GetKeyboardState(keyboard, history, state, &length);

		// Exit if ESC pressed
		for(int j = 0; j < length; j++){
			if(history[(int)j] == -43 && state[(int)j]){
				done = true;
			}
		}

		// Draw Pixel
		DrawPixel(pixels, w, fmod(counter * 7, w), fmod(counter *11, h), 255, 255, 255);

		// Display Image
		Display(screen, pixels);
		Synchronize(screen);
	}

	// Free
	free(pixels);
	free(history);
	free(state);
}


