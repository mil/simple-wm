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

//Resize Structure
typedef struct {
	Window				window;
	XButtonEvent 		*button;
	XWindowAttributes 	attributes;
} PointerMotion;
PointerMotion drag;


//Display message 
void displayMessage(Window *window, char message[]) {
	XClearWindow(display, *window);
	XDrawString(
			display,
			*window,
			DefaultGC(display, activeScreen),
			15, 15,
			message, strlen(message)
			);
}

//Creates and maps window
int createStatusWindow() {

	int width = DisplayWidth(display, activeScreen);
	int height = DisplayHeight(display, activeScreen);
	int infoBoxHeight = 20;

	statusWindow = XCreateSimpleWindow(
			display, RootWindow(display, activeScreen),  //Display, Parent
			0, (height - infoBoxHeight), //X, Y
			width, infoBoxHeight,  //Width, Height
			1, //Border Width
			BlackPixel(display, activeScreen), //Border
			WhitePixel(display, activeScreen) //Background
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
			FocusChangeMask | PropertyChangeMask |
			SubstructureNotifyMask | SubstructureRedirectMask | 
			KeyPressMask | ButtonPressMask
			);
}

//Raises Window, Focuses, Makes Window the Active Window
void raiseWindow(Window *window){
	XRaiseWindow(display, *window);
	activeWindow = *window;
}

void mapWindow(Window *window) {

	XMapWindow(display, *window);
	applyBorder(window);
	centerPointer(window);
	XSelectInput(display, *window, 
			FocusChangeMask | KeyPressMask | ButtonPressMask 
			);
}

//Handles Keypress, takes in modifier and keycode
void keyPress(int modifier, int keycode) {

	XWindowAttributes attributes;
	int moveX = 0, moveY = 0;

	if (activeWindow != NIL && modifier == (Mod1Mask||Mod2Mask||Mod3Mask||Mod4Mask)) {
		switch (keycode) {
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
		XMoveWindow(display, activeWindow, 
				attributes.x + moveX, 
				attributes.y + moveY
				);
	}
}

void buttonPress(int button, int x, int y, Window *window) {
	//Left Click -- Click to Focus
	if (button == 1) {
		if (*window) {
			//	displayMessage(&statusWindow, "Clicking to Focus");
			raiseWindow(window); //Or &*, just passes the pointer
		} else {
			//Click first button on root window
		}
	}

	if (*window && drag.button != None ) {
		displayMessage(&statusWindow, "DRAGOR");

		XMoveResizeWindow(
				display, *window,
				x + (x - drag.button -> x_root),
				y + (y - drag.button -> y_root),
				1,
				1
				);
	}

}

//Stores Dragging Motion in drag struct
void motionInWindow(Window *window, XButtonEvent *button) {

	if (*window) {
		drag.window = *window;
		drag.button = button;
		XGetWindowAttributes(display, *window, &drag.attributes);
	}
}

void handleEvent() {
	//Waits for the Next Event (Blocking)
	XNextEvent(display, &event);

	switch (event.type) {
		case KeyPress: 
			keyPress(event.xkey.state, event.xkey.keycode); 
			break;

		case ButtonPress:
			buttonPress(
					event.xbutton.button, 
					event.xbutton.x_root, event.xbutton.y_root, 
					&event.xbutton.subwindow
					);			
			break;

			// Motion within a window
		case MotionNotify:
			motionInWindow(&event.xbutton.subwindow, &event.xbutton);
			displayMessage(&statusWindow, "Motion Notify");
			break;

		case CreateNotify:
			displayMessage(&statusWindow, "Create Notify");
			break;

		case MapRequest:
			displayMessage(&statusWindow, "Map Request");
			XMapRequestEvent mapRequest = event.xmaprequest;
			mapWindow(&mapRequest.window);
			raiseWindow(&mapRequest.window);
			break;

		case FocusIn:
			displayMessage(&statusWindow, "Focus In");
			raiseWindow(&event.xfocus.window);

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
