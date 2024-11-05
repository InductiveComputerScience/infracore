#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <Screen.h>

DWORD WINAPI ScreenDriverThread(LPVOID lpParam);

struct ScreenStructureWinAPI{
  uint32_t *pixelsStart;
  uint32_t *pixels;
	HANDLE semInit;
	HANDLE initDone;
	HANDLE semDone;
	HANDLE semVSync;
  bool initSuccess;
  bool quit;
  //pthread_t thread_id;
  int width;
  int height;
  double density;
	HINSTANCE hInstance;
  //SDL_Window *window;
  //SDL_Renderer *renderer;
  //SDL_Texture *texture;
  //KeyboardStructure *keyboard;

	struct {
		  int width;
		  int height;
		  uint32_t *pixels;
	} frame; // = 0;

	BITMAPINFO frame_bitmap_info;
	HBITMAP frame_bitmap; // = 0;
	HDC frame_device_context; // = 0;

	DWORD thread_id;
};

typedef struct ScreenStructureWinAPI ScreenStructureWinAPI;

bool init = false;
bool quit = false;
ScreenStructureWinAPI *screenS_Global;

bool CreateScreenWinAPI(ScreenStructure **screen, HINSTANCE rInstance, int64_t w, int64_t h, double density){
  ScreenStructureWinAPI *screenS;
  int ret;

  *screen = malloc(sizeof(ScreenStructure));
  screenS = calloc(sizeof(ScreenStructureWinAPI), 1);
  (*screen)->p = screenS;

	screenS_Global = screenS;
  
  screenS->quit = false;
  screenS->width = w;
  screenS->height = h;
  screenS->density = density;
  screenS->initSuccess = false;

	screenS->hInstance = rInstance;

	screenS->semInit = CreateSemaphore(NULL, 0, 1, NULL);
	screenS->semDone = CreateSemaphore(NULL, 0, 1, NULL);
	screenS->semVSync = CreateSemaphore(NULL, 0, 1, NULL);

	CreateThread(NULL, 0, ScreenDriverThread, screenS, 0, &screenS->thread_id);

	WaitForSingleObject(&screenS->semInit, INFINITE);
	printf("initialized\n");
  
  //printf("Waiting for screen init\n");
  WaitForSingleObject(&screenS->initDone, INFINITE);
  //printf("Screen init done, returning\n");

  return screenS->initSuccess;
}

bool CloseScreenWinAPI(ScreenStructure *screen){
  ScreenStructureWinAPI *screenS;
  screenS = (ScreenStructureWinAPI*)screen->p;

	quit = true;
	WaitForSingleObject(screenS->semDone, INFINITE);
}

double xGetCurrentTime(){
	SYSTEMTIME systemTime;
	double s;

	GetSystemTime(&systemTime);

	s = systemTime.wDay * 24.0 * 60.0 * 60.0 +
			systemTime.wHour * 60.0 * 60.0 +
			systemTime.wMinute * 60.0 +
			systemTime.wSecond * 1.0 +
			systemTime.wMilliseconds / 1000.0;

	return s;
}



int ScreenPUProgram();

/*int WINAPI WinMain(HINSTANCE rInstance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow) {
	hInstance = rInstance;

	semInit = CreateSemaphore(NULL, 0, 1, NULL);
	semDone = CreateSemaphore(NULL, 0, 1, NULL);
	semVSync = CreateSemaphore(NULL, 0, 1, NULL);

	DWORD thread_id;
	CreateThread(NULL, 0, ScreenDriverThread, NULL, 0, &thread_id);

	WaitForSingleObject(semInit, INFINITE);
	printf("initialized\n");

	ScreenPUProgram();

	quit = true;
	WaitForSingleObject(semDone, INFINITE);

	return 0;
}*/

// ----------------------------------------------------------------------------

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);


double now, last;

