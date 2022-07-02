#include <Screen.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void DrawFilledRectangle(double *image, double x, double y, double w, double h, double W, double *color){
  double i, j;

  for(i = 0.0; i < w; i = i + 1.0){
    for(j = 0.0; j < h; j = j + 1.0){
      image[(int)((y+j)*W + (x+i))*3+0] = color[0];
      image[(int)((y+j)*W + (x+i))*3+1] = color[1];
      image[(int)((y+j)*W + (x+i))*3+2] = color[2];
    }
  }
}


// Screen Controller PU Program
void ScreenPUProgram(){
	double w, h, d;

	ScreenSize(0, &w, &h, &d);

	double *pixels = malloc(w * h * sizeof(double) * 3);

	int x = 10;
	int y = 10;
	int dx = 1;
	int dy = 1;

	memset(pixels, 0, w * h * sizeof(double) * 3);

	for(;;){
		x += dx;
		y += dy;

		if(x <= 0){
			dx = 1;
		}
		if(y <= 0){
			dy = 1;
		}
		if(x >= w - 50){
			dx = -1;
		}
		if(y >= h - 50){
			dy = -1;
		}

	  memset(pixels, 0, w * h * sizeof(double) * 3);
		double white[3] = {1, 1, 1};
		DrawFilledRectangle(pixels, x, y, 50, 50, w, white);

		Display(0, pixels);
		WaitForDraw(0);
	}
}
















