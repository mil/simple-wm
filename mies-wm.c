// mies-wm
#include <assert.h>
#include <string.h>
#include <X11/Xlib.h>

#define NIL (0)

//Screen and Display
int 			screen;
int				activeScreen;
Display			*display;

//Windows
Window 			window; 

//Event
XEvent			event;


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

void setupEvents(Window *window) {
    XSelectInput(display, *window, 
			KeyPressMask
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


	for (;;) {
		XNextEvent(display, &event);

		displayMessage(&window);	
		setBorder(&window);
	}

	return 0;
}
