#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define GetKeyboardState GetKeyboardStateX
#include <windows.h>
#undef GetKeyboardState
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <Screen.h>
#include <Keyboard.h>

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
  int width;
  int height;
  double density;
  HINSTANCE hInstance;
  KeyboardStructure *keyboard;

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

  WaitForSingleObject(screenS->semInit, INFINITE);
  //printf("initialized\n");

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

struct KeyboardStructureWinAPI{
  double characters;
  double controlKeys;
  int historyMaxLength;
  double *history;
  bool *state;
  int pos;
  bool shiftdown;
};

typedef struct KeyboardStructureWinAPI KeyboardStructureWinAPI;

void AddKeyboardEvent(KeyboardStructureWinAPI *keyboardS, int code, bool state){
  //printf("Adding event %d, %d\n", code, state);
  if(keyboardS->pos < keyboardS->historyMaxLength){
    keyboardS->history[keyboardS->pos] = code;
    keyboardS->state[keyboardS->pos] = state;
    keyboardS->pos++;
  }
}

bool CreateKeyboardWinAPI(KeyboardStructure **keyboard, ScreenStructure *screen){
  ScreenStructureWinAPI *screenS;

  screenS = (ScreenStructureWinAPI*)screen->p;

  *keyboard = screenS->keyboard;
}

bool CreateKeyboardWinAPIInner(KeyboardStructure **keyboard){
  double i;
  KeyboardStructureWinAPI *keyboardS;

  *keyboard = malloc(sizeof(KeyboardStructure));
  keyboardS = malloc(sizeof(KeyboardStructureWinAPI));
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

bool CloseKeyboardWinAPI(KeyboardStructure *keyboard){
}

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

    //printf("%d x %d\n", screenS->width, screenS->height);

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

    ReleaseSemaphore(screenS->semDone, 1, NULL);

    return 0;
}


