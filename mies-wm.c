// mies-wm
#include <assert.h>
#include <string.h>
#include <X11/Xlib.h>

#define NIL (0)

int screen;
int	activeScreen;
Display	*display;
Window window, root; 
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

//Expands the border to be 20 pixels
void expandBorder(Window *window) {
	XSetWindowBorderWidth(display,*window,20);
	XSetWindowBorder(display, *window, 20);
}

//Sets given window with cursor
void setCursor(Window *window, int cursor) {
	//Create the Cursor and then Define Cursor for Window
	cursor = XCreateFontCursor(display, cursor);
	XDefineCursor(display, *window, cursor);
}

//Sets up events for given window
void setupEvents(Window *window) {
    XSelectInput(display, *window,
		KeyPressMask ||
		ButtonPressMask ||
		PointerMotionMask
	);
}

//Raises window above all
void raiseWindow(Window *window){
	XRaiseWindow(display, *window);
}

int main() {
	//Open Display and Assert
	display = XOpenDisplay(NIL);
	assert(display);

	root = RootWindow(display, activeScreen);

	//Get Active Screen
	activeScreen = DefaultScreen(display);

	//Create a Window and Setup Events for the Window
	window = createWindow();
	setupEvents(&root);

	//Set Cursor for window only to XC_gumby
	setCursor(&window, 56);

	for (;;) {
		XNextEvent(display, &event);
		if (event.type == KeyPress) {
			raiseWindow(&window);
			displayMessage(&window);	
			expandBorder(&window);
		} else if (event.type == ButtonPress) {	
		} else {	

		}
	}
	return 0;
}
