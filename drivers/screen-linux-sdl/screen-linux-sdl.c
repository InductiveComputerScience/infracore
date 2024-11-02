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
#include <Keyboard.h>

bool isFullScreen = false;

void toggleFullScreen(SDL_Window* window, bool currentState){
  isFullScreen = !currentState;

  SDL_SetWindowFullscreen(window, !currentState);
  SDL_ShowCursor(currentState);
}

struct ScreenStructureLinuxSDL{
	uint32_t *pixelsStart;
	uint32_t *pixels;
	sem_t semVSync, initDone;
	bool initSuccess;
	bool quit;
	pthread_t thread_id;
	int width;
	int height;
	double density;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	KeyboardStructure *keyboard;
};

typedef struct ScreenStructureLinuxSDL ScreenStructureLinuxSDL;

struct KeyboardStructureLinuxSDL{
	double characters;
	double controlKeys;
	int historyMaxLength;
	double *history;
	bool *state;
	int pos;
};

typedef struct KeyboardStructureLinuxSDL KeyboardStructureLinuxSDL;

void AddKeyboardEvent(KeyboardStructureLinuxSDL *keyboardS, int code, bool state){
	//printf("Adding event %d, %d\n", code, state);
	if(keyboardS->pos < keyboardS->historyMaxLength){
		keyboardS->history[keyboardS->pos] = code;
		keyboardS->state[keyboardS->pos] = state;
		keyboardS->pos++;
	}
}

bool CreateKeyboardLinuxSDL(KeyboardStructure **keyboard, ScreenStructure *screen){
	ScreenStructureLinuxSDL *screenS;

	screenS = (ScreenStructureLinuxSDL*)screen->p;

	*keyboard = screenS->keyboard;
}

bool CreateKeyboardLinuxSDLInner(KeyboardStructure **keyboard){
	double i;
	KeyboardStructureLinuxSDL *keyboardS;

	*keyboard = malloc(sizeof(KeyboardStructure));
	keyboardS = malloc(sizeof(KeyboardStructureLinuxSDL));
	(*keyboard)->p = keyboardS;

	keyboardS->characters = 128;
	keyboardS->controlKeys = 47;
	keyboardS->historyMaxLength = 20;

	keyboardS->history = malloc((keyboardS->historyMaxLength)*sizeof(double));
	keyboardS->state = malloc((keyboardS->historyMaxLength)*sizeof(bool));
	for(i = 0; i < keyboardS->historyMaxLength; i = i + 1){
		keyboardS->history[(int)i] = 0;
		keyboardS->state[(int)i] = false;
	}
	keyboardS->pos = 0;

	return true;
}

bool CloseKeyboardLinuxSDL(KeyboardStructure *keyboard){
	KeyboardStructureLinuxSDL *keyboardS;

	keyboardS = (KeyboardStructureLinuxSDL*)keyboard->p;

	free(keyboardS->history);
	free(keyboardS->state);
	free(keyboardS);
	free(keyboard);

	return true;
}

void *ScreenDriverThread(void *vargp);

bool CreateScreenLinuxSDL(ScreenStructure **screen, int64_t w, int64_t h, double density){
	ScreenStructureLinuxSDL *screenS;
	int ret;

	*screen = malloc(sizeof(ScreenStructure));
	screenS = malloc(sizeof(ScreenStructureLinuxSDL));
	(*screen)->p = screenS;
	
	sem_init(&screenS->initDone, 0, 0);
	
	sem_init(&screenS->semVSync, 0, 0);
	screenS->quit = false;
	screenS->width = w;
	screenS->height = h;
	screenS->density = density;
	screenS->initSuccess = false;

	pthread_create(&screenS->thread_id, NULL, ScreenDriverThread, screenS);
	
	//printf("Waiting for screen init\n");
	sem_wait(&screenS->initDone);
	//printf("Screen init done, returning\n");

	return screenS->initSuccess;
}

bool CloseScreenLinuxSDL(ScreenStructure *screen){
	ScreenStructureLinuxSDL *screenS;

	screenS = (ScreenStructureLinuxSDL*)screen->p;

	screenS->quit = true;
	
	//printf("waiting to quit\n");
	pthread_join(screenS->thread_id, NULL);
	//printf("done quiting\n");

	free(screenS);
	free(screen);

	return true;
}

