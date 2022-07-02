#include <stdint.h>

// Screen Controller Instructions
void ScreenSize(int dev, double *width, double *height, double *density);
void Display(int dev, double *pixels);
void WaitForDraw(int dev);

