#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

void DrawPixel(uint32_t *pixels, int w, int x, int y, int r, int g, int b){
	pixels[y*w + x] = r << 16 | g << 8 | b;
}

void Program(ScreenStructure *screen){
	double w, h, d;
	uint32_t *pixels;
	int counter;

	// Screen
	ScreenSpecs(screen, &w, &h, &d);
	pixels = malloc(4 * w * h);
	memset(pixels, 0, 4 * w * h);

	// Main loop
	for(counter = 0; counter < 300; counter++){
		// Draw Pixel
		DrawPixel(pixels, w, fmod(counter * 7, w), fmod(counter *11, h), 255, 255, 255);

		// Display Image
		Display(screen, pixels);
		Synchronize(screen);
	}

	// Free
	//free(pixels);
}


