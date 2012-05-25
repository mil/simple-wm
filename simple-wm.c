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
	int x;
	int y;
} PointerMotion;
PointerMotion drag;


/* ---------------------------
 * Status Window Related
 * --------------------------- */
void createStatusWindow() {
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
}

void logMessage(char message[]) {
	XClearWindow(display, statusWindow);
	XDrawString(
			display,
			statusWindow,
			DefaultGC(display, activeScreen),
			15, 15,
			message, strlen(message)
			);
}



/* ----------------------------------
 * Window Manipulation Initializaiton 
 * ---------------------------------- */
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



/* ---------------------------
 * Window Manipulation Related
 * --------------------------- */
void applyBorder(Window *window) {
	XSetWindowBorderWidth(display,*window,20);
	XSetWindowBorder(display, *window, 20);
}

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
//Raises Window, Focuses, Makes Window the Active Window
void raiseWindow(Window *window){
	XRaiseWindow(display, *window);
	activeWindow = *window;
}



/* ---------------------------
 * Event Handlers
 * --------------------------- */
void hMapRequest(XEvent *event) {
	logMessage("Map Request");

	//Map the Window
	Window mapRequestWindow = event -> xmaprequest.window;
	XMapWindow(display, mapRequestWindow);

	//Window Fns: Raise, Border, Center Pointer, Setup Events
	raiseWindow(&mapRequestWindow);
	applyBorder(&mapRequestWindow);
	centerPointer(&mapRequestWindow);

	/*applyBorder();
		centerPointer(window);
		XSelectInput(display, *window, 
		FocusChangeMask | KeyPressMask | ButtonPressMask 
		);
		*/
}

//Handles Keypress, takes in modifier and keycode
void hKeyPress(XEvent *event) {

	/*
		 keyPress(event.xkey.state, event.xkey.keycode); 


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
		 */
}

void hButtonPress(XEvent *event) {

	//void buttonPress(int button, int type, int x, int y, Window *window) {
	/*
		 buttonPress(
		 event.xbutton.button, 
		 event.type,
		 event.xbutton.x_root, event.xbutton.y_root, 
		 &event.xbutton.subwindow
		 );		

	//Left Click -- Click to Focus
	if (button == 1) {
	if (*window) {
	//	logMessage(&statusWindow, "Clicking to Focus");
	raiseWindow(window); //Or &*, just passes the pointer
	} else {
	//Click first button on root window
	}
	}

	if (*window && drag.button != None  && type == ButtonRelease ) {
	logMessage(&statusWindow, "DRAGOR");

	if (button == 1) {
	XMoveWindow(
	display, *window,
	(drag.attributes.x) + (drag.button -> x_root - x),
	(drag.attributes.y) + (drag.button -> y_root - y)
	);
	}
	}

*/

}

//void motionInWindow(Window *window, XButtonEvent *button) {
void hMotionNotify(XEvent *event) {	
	/*
		 motionInWindow(&event.xbutton.subwindow, &event.xbutton);
		 logMessage(&statusWindow, "Motion Notify");

		 if (*window) {
		 drag.window = *window;
		 drag.button = button;
		 XGetWindowAttributes(display, *window, &drag.attributes);
		 }

*/
}

void hFocusIn(XEvent *event) {
	/*
		 logMessage(&statusWindow, "Focus In");
		 raiseWindow(&event.xfocus.window);
		 */
}

void hFocusOut(XEvent *event) {
	// logMessage(&statusWindow, "Focus Out");
}

void hPropertyNotify(XEvent *event) {

}


void handleEvent() {
	//Waits for the Next Event (Blocking)
	XNextEvent(display, &event);

	switch (event.type) {
		case KeyPress:       hKeyPress(&event);       break;
		case ButtonPress:    hButtonPress(&event);    break;
		case MotionNotify:   hMotionNotify(&event);   break;
		case CreateNotify:   logMessage("Create Notify"); break;
		case MapRequest:     hMapRequest(&event);     break; 
		case FocusIn:        hFocusIn(&event);        break;
		case FocusOut:       hFocusOut(&event);       break;
		case PropertyNotify: hPropertyNotify(&event); break;
		default: break;
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
	createStatusWindow();
	setCursor(&statusWindow, 56);

	while (True) { handleEvent(); }

	//If Event Loop Were to Break
	XCloseDisplay(display);
	return 0;
}