void *ScreenDriverThread(void *vargp){
	int ret;
	bool success;
	ScreenStructureLinuxSDL *screenS = (ScreenStructureLinuxSDL*)vargp;
	
	// Init
  ret = SDL_Init(SDL_INIT_VIDEO);
	if(ret == 0){
		screenS->window = SDL_CreateWindow("SDL2 Virtual Screen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenS->width, screenS->height, 0);
		
		if(screenS->window != NULL){
			screenS->renderer = SDL_CreateRenderer(screenS->window, -1, SDL_RENDERER_PRESENTVSYNC);
			if(screenS->renderer != NULL){
				screenS->texture = SDL_CreateTexture(screenS->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenS->width, screenS->height);
				if(screenS->texture != NULL){
					screenS->pixelsStart = malloc(screenS->width * screenS->height * sizeof(uint32_t));
					screenS->pixels = screenS->pixelsStart;
					memset(screenS->pixels, 0, screenS->width * screenS->height * sizeof(Uint32));

					CreateKeyboardLinuxSDLInner(&screenS->keyboard);
					
					success = true;
				}else{
					success = false;
					printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
				}
			}else{
				success = false;
				printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
			}
		}else{
			success = false;
			printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
		}
	}else{
		success = false;
		printf("SDL_Init failed: %d, %s\n", ret, SDL_GetError());
	}
	
	// Init done
	screenS->initSuccess = success;
	sem_post(&screenS->initDone);
	//printf("Screen init done: %d\n", success);
	
	if(success){
		KeyboardStructureLinuxSDL *keyboardS = (KeyboardStructureLinuxSDL *)screenS->keyboard->p;

		bool leftMouseButtonDown = false;
		SDL_Event event;

		int count = 0, count2 = 0;
		int last, now, hadEvent;
		bool currenState = false;

		while (!screenS->quit){
			while(hadEvent = SDL_PollEvent(&event)){
				if(event.key.repeat==0){

					switch (event.type){
						case SDL_KEYDOWN:

							// 1-4 - Left, Right, Up, Down
							if(event.key.keysym.sym == SDLK_UP) {
								AddKeyboardEvent(keyboardS, -1, true);
							}
							if(event.key.keysym.sym == SDLK_DOWN) {
								AddKeyboardEvent(keyboardS, -2, true);
							}
							if(event.key.keysym.sym == SDLK_LEFT) {
								AddKeyboardEvent(keyboardS, -3, true);
							}
							if(event.key.keysym.sym == SDLK_RIGHT) {
								AddKeyboardEvent(keyboardS, -4, true);
							}

							// 5-10 - LSHIFT, RSHIFT, ALT, ALTGR, LCTRL, RCTRL
							if(event.key.keysym.sym == SDLK_LSHIFT) {
								AddKeyboardEvent(keyboardS, -5, true);
							}
							if(event.key.keysym.sym == SDLK_RSHIFT) {
								AddKeyboardEvent(keyboardS, -6, true);
							}
							if(event.key.keysym.sym == SDLK_LALT) {
								AddKeyboardEvent(keyboardS, -7, true);
							}
							if(event.key.keysym.sym == SDLK_RALT) {
								AddKeyboardEvent(keyboardS, -8, true);
							}
							if(event.key.keysym.sym == SDLK_LCTRL) {
								AddKeyboardEvent(keyboardS, -9, true);
							}
							if(event.key.keysym.sym == SDLK_RCTRL) {
								AddKeyboardEvent(keyboardS, -10, true);
							}

							// 11-14 - LF, HT, CR, SP
							if(event.key.keysym.sym == SDLK_RETURN) {
								AddKeyboardEvent(keyboardS, -11, true);
							}
							if(event.key.keysym.sym == SDLK_TAB) {
								AddKeyboardEvent(keyboardS, -12, true);
							}
							/*if(event.key.keysym.sym == SDLK_LCTRL) {
								AddKeyboardEvent(keyboardS, -13, true);
							}*/
							/*if(event.key.keysym.sym == SDLK_RCTRL) {
								AddKeyboardEvent(keyboardS, -14, true);
							}*/

							// 15-38 - F1-F24
							if(event.key.keysym.sym == SDLK_F1) {
								AddKeyboardEvent(keyboardS, -15, true);
							}
							if(event.key.keysym.sym == SDLK_F2) {
								AddKeyboardEvent(keyboardS, -16, true);
							}
							if(event.key.keysym.sym == SDLK_F3) {
								AddKeyboardEvent(keyboardS, -17, true);
							}
							if(event.key.keysym.sym == SDLK_F4) {
								AddKeyboardEvent(keyboardS, -18, true);
							}
							if(event.key.keysym.sym == SDLK_F5) {
								AddKeyboardEvent(keyboardS, -19, true);
							}
							if(event.key.keysym.sym == SDLK_F6) {
								AddKeyboardEvent(keyboardS, -20, true);
							}
							if(event.key.keysym.sym == SDLK_F7) {
								AddKeyboardEvent(keyboardS, -21, true);
							}
							if(event.key.keysym.sym == SDLK_F8) {
								AddKeyboardEvent(keyboardS, -22, true);
							}
							if(event.key.keysym.sym == SDLK_F9) {
								AddKeyboardEvent(keyboardS, -23, true);
							}
							if(event.key.keysym.sym == SDLK_F10) {
								AddKeyboardEvent(keyboardS, -24, true);
							}
							if(event.key.keysym.sym == SDLK_F11) {
								AddKeyboardEvent(keyboardS, -25, true);
							}
							if(event.key.keysym.sym == SDLK_F12) {
								AddKeyboardEvent(keyboardS, -26, true);
							}
							if(event.key.keysym.sym == SDLK_F13) {
								AddKeyboardEvent(keyboardS, -27, true);
							}
							if(event.key.keysym.sym == SDLK_F14) {
								AddKeyboardEvent(keyboardS, -28, true);
							}
							if(event.key.keysym.sym == SDLK_F15) {
								AddKeyboardEvent(keyboardS, -29, true);
							}
							if(event.key.keysym.sym == SDLK_F16) {
								AddKeyboardEvent(keyboardS, -30, true);
							}
							if(event.key.keysym.sym == SDLK_F17) {
								AddKeyboardEvent(keyboardS, -31, true);
							}
							if(event.key.keysym.sym == SDLK_F18) {
								AddKeyboardEvent(keyboardS, -32, true);
							}
							if(event.key.keysym.sym == SDLK_F19) {
								AddKeyboardEvent(keyboardS, -33, true);
							}
							if(event.key.keysym.sym == SDLK_F20) {
								AddKeyboardEvent(keyboardS, -34, true);
							}
							if(event.key.keysym.sym == SDLK_F21) {
								AddKeyboardEvent(keyboardS, -35, true);
							}
							if(event.key.keysym.sym == SDLK_F22) {
								AddKeyboardEvent(keyboardS, -36, true);
							}
							if(event.key.keysym.sym == SDLK_F23) {
								AddKeyboardEvent(keyboardS, -37, true);
							}
							if(event.key.keysym.sym == SDLK_F24) {
								AddKeyboardEvent(keyboardS, -38, true);
							}

							// 39-47 - Home, PGUP, PGDN, END, ESC, DEL, INS, PRTSCR, BACKSP

							if(event.key.keysym.sym == SDLK_HOME) {
								AddKeyboardEvent(keyboardS, -39, true);
							}
							if(event.key.keysym.sym == SDLK_PAGEUP) {
								AddKeyboardEvent(keyboardS, -40, true);
							}
							if(event.key.keysym.sym == SDLK_PAGEDOWN) {
								AddKeyboardEvent(keyboardS, -41, true);
							}
							if(event.key.keysym.sym == SDLK_END) {
								AddKeyboardEvent(keyboardS, -42, true);
							}
							if(event.key.keysym.sym == SDLK_ESCAPE) {
								AddKeyboardEvent(keyboardS, -43, true);
							}
							if(event.key.keysym.sym == SDLK_DELETE) {
								AddKeyboardEvent(keyboardS, -44, true);
							}
							if(event.key.keysym.sym == SDLK_INSERT) {
								AddKeyboardEvent(keyboardS, -45, true);
							}
							if(event.key.keysym.sym == SDLK_PRINTSCREEN) {
								AddKeyboardEvent(keyboardS, -46, true);
							}
							if(event.key.keysym.sym == SDLK_BACKSPACE) {
								AddKeyboardEvent(keyboardS, -47, true);
							}

							// A-Z, a-z -- 65-90, 97-122 -- 52 keys
							if(event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
								if(event.key.keysym.mod & KMOD_SHIFT) {
									AddKeyboardEvent(keyboardS, event.key.keysym.sym+'A'-SDLK_a, true);
								}else{
									AddKeyboardEvent(keyboardS, event.key.keysym.sym+'a'-SDLK_a, true);
								}
							}

							// Symbols
							if(event.key.keysym.sym == SDLK_PLUS) {
								AddKeyboardEvent(keyboardS, '+', true);
							}
							if(event.key.keysym.sym == SDLK_MINUS) {
								AddKeyboardEvent(keyboardS, '-', true);
							}
							if(event.key.keysym.sym == SDLK_SPACE) {
								AddKeyboardEvent(keyboardS, ' ', true);
							}
							if(event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
								AddKeyboardEvent(keyboardS, event.key.keysym.sym+'0'-SDLK_0, true);
							}
							
							// Virtual screen control: Full Screen
							if(event.key.keysym.mod & KMOD_ALT) {
								if(event.key.keysym.sym == SDLK_RETURN) {
									toggleFullScreen(screenS->window, currenState);
									currenState = !currenState;
								}
							}

							/*if(event.key.keysym.mod & KMOD_SHIFT) {
								printf("<shift> down\n");
							}
							if(event.key.keysym.mod & KMOD_ALT) {
								printf("<alt> down\n");
							}*/

							break;

						case SDL_KEYUP:

							// 1-4 - Left, Right, Up, Down
							if(event.key.keysym.sym == SDLK_UP) {
								AddKeyboardEvent(keyboardS, -1, false);
							}
							if(event.key.keysym.sym == SDLK_DOWN) {
								AddKeyboardEvent(keyboardS, -2, false);
							}
							if(event.key.keysym.sym == SDLK_LEFT) {
								AddKeyboardEvent(keyboardS, -3, false);
							}
							if(event.key.keysym.sym == SDLK_RIGHT) {
								AddKeyboardEvent(keyboardS, -4, false);
							}

							// 5-10 - LSHIFT, RSHIFT, ALT, ALTGR, LCTRL, RCTRL
							if(event.key.keysym.sym == SDLK_LSHIFT) {
								AddKeyboardEvent(keyboardS, -5, false);
							}
							if(event.key.keysym.sym == SDLK_RSHIFT) {
								AddKeyboardEvent(keyboardS, -6, false);
							}
							if(event.key.keysym.sym == SDLK_LALT) {
								AddKeyboardEvent(keyboardS, -7, false);
							}
							if(event.key.keysym.sym == SDLK_RALT) {
								AddKeyboardEvent(keyboardS, -8, false);
							}
							if(event.key.keysym.sym == SDLK_LCTRL) {
								AddKeyboardEvent(keyboardS, -9, false);
							}
							if(event.key.keysym.sym == SDLK_RCTRL) {
								AddKeyboardEvent(keyboardS, -10, false);
							}

							// 11-14 - LF, HT, CR, SP
							if(event.key.keysym.sym == SDLK_RETURN) {
								AddKeyboardEvent(keyboardS, -11, false);
							}
							if(event.key.keysym.sym == SDLK_TAB) {
								AddKeyboardEvent(keyboardS, -12, false);
							}
							/*if(event.key.keysym.sym == SDLK_LCTRL) {
								AddKeyboardEvent(keyboardS, -13, false);
							}*/
							/*if(event.key.keysym.sym == SDLK_RCTRL) {
								AddKeyboardEvent(keyboardS, -14, false);
							}*/

							// 15-38 - F1-F24
							if(event.key.keysym.sym == SDLK_F1) {
								AddKeyboardEvent(keyboardS, -15, false);
							}
							if(event.key.keysym.sym == SDLK_F2) {
								AddKeyboardEvent(keyboardS, -16, false);
							}
							if(event.key.keysym.sym == SDLK_F3) {
								AddKeyboardEvent(keyboardS, -17, false);
							}
							if(event.key.keysym.sym == SDLK_F4) {
								AddKeyboardEvent(keyboardS, -18, false);
							}
							if(event.key.keysym.sym == SDLK_F5) {
								AddKeyboardEvent(keyboardS, -19, false);
							}
							if(event.key.keysym.sym == SDLK_F6) {
								AddKeyboardEvent(keyboardS, -20, false);
							}
							if(event.key.keysym.sym == SDLK_F7) {
								AddKeyboardEvent(keyboardS, -21, false);
							}
							if(event.key.keysym.sym == SDLK_F8) {
								AddKeyboardEvent(keyboardS, -22, false);
							}
							if(event.key.keysym.sym == SDLK_F9) {
								AddKeyboardEvent(keyboardS, -23, false);
							}
							if(event.key.keysym.sym == SDLK_F10) {
								AddKeyboardEvent(keyboardS, -24, false);
							}
							if(event.key.keysym.sym == SDLK_F11) {
								AddKeyboardEvent(keyboardS, -25, false);
							}
							if(event.key.keysym.sym == SDLK_F12) {
								AddKeyboardEvent(keyboardS, -26, false);
							}
							if(event.key.keysym.sym == SDLK_F13) {
								AddKeyboardEvent(keyboardS, -27, false);
							}
							if(event.key.keysym.sym == SDLK_F14) {
								AddKeyboardEvent(keyboardS, -28, false);
							}
							if(event.key.keysym.sym == SDLK_F15) {
								AddKeyboardEvent(keyboardS, -29, false);
							}
							if(event.key.keysym.sym == SDLK_F16) {
								AddKeyboardEvent(keyboardS, -30, false);
							}
							if(event.key.keysym.sym == SDLK_F17) {
								AddKeyboardEvent(keyboardS, -31, false);
							}
							if(event.key.keysym.sym == SDLK_F18) {
								AddKeyboardEvent(keyboardS, -32, false);
							}
							if(event.key.keysym.sym == SDLK_F19) {
								AddKeyboardEvent(keyboardS, -33, false);
							}
							if(event.key.keysym.sym == SDLK_F20) {
								AddKeyboardEvent(keyboardS, -34, false);
							}
							if(event.key.keysym.sym == SDLK_F21) {
								AddKeyboardEvent(keyboardS, -35, false);
							}
							if(event.key.keysym.sym == SDLK_F22) {
								AddKeyboardEvent(keyboardS, -36, false);
							}
							if(event.key.keysym.sym == SDLK_F23) {
								AddKeyboardEvent(keyboardS, -37, false);
							}
							if(event.key.keysym.sym == SDLK_F24) {
								AddKeyboardEvent(keyboardS, -38, false);
							}

							// 39-47 - Home, PGUP, PGDN, END, ESC, DEL, INS, PRTSCR, BACKSP

							if(event.key.keysym.sym == SDLK_HOME) {
								AddKeyboardEvent(keyboardS, -39, false);
							}
							if(event.key.keysym.sym == SDLK_PAGEUP) {
								AddKeyboardEvent(keyboardS, -40, false);
							}
							if(event.key.keysym.sym == SDLK_PAGEDOWN) {
								AddKeyboardEvent(keyboardS, -41, false);
							}
							if(event.key.keysym.sym == SDLK_END) {
								AddKeyboardEvent(keyboardS, -42, false);
							}
							if(event.key.keysym.sym == SDLK_ESCAPE) {
								AddKeyboardEvent(keyboardS, -43, false);
							}
							if(event.key.keysym.sym == SDLK_DELETE) {
								AddKeyboardEvent(keyboardS, -44, false);
							}
							if(event.key.keysym.sym == SDLK_INSERT) {
								AddKeyboardEvent(keyboardS, -45, false);
							}
							if(event.key.keysym.sym == SDLK_PRINTSCREEN) {
								AddKeyboardEvent(keyboardS, -46, false);
							}
							if(event.key.keysym.sym == SDLK_BACKSPACE) {
								AddKeyboardEvent(keyboardS, -47, false);
							}

							// A-Z, a-z -- 65-90, 97-122 -- 52 keys
							if(event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') {
								if(event.key.keysym.mod & KMOD_SHIFT) {
									AddKeyboardEvent(keyboardS, event.key.keysym.sym+'A'-SDLK_a, false);
								}else{
									AddKeyboardEvent(keyboardS, event.key.keysym.sym+'a'-SDLK_a, false);
								}
							}

							// Symbols
							if(event.key.keysym.sym == SDLK_PLUS) {
								AddKeyboardEvent(keyboardS, '+', false);
							}
							if(event.key.keysym.sym == SDLK_MINUS) {
								AddKeyboardEvent(keyboardS, '-', false);
							}
							if(event.key.keysym.sym == SDLK_SPACE) {
								AddKeyboardEvent(keyboardS, ' ', false);
							}
							if(event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
								AddKeyboardEvent(keyboardS, event.key.keysym.sym+'0'-SDLK_0, false);
							}

							/*if(event.key.keysym.mod & KMOD_SHIFT) {
								printf("<shift> up\n");
							}
							if(event.key.keysym.mod & KMOD_ALT) {
								printf("<alt> up\n");
							}*/

							/*if(event.key.keysym.sym == SDLK_f) {
								if(event.key.keysym.mod & KMOD_SHIFT) {
									printf("F up\n");
								}else{
									printf("f up\n");
								}
							}*/

							break;
						case SDL_QUIT:
							break;
					}
				}
			}

			ret = SDL_UpdateTexture(screenS->texture, NULL, screenS->pixels, screenS->width * sizeof(uint32_t));
			if(ret == 0){
				ret = SDL_RenderClear(screenS->renderer);
				if(ret == 0){
					ret = SDL_RenderCopy(screenS->renderer, screenS->texture, NULL, NULL);
					if(ret == 0){
						SDL_RenderPresent(screenS->renderer);
					}else{
						success = false;
						printf("SDL_RenderCopy failed: %d, %s\n", ret, SDL_GetError());
					}
				}else{
					success = false;
					printf("SDL_RenderClear failed: %d, %s\n", ret, SDL_GetError());
				}
			}else{
				success = false;
				printf("SDL_UpdateTexture failed: %d, %s\n", ret, SDL_GetError());
			}

			sem_post(&screenS->semVSync);
			//static int i = 0;
			//printf("rendering %d\n", i++);
		}
  }
  
  //printf("Starting quiting process...\n");
  
  // Done
  free(screenS->pixelsStart);
  SDL_DestroyTexture(screenS->texture);
  SDL_DestroyRenderer(screenS->renderer);
  SDL_DestroyWindow(screenS->window);
  SDL_Quit();
  
  //printf("Done\n");

  return 0;
}

// ============================================================================

void ScreenSpecs(ScreenStructure *screen, double *rwidth, double *rheight, double *rdensity){
	ScreenStructureLinuxSDL *screenS;

	screenS = (ScreenStructureLinuxSDL*)screen->p;

	*rwidth = screenS->width;
	*rheight = screenS->height;
	*rdensity = screenS->density;
}

void DisplayProgsbase(ScreenStructure *screen, double *rpixels){
	int x, y, r, g, b, p;
	ScreenStructureLinuxSDL *screenS;

	screenS = (ScreenStructureLinuxSDL*)screen->p;

	for(y = 0; y < screenS->height; y++){
		for(x = 0; x < screenS->width; x++){
			p = y * screenS->width + x;
			r = lround(rpixels[p*3 + 0] * 255);
			g = lround(rpixels[p*3 + 1] * 255);
			b = lround(rpixels[p*3 + 2] * 255);
			screenS->pixels[p] = r << 16 | g << 8 | b;
		}
	}
}

void Display(ScreenStructure *screen, uint32_t *rpixels){
	int x, y, r, g, b, p;
	ScreenStructureLinuxSDL *screenS;

	screenS = (ScreenStructureLinuxSDL*)screen->p;

	screenS->pixels = rpixels;
}


void Synchronize(ScreenStructure *screen){
	ScreenStructureLinuxSDL *screenS;

	screenS = (ScreenStructureLinuxSDL*)screen->p;

	// At most 250 fps for those without vsync.
	usleep(4 * 1000);
	sem_wait(&screenS->semVSync);
}

// ============================================================================

void GetKeyboardSpecs(KeyboardStructure *keyboard, double *characters, double *controlKeys, double *historyMaxLength){
	KeyboardStructureLinuxSDL *keyboardS;

	keyboardS = (KeyboardStructureLinuxSDL*)keyboard->p;

	*characters = keyboardS->characters;
	*controlKeys = keyboardS->controlKeys;
	*historyMaxLength = keyboardS->historyMaxLength;
}

void GetKeyboardState(KeyboardStructure *keyboard, double *history, bool *state, double *length){
	int i;
	KeyboardStructureLinuxSDL *keyboardS;

	keyboardS = (KeyboardStructureLinuxSDL*)keyboard->p;

	*length = keyboardS->pos;

	for(i = 0; i < keyboardS->pos; i = i + 1){
		history[i] = keyboardS->history[i];
		state[i] = keyboardS->state[i];
	}

	keyboardS->pos = 0;
}

// ============================================================================


























