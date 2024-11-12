#define WIN32_LEAN_AND_MEAN
#define UNICODE

#include <stdbool.h>
#define GetKeyboardState GetKeyboardStateX
#include <windows.h>
#undef GetKeyboardState

bool CreateScreenWinAPI(ScreenStructure **screen, HINSTANCE rInstance, int64_t w, int64_t h, double d);
bool CloseScreenWinAPI(ScreenStructure *screen);

bool CreateKeyboardWinAPI(KeyboardStructure **keyboard, ScreenStructure *screen);
bool CloseKeyboardWinAPI(KeyboardStructure *keyboard);