LRESULT CALLBACK WindowProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
    ScreenStructureWinAPI *screenS = screenS_Global;
    KeyboardStructure *keyboard = screenS->keyboard;
    KeyboardStructureWinAPI *keyboardS;

    if(screenS->keyboard != NULL){
      keyboardS = (KeyboardStructureWinAPI*)keyboard->p;
    }

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

        case WM_SYSKEYDOWN: {
            printf("syskey %d\n", wParam);

          } break;

        case WM_SYSKEYUP: {

          } break;

        case WM_KEYDOWN:
            // 1-4 - Left, Right, Up, Down
            if(wParam == VK_UP){
              AddKeyboardEvent(keyboardS, -1, true);
            }
            if(wParam == VK_DOWN){
              AddKeyboardEvent(keyboardS, -2, true);
            }
            if(wParam == VK_LEFT){
              AddKeyboardEvent(keyboardS, -3, true);
            }
            if(wParam == VK_RIGHT){
              AddKeyboardEvent(keyboardS, -4, true);
            }
            if(wParam == VK_ESCAPE){
              AddKeyboardEvent(keyboardS, -43, true);
            }

            // 5-10 - LSHIFT, RSHIFT, ALT, ALTGR, LCTRL, RCTRL
            if(wParam == VK_SHIFT){
              keyboardS->shiftdown = true;
              //if(GetAsyncKeyState(VK_LSHIFT) & 0x8000){
                AddKeyboardEvent(keyboardS, -5, true);
              //}else if(GetAsyncKeyState(VK_RSHIFT) & 0x8000){
              //  AddKeyboardEvent(keyboardS, -6, true);
              //}
            }
            if(wParam == VK_LMENU){
              AddKeyboardEvent(keyboardS, -7, true);
            }
            if(wParam == VK_RMENU){
              AddKeyboardEvent(keyboardS, -8, true);
            }
            if(wParam == VK_LCONTROL){
              AddKeyboardEvent(keyboardS, -9, true);
            }
            if(wParam == VK_RCONTROL){
              AddKeyboardEvent(keyboardS, -10, true);
            }

            // A-Z, a-z -- 65-90, 97-122 -- 52 keys
            if(wParam >= 'A' && wParam <= 'Z') {
              //printf("Shift %d\n", keyboardS->shiftdown);
              if(keyboardS->shiftdown) {
                AddKeyboardEvent(keyboardS, wParam, true);
              }else{
                AddKeyboardEvent(keyboardS, wParam - 'A' + 'a', true);
              }
            }

            break;

        case WM_KEYUP:
            // 1-4 - Left, Right, Up, Down
            if(wParam == VK_UP){
              AddKeyboardEvent(keyboardS, -1, false);
            }
            if(wParam == VK_DOWN){
              AddKeyboardEvent(keyboardS, -2, false);
            }
            if(wParam == VK_LEFT){
              AddKeyboardEvent(keyboardS, -3, false);
            }
            if(wParam == VK_RIGHT){
              AddKeyboardEvent(keyboardS, -4, false);
            }
            if(wParam == VK_ESCAPE){
              AddKeyboardEvent(keyboardS, -43, false);
            }

            // 5-10 - LSHIFT, RSHIFT, ALT, ALTGR, LCTRL, RCTRL
            if(wParam == VK_SHIFT){
              keyboardS->shiftdown = false;
              //if(GetAsyncKeyState(VK_LSHIFT) & 0x8000){
                AddKeyboardEvent(keyboardS, -5, false);
              //}else if(GetAsyncKeyState(VK_RSHIFT) & 0x8000){
              //  AddKeyboardEvent(keyboardS, -6, false);
              //}
            }

            // A-Z, a-z -- 65-90, 97-122 -- 52 keys
            if(wParam >= 'A' && wParam <= 'Z') {
              if(keyboardS->shiftdown) {
                AddKeyboardEvent(keyboardS, wParam, false);
              }else{
                AddKeyboardEvent(keyboardS, wParam - 'A' + 'a', false);
              }
            }

            break;

        case WM_SIZE: {
            if(!init){
              screenS->frame_bitmap_info.bmiHeader.biWidth  = LOWORD(lParam);
              screenS->frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

              //printf("%d x %d\n", LOWORD(lParam), HIWORD(lParam));

              if(screenS->frame_bitmap) DeleteObject(screenS->frame_bitmap);
              screenS->frame_bitmap = CreateDIBSection(NULL, &screenS->frame_bitmap_info, DIB_RGB_COLORS, &screenS->frame.pixels, 0, 0);
              SelectObject(screenS->frame_device_context, screenS->frame_bitmap);

              screenS->frame.width =  LOWORD(lParam);
              screenS->frame.height = HIWORD(lParam);

              screenS->initSuccess = true;

              CreateKeyboardWinAPIInner(&screenS->keyboard);

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

  int x, y, r, g, b, p;
  uint32_t ap;

  int width = screenS->width;
  int height = screenS->height;

  for(y = 0; y < height; y++){
    for(x = 0; x < width; x++){
      p = y * width + x;
      ap = rpixels[p];
      p = (height - y - 1) * width + x;
      screenS->frame.pixels[p] = ap;
    }
  }
}

void Synchronize(ScreenStructure *screen){
  ScreenStructureWinAPI *screenS;
  screenS = (ScreenStructureWinAPI*)screen->p;

  WaitForSingleObject(screenS->semVSync, INFINITE);
}

// ============================================================================

void GetKeyboardSpecs(KeyboardStructure *keyboard, double *characters, double *controlKeys, double *historyMaxLength){
  KeyboardStructureWinAPI *keyboardS;

  keyboardS = (KeyboardStructureWinAPI*)keyboard->p;

  *characters = keyboardS->characters;
  *controlKeys = keyboardS->controlKeys;
  *historyMaxLength = keyboardS->historyMaxLength;
}

void GetKeyboardState(KeyboardStructure *keyboard, double *history, bool *state, double *length){
  int i;
  KeyboardStructureWinAPI *keyboardS;

  keyboardS = (KeyboardStructureWinAPI*)keyboard->p;

  *length = keyboardS->pos;

  for(i = 0; i < keyboardS->pos; i = i + 1){
    history[i] = keyboardS->history[i];
    state[i] = keyboardS->state[i];
  }

  keyboardS->pos = 0;
}











