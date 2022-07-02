#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <semaphore.h>

#include <Screen.h>

bool isFullScreen = false;

void toggleFullScreen(SDL_Window* window, bool currentState){
  isFullScreen = !currentState;

  SDL_SetWindowFullscreen(window, !currentState);
  SDL_ShowCursor(currentState);
}

// Screen Config:
const int width = WWIDTH;
const int height = WHEIGHT;
const double density = 1920/0.508;
// --------------

uint32_t *pixels;

bool init = false;
sem_t semInit;
sem_t semDone;
sem_t semVSync;
bool quit = false;

void *ScreenDriverThread(void *vargp);

int FOUT();

int FIN(){
	sem_init(&semInit, 0, 0);
	sem_init(&semDone, 0, 0);
	sem_init(&semVSync, 0, 0);

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, ScreenDriverThread, NULL);

	sem_wait(&semInit);

	FOUT();

	quit = true;
	sem_wait(&semDone);
}

void *ScreenDriverThread(void *vargp){
  bool leftMouseButtonDown = false;
  SDL_Event event;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("SDL2 Virtual Screen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
  pixels = malloc(width * height * sizeof(uint32_t));
  memset(pixels, 255, width * height * sizeof(Uint32));

	int count = 0, count2 = 0;
	int last, now;
	bool currenState = false;

  while (!quit){
		SDL_PollEvent(&event);

		switch (event.type){
		  case SDL_QUIT:
		    quit = true;
		    break;
		}

		SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		if(!init){
			sem_post(&semInit);
			init = true;
		}
		sem_post(&semVSync);
  }

  free(pixels);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

	sem_post(&semDone);		

  return 0;
}

void ScreenSize(int dev, double *rwidth, double *rheight, double *rdensity){
	*rwidth = width;
	*rheight = height;
	*rdensity = density;
}

void Display(int dev, double *rpixels){
	int x, y, r, g, b, p;

	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			p = y * width + x;
			r = lround(rpixels[p*3 + 0] * 255);
			g = lround(rpixels[p*3 + 1] * 255);
			b = lround(rpixels[p*3 + 2] * 255);
			pixels[p] = r << 16 | g << 8 | b;
		}
	}
}


void WaitForDraw(int dev){
	sem_wait(&semVSync);
}

