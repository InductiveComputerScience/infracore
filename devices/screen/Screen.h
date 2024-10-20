#ifndef ScreenH
#define ScreenH

#include <stdint.h>

struct ScreenStructure{
	void *p;
};

typedef struct ScreenStructure ScreenStructure;

// Screen Controller Instructions
void ScreenSpecs(ScreenStructure *screen, double *width, double *height, double *density);
void Display(ScreenStructure *screen, double *pixels);
void Synchronize(ScreenStructure *screen);

void DisplayBytes(ScreenStructure *screen, uint32_t *rpixels);

#endif