DWORD WINAPI ScreenDriverThread(LPVOID lpParam){
	  ScreenStructureWinAPI *screenS = (ScreenStructureWinAPI*)lpParam;

    const wchar_t window_class_name[] = L"My Window Class";
    static WNDCLASS window_class = { 0 };
    window_class.lpfnWndProc = WindowProcessMessage;
    window_class.hInstance = screenS->hInstance;
    window_class.lpszClassName = window_class_name;
    RegisterClass(&window_class);

    screenS->frame_bitmap_info.bmiHeader.biSize = sizeof(screenS->frame_bitmap_info.bmiHeader);
    screenS->frame_bitmap_info.bmiHeader.biPlanes = 1;
    screenS->frame_bitmap_info.bmiHeader.biBitCount = 32;
    screenS->frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    screenS->frame_device_context = CreateCompatibleDC(0);

		printf("%d x %d\n", screenS->width, screenS->height);

		RECT wr = {0, 0, screenS->width, screenS->height};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW|WS_VISIBLE, FALSE);

    static HWND window_handle;
    window_handle = CreateWindowEx(0, window_class_name, L"Windows API Virtual Screen", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    	CW_USEDEFAULT, CW_USEDEFAULT,
      wr.right - wr.left,
      wr.bottom - wr.top,
			NULL, NULL, screenS->hInstance, NULL);

		now = xGetCurrentTime();
		last = xGetCurrentTime();

    while(!quit) {
        static MSG message = { 0 };
        while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
					DispatchMessage(&message);
				}

        InvalidateRect(window_handle, NULL, FALSE);
        UpdateWindow(window_handle);
    }

    return 0;
}


LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
	 	ScreenStructureWinAPI *screenS = screenS_Global;

    switch(message) {
        case WM_QUIT:
        case WM_DESTROY: {
            quit = true;
        } break;

        case WM_PAINT: {
            static PAINTSTRUCT paint;
            static HDC device_context;
            device_context = BeginPaint(window_handle, &paint);
            BitBlt(device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                   screenS->frame_device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   SRCCOPY);
            EndPaint(window_handle, &paint);

						ReleaseSemaphore(screenS->semVSync, 1, NULL);
						
						now = xGetCurrentTime();
						while(now - last < 0.016){
							Sleep(1);
							now = xGetCurrentTime();
						}
						last = now;
        } break;

        case WM_SIZE: {
						if(!init){
		          screenS->frame_bitmap_info.bmiHeader.biWidth  = LOWORD(lParam);
		          screenS->frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

							printf("%d x %d\n", LOWORD(lParam), HIWORD(lParam));

		          if(screenS->frame_bitmap) DeleteObject(screenS->frame_bitmap);
		          screenS->frame_bitmap = CreateDIBSection(NULL, &screenS->frame_bitmap_info, DIB_RGB_COLORS, &screenS->frame.pixels, 0, 0);
		          SelectObject(screenS->frame_device_context, screenS->frame_bitmap);

		          screenS->frame.width =  LOWORD(lParam);
		          screenS->frame.height = HIWORD(lParam);
							ReleaseSemaphore(screenS->semInit, 1, NULL);
							init = true;
						}
        } break;

        default: {
            return DefWindowProc(window_handle, message, wParam, lParam);
        }
    }
    return 0;
}
// ----------------------------------------------------------------------------

/*void ScreenSize(int dev, double *rwidth, double *rheight, double *rdensity){
	*rwidth = width;
	*rheight = height;
	*rdensity = density;
}

void DisplayX(int dev, double *rpixels){
	int x, y, r, g, b, p;

	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			p = y * width + x;
			r = lround(rpixels[p*3 + 0] * 255);
			g = lround(rpixels[p*3 + 1] * 255);
			b = lround(rpixels[p*3 + 2] * 255);
			p = (height - y - 1) * width + x;
			frame.pixels[p] = r << 16 | g << 8 | b;
		}
	}
}


void WaitForDraw(int dev){
	WaitForSingleObject(semVSync, INFINITE);
}*/

void ScreenSpecs(ScreenStructure *screen, double *width, double *height, double *density){
  ScreenStructureWinAPI *screenS;
  screenS = (ScreenStructureWinAPI*)screen->p;
  
  *width = screenS->width;
  *height = screenS->height;
  *density = screenS->density;
}

void Display(ScreenStructure *screen, uint32_t *rpixels){
  ScreenStructureWinAPI *screenS;
  screenS = (ScreenStructureWinAPI*)screen->p;

	screenS->frame.pixels = rpixels;
}

void Synchronize(ScreenStructure *screen){
  ScreenStructureWinAPI *screenS;
  screenS = (ScreenStructureWinAPI*)screen->p;

	WaitForSingleObject(screenS->semVSync, INFINITE);
}












