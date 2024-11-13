#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

void draw(char *rgb_out, int w, int h){
	int i = 0;

	for (i = 0; i < w*h*4; i += 4) {
		rgb_out[i + 0] = 255; // B
		rgb_out[i + 1] = 255; // G
		rgb_out[i + 2] = 255; // R
		rgb_out[i + 3] = 0;
	}
}

XImage *create_ximage(Display *display, Visual *visual, int width, int height){
	char *image32 = (char *)malloc(width * height * 4);
	draw(image32, width, height);
	return XCreateImage(display, visual, 24, ZPixmap, 0, image32, width, height, 32, 0);
}

int main(int argc, char **argv){
	int win_b_color, win_w_color;
	XEvent xev;
	Window window;
	GC gc;
	Display *display = XOpenDisplay(NULL);
	Visual *visual;
	XImage *ximage;

	win_b_color = BlackPixel(display, DefaultScreen(display));
	win_w_color = BlackPixel(display, DefaultScreen(display));
	window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 640, 480, 0, win_b_color, win_w_color);
	
	XStoreName(display, window, "X11 Virtual Screen");

	gc = XCreateGC(display, window, 0, NULL);
	visual = DefaultVisual(display, 0);

	XMapWindow(display, window);
	XFlush(display);
	ximage = create_ximage(display, visual, 640, 480);
	while (1) {
		int r;

		r = XPutImage(display, window, gc, ximage, 0, 0, 0, 0, 640, 480);
		XSync(display, 1);
		XFlush(display);
		
		usleep(10* 16 * 1000);
	}

	return 0;
}
