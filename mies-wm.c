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
void displayMessage(Window *window, char message[]) {
	XDrawString(
		display,
		*window,
		DefaultGC(display, activeScreen),
		50, 50,
		message, strlen(message)
	);
}

//Creates and maps window
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

//Just a test of Xwarppointer
void warpPointer(Window *window) {
	XWarpPointer(display, None, *window, 10, 10, 20, 0, 0,0);
}

//Sets up events for given window
void setupEvents(Window *window) {
    XSelectInput(display, *window,
		KeyPressMask ||
		ButtonPressMask ||
		PointerMotionMask ||
		
		//For Pointer Entry / Exit
		//Generates events: EnterNotify, LeaveNotify
		EnterWindowMask ||
		LeaveWindowMask ||

		//Alerts of motion notify 
		//ONLY within the window itself(start->end)
		MotionNotify 
	);
}

//Raises window above all
void raiseWindow(Window *window){
	XRaiseWindow(display, *window);
}
/*
void killWindow(Window *window) {
	XEvent kill;
	kill.type =  ClientMessage;
	kill.xclient = *window;

	XSendEvent(display, *window, False, NoEventMask, &kill);
}
*/

void handleEvent() {
	//Waits for the Next Event (Blocking)
	XNextEvent(display, &event);

	switch (event.type) {

		case KeyPress:
			raiseWindow(&window);
			displayMessage(&window, "mmmm wm");	

			expandBorder(&window);
			warpPointer(&window);
		break;

		case ButtonPress:
		break;


		case EnterNotify:
			displayMessage(&window, "EnterNotify");
			setCursor(&window, 52);
		break;

		case LeaveNotify:
			displayMessage(&window, "LeaveNotify");
			setCursor(&window, 52);
		break;

		case MotionNotify:
			displayMessage(&window, "MotionNotify");
		break;

		case FocusIn:
			displayMessage(&window, "FocusIn");
		break;


		case FocusOut:
			displayMessage(&window, "FocusOut");
		break;

		default:
		break;
	}
}


int main() {
	//Open Display and Assert
	display = XOpenDisplay(NIL);
	assert(display);

	//Setup the Root Window, Active Screen, and Events
	root = RootWindow(display, activeScreen);
	activeScreen = DefaultScreen(display);
	setupEvents(&root);

	//Create a Window and Setup Events for the Window
	//Set Cursor for window only to XC_gumby
	window = createWindow();
	setCursor(&window, 56);

	while (True) {
		//Enter the Event Loop
		handleEvent();
	}

	//If Event Loop Were to Break
	XCloseDisplay(display);
	return 0;
}
