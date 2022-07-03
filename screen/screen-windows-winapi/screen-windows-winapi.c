#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sysinfoapi.h>
#include <time.h>
#include <math.h>

#include <Screen.h>

uint32_t *pixels;

// Screen Config:
const int width = WWIDTH;
const int height = WHEIGHT;
const double density = 1920/0.508;
// --------------

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
    switch(Message) {
        case WM_KEYDOWN: {
          switch(WParam) {
						case 'O': {
							DestroyWindow(Window);
						};
					}
					break;
        }
        case WM_DESTROY: {
					PostQuitMessage(0);
					break;
				}
        default: {
					return DefWindowProc(Window, Message, WParam,  LParam);
				}
    }
    return 0;
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

bool init = false;
HANDLE semInit;
HANDLE semDone;
HANDLE semVSync;
bool quit = false;

DWORD WINAPI ScreenDriverThread(LPVOID lpParam);

HINSTANCE Instance;

int ScreenPUProgram();

int WINAPI WinMain(HINSTANCE pInstance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow) {
	Instance = pInstance;

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
}

DWORD WINAPI ScreenDriverThread(LPVOID lpParam){
    WNDCLASS WindowClass = {};
    const wchar_t ClassName[] = L"MyWindowClass";
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = ClassName;
    WindowClass.hCursor = LoadCursor(0, IDC_CROSS);
		RegisterClass(&WindowClass);

		printf("%d x %d\n", width, height);

		RECT wr = {0, 0, width, height};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW|WS_VISIBLE, FALSE);
    
    HWND Window = CreateWindowEx(
			0, ClassName, L"Windows Virtual Screen",
			WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
      wr.right - wr.left,
      wr.bottom - wr.top,
			0, 0, Instance, 0
		);
    
    // Pixels
    pixels = VirtualAlloc(0,
			width * height * 4,
			MEM_RESERVE|MEM_COMMIT,
			PAGE_READWRITE
		);
 
    BITMAPINFO BitmapInfo;
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = width;
    BitmapInfo.bmiHeader.biHeight = -height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    HDC DeviceContext = GetDC(Window);
    
    bool Running = true;

		int count = 0;
		int prev, cur;
		double now, last;
		now = xGetCurrentTime();
		last = xGetCurrentTime();

		prev = time(NULL);
    
    while(Running) {
        MSG Message;
        while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            if(Message.message == WM_QUIT){
							Running = false;
						}
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

				// Wait
				while(now - last < 0.016){
					now = xGetCurrentTime();
					Sleep(2);
				}
				last = now;
        
				// Draw screen
        SetDIBitsToDevice(
					DeviceContext,
					0, 0,
					width, height,
					0, 0,
					0, height,
					pixels, &BitmapInfo,
					DIB_RGB_COLORS
				);

				count++;
				cur = time(NULL);
				if(prev != cur){
					printf("FPS: %d\n", count);
					count = 0;
					prev = cur;
				}

				if(!init){
					ReleaseSemaphore(semInit, 1, NULL);
					init = true;
				}
				ReleaseSemaphore(semVSync, 1, NULL);
    }
    
    return 0;
}

// ----------------------------------------------------------------------------

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
	WaitForSingleObject(semVSync, INFINITE);
}













