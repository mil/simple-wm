// mies-wm
#include <assert.h>
#include <string.h>
#include <X11/Xlib.h>

#define NIL (0)

int screen;
int	activeScreen;
Display	*display;
Window statusWindow, activeWindow, root; 
XEvent event;
Cursor cursor;


//Display message 
void displayMessage(Window *window, char message[]) {
	XClearWindow(display, *window);
	XDrawString(
		display,
		*window,
		DefaultGC(display, activeScreen),
		50, 50,
		message, strlen(message)
	);
}

//Creates and maps window
int createStatusWindow() {
	statusWindow = XCreateSimpleWindow(
		display, 
		RootWindow(display, activeScreen), 
		500, 500, 
		100, 200, 1,
		BlackPixel(display, activeScreen), 
		WhitePixel(display, activeScreen)
	);	
	XMapWindow(display, statusWindow);

	return statusWindow;
}

//Expands the border to be 20 pixels
void applyBorder(Window *window) {
	XSetWindowBorderWidth(display,*window,20);
	XSetWindowBorder(display, *window, 20);
}

//Sets given window with cursor
void setCursor(Window *window, int cursor) {
	//Create the Cursor and then Define Cursor for Window
	cursor = XCreateFontCursor(display, cursor);
	XDefineCursor(display, *window, cursor);
}

void centerPointer(Window *window) {
	//Get Window Attributes
	XWindowAttributes windowAttributes;
	XGetWindowAttributes(display, *window, &windowAttributes);

	int centerX = windowAttributes.width / 2,
		centerY = windowAttributes.height / 2;

	//Warp to Center
	XWarpPointer(display, None, *window, 0, 0, 0, 0, centerX,centerY);
}

//Sets up events for given window
void setupEvents() {
	XGrabButton(
		//Display, Button, Modifiers
		display, AnyButton, AnyModifier, 
		//Window, OwnerE?, EventMask
		root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, 
		//PointerMode, KBMode, Confine, Cursor
		GrabModeAsync, GrabModeAsync, None, None
	);

	//Gimme Some Events 
	XSelectInput(display, root, 
		SubstructureNotifyMask		| 
		SubstructureRedirectMask	| 
		KeyPressMask				|
		PropertyChangeMask			|
		ButtonPressMask				//Button Press on Root
	);
}

//Raises window above all
void raiseWindow(Window *window){
	XRaiseWindow(display, *window);
}

void mapWindow(Window *window) {
	XMapWindow(display, *window);
	applyBorder(window);
	centerPointer(window);
	XSelectInput(display, *window, 
		FocusChangeMask | KeyPressMask | 
		ButtonPressMask
	);
}

/*void moveWindow(Window *window, int xDifference, int yDifference) {
	XMoveResizeWindow(display,
}
*/

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

	XWindowAttributes attributes;
	int moveX = 0,
		moveY = 0;
	switch (event.type) {
		//raiseWindow(&window);
		//warpPointer(&window);
		//setCursor(&window, 52);
		//expandBorder(&window);
		//warpPointer(&window);
		case KeyPress:	
			if (activeWindow != NIL) {

				switch (event.xkey.keycode) {
					case 113:
						moveX = -10; //left
						break;
					case 114:
						moveX = 10; //right
						break;
					case 111:
						moveY = -10; //up
						break;
					case 116:
						moveY = 10; //down
						break;
				}

				XGetWindowAttributes(display, activeWindow, &attributes);
				XMoveWindow(display, activeWindow, attributes.x + moveX, attributes.y + moveY);

			}
			break;

		case ButtonPress:
			displayMessage(&statusWindow, "button press");
			break;

		case CreateNotify:
			displayMessage(&statusWindow, "Create Notify");
			break;

		case MapRequest:
			displayMessage(&statusWindow, "Map Request");
			XMapRequestEvent mapRequest = event.xmaprequest;
			mapWindow(&mapRequest.window);
			break;

		case FocusIn:
			displayMessage(&statusWindow, "Focus In");
			activeWindow = event.xfocus.window;

			break;

		case FocusOut:
			displayMessage(&statusWindow, "Focus Out");
			break;

		case PropertyNotify:
			displayMessage(&statusWindow, "property ntoify");
			break;

		default:
			//	displayMessage(&window, "Unknown Event");
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
	statusWindow = createStatusWindow();
	setCursor(&statusWindow, 56);

	while (True) {
		//Enter the Event Loop
		handleEvent();
	}

	//If Event Loop Were to Break
	XCloseDisplay(display);
	return 0;
}
