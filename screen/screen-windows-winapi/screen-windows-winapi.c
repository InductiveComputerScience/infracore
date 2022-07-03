#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include <Screen.h>

uint32_t *pixels;

// Screen Config:
const int width = WWIDTH;
const int height = WHEIGHT;
const double density = 1920/0.508;
// --------------

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

HINSTANCE hInstance;

int ScreenPUProgram();

int WINAPI WinMain(HINSTANCE rInstance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow) {
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
}

// ----------------------------------------------------------------------------

struct {
		int width;
		int height;
		uint32_t *pixels;
} frame = {0};

LRESULT CALLBACK WindowProcessMessage(HWND, UINT, WPARAM, LPARAM);

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HDC frame_device_context = 0;

double now, last;

DWORD WINAPI ScreenDriverThread(LPVOID lpParam){
		const wchar_t window_class_name[] = L"My Window Class";
		static WNDCLASS window_class = { 0 };
		window_class.lpfnWndProc = WindowProcessMessage;
		window_class.hInstance = hInstance;
		window_class.lpszClassName = window_class_name;
		RegisterClass(&window_class);

		frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
		frame_bitmap_info.bmiHeader.biPlanes = 1;
		frame_bitmap_info.bmiHeader.biBitCount = 32;
		frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
		frame_device_context = CreateCompatibleDC(0);

		printf("%d x %d\n", width, height);

		RECT wr = {0, 0, width, height};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW|WS_VISIBLE, FALSE);

		static HWND window_handle;
		window_handle = CreateWindowEx(0, window_class_name, L"Windows API Virtual Screen", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			wr.right - wr.left,
			wr.bottom - wr.top,
			NULL, NULL, hInstance, NULL);

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
									 frame_device_context,
									 paint.rcPaint.left, paint.rcPaint.top,
									 SRCCOPY);
						EndPaint(window_handle, &paint);

						ReleaseSemaphore(semVSync, 1, NULL);
						
						now = xGetCurrentTime();
						while(now - last < 0.016){
							Sleep(1);
							now = xGetCurrentTime();
						}
						last = now;
				} break;

				case WM_SIZE: {
						if(!init){
							frame_bitmap_info.bmiHeader.biWidth	= LOWORD(lParam);
							frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

							if(frame_bitmap) DeleteObject(frame_bitmap);
							frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
							SelectObject(frame_device_context, frame_bitmap);

							frame.width =	LOWORD(lParam);
							frame.height = HIWORD(lParam);
							ReleaseSemaphore(semInit, 1, NULL);
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
			p = (height - y - 1) * width + x;
			frame.pixels[p] = r << 16 | g << 8 | b;
		}
	}
}


void WaitForDraw(int dev){
	WaitForSingleObject(semVSync, INFINITE);
}













