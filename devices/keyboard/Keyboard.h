#ifndef KeyboardH
#define KeyboardH

#include <stdint.h>
#include <stdbool.h>

struct KeyboardStructure{
	void *p;
};

typedef struct KeyboardStructure KeyboardStructure;

// Keyboard Controller Instructions
void GetKeyboardSpecs(KeyboardStructure *keyboard, double *characters, double *controlKeys, double *historyMaxLength);
void GetKeyboardState(KeyboardStructure *keyboard, double *history, bool *state, double *length);

/*
 - Positive numbers are the unicode coint points.
 - The following numbers are negative for control keys.

 1-4 - Left, Right, Up, Down
 5-10 - LSHIFT, RSHIFT, ALT, ALTGR, LCTRL, RCTRL
 11-14 - LF, HT, CR, SP
 15-38 - F1-F24
 39-47 - Home, PGUP, PGDN, END, ESC, DEL, INS, PRTSCR, BACKSP
*/


/*
New Design:
 - GetKeyboardSpecs: 
 - GetKeyboardState: 

 * Instead of simultaneous, get history length
 * GetKeyboardState: Get history of key presses and releases.
*/

#endif
