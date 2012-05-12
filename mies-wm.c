// mies-wm
#include <assert.h>
#include <string.h>
#include <X11/Xlib.h>

#define NIL (0)

int screen;
int	activeScreen;
Display	*display;
Window window; 
XEvent event;
Cursor cursor;

//Display message 
void displayMessage() {
	XDrawString(
		display,
		window,
		DefaultGC(display, activeScreen),
		50, 50,
		"mmm wm", strlen("mmm wm")
	);
}

//Creates a window
int createWindow() {
	window = XCreateSimpleWindow(
		display, 
		RootWindow(display, activeScreen), 
		10, 10, 
		100, 200, 1,
		BlackPixel(display, activeScreen), 
		WhitePixel(display, activeScreen)
	);	
    XMapWindow(display, window);

	return window;
}

//Centers the pointer in the middle of the current window
void setBorder(Window *window) {
	XSetWindowBorderWidth(display,*window,1);
	XSetWindowBorder(display, *window, 20);
}

void setCursor(Window *window, int cursor) {
	//Create the Cursor and then Define Cursor for Window
	cursor = XCreateFontCursor(display, cursor);
	XDefineCursor(display, *window, cursor);
}

void setupEvents(Window *window) {
    XSelectInput(display, *window,
		KeyPressMask ||
		ButtonPressMask ||
		PointerMotionMask
	);
}

int main() {
	//Open Display and Assert
	display = XOpenDisplay(NIL);
	assert(display);

	//Get Active Screen
	activeScreen = DefaultScreen(display);

	//Create a Window and Setup Events for the Window
	window = createWindow();
	setupEvents(&window);

	//Set Cursor for window only to XC_gumby
	setCursor(&window, 56);

	for (;;) {
		XNextEvent(display, &event);
		if (event.type == KeyPress) {
			displayMessage(&window);	
		} else if (event.type == ButtonPress) {	
			setBorder(&window);
		} else {	

		}
	}
	return 0;
}
