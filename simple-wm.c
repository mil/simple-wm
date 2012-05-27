// mies-wm
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <X11/Xlib.h>
#define NIL (0)

int screen;
int	activeScreen;
Display	*display;
Window statusWindow, activeWindow, root; 
XEvent event;
Cursor cursor;

//PointerMotion Struct Contains Original Button Event and Original Attributes
typedef struct {
	XButtonEvent       button;
	XWindowAttributes  attributes;
} PointerMotion;
PointerMotion origin;

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
void setupEvents() {

	origin.button.subwindow = None;

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
	XSetWindowBorderWidth(display,*window,5);
	XSetWindowBorder(display, *window, 5);
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

	//applyBorder();
	//centerPointer(window)
	XSelectInput(display, event -> xmaprequest.window, FocusChangeMask | KeyPressMask | ButtonPressMask );
}

void hConfigureRequest(XEvent *event) {

}
void hResizeRequest(XEvent *event) {
}


//Handles Keypress, takes in modifier and keycode
void hKeyPress(XEvent *event) {

	if (activeWindow == NIL) { return; }

	XWindowAttributes attributes;
	int moveX = 0, moveY = 0;

	if (event -> xkey.state == ShiftMask) {
		switch (event -> xkey.keycode) {
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
		XMoveWindow(
				display,
				activeWindow,
				attributes.x + moveX,
				attributes.y + moveY
				);
	}
}

void hButtonPress(XEvent *event) {
	//Clicking on the Root Window
	if (event -> xbutton.subwindow == None) { return; }

	logMessage("Clicking");

	// Shift Click to Move -- store into drag struct
	if (event -> xkey.state == ShiftMask) {
		//Warp pointer to corner if resizing
		if (event -> xbutton.button == 3) {
			/*
			XWindowAttributes windowAttributes;
			XGetWindowAttributes(display, event -> xbutton.subwindow, &windowAttributes);
			XWarpPointer(
					display,
					event -> xbutton.subwindow,
					event -> xbutton.subwindow,
					0, 0, 0, 0, windowAttributes.width, windowAttributes.height
					);
			*/
		}

		XGetWindowAttributes(display, 
				event -> xbutton.subwindow, 
				&(origin.attributes)
				);
		origin.button = event -> xbutton;
	}

		//Clicking on A Normal Window
		switch (event -> xbutton.button) {
			case 1:
				logMessage("1st Button");
				raiseWindow(&(event -> xbutton.subwindow));
				break;
			case 3:
				logMessage("3rd");
			default:
				break;
		}
}

int xError(XErrorEvent *e) {
	fprintf(stderr, "XErrorEvent of Request Code: %d and Error Code of %d\n", e -> request_code, e -> error_code);
	return 0;
}

void hButtonRelease(XEvent *event) {
	origin.button.subwindow = None;
}

void hMotionNotify(XEvent *event) {	

	if (origin.button.subwindow == None) { return; }
	switch (origin.button.button) {
		case 1:
			XMoveWindow(
					display,
					origin.button.subwindow,
					origin.attributes.x + (event -> xbutton.x_root - origin.button.x_root),
					origin.attributes.y + (event -> xbutton.y_root - origin.button.y_root)
					);
			break;
		case 3:
			logMessage("Drag 3");
			/* Calculate Difference between current position original click */
			int xDifference = event -> xbutton.x_root - origin.button.x_root;
			int yDifference = event -> xbutton.y_root - origin.button.y_root;

			/* Data for XMoveResize if contracting/expanding normally */
			int newX = origin.attributes.x;
			int newY = origin.attributes.y;
			int newWidth = origin.attributes.width + xDifference;
			int newHeight = origin.attributes.height + yDifference;

			/* Check if Drag is to the left or top of window, flip window */
			if (newWidth == 0) { newWidth = 1; } else if (newWidth < 1 ) {
				newX     = newX + xDifference + origin.attributes.width;
				newWidth = (xDifference * -1) - origin.attributes.width;
			}
			if (newHeight == 0) { newHeight = 1; } else if (newHeight < 1) {
				newY      = newY + yDifference + origin.attributes.height;
				newHeight = (yDifference * -1) - origin.attributes.height;
			}

			/* Fire to XMoveResizeWindow */
			XMoveResizeWindow(display, origin.button.subwindow,newX, newY, newWidth, newHeight);
			break;
	}

}

void hCreateNotify(XEvent *event) {
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
		case KeyPress:          hKeyPress(&event);         break;
		case ButtonPress:       hButtonPress(&event);      break;
		case ButtonRelease:     hButtonRelease(&event);    break;
		case MotionNotify:      hMotionNotify(&event);     break;
		case CreateNotify:      hCreateNotify(&event);     break;
		case MapRequest:        hMapRequest(&event);       break; 
		case ConfigureRequest:  hConfigureRequest(&event); break;
		case ResizeRequest:     hResizeRequest(&event);    break;
		case FocusIn:           hFocusIn(&event);          break;
		case FocusOut:          hFocusOut(&event);         break;
		case PropertyNotify:    hPropertyNotify(&event);   break;
		default:                                           break;
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
	setCursor(&statusWindow, 61);

	//Setup Error Handling
	XSetErrorHandler((XErrorHandler)(xError));


	//Main Event Loop
	while (True) { handleEvent(); }

	//If Event Loop Were to Break
	XCloseDisplay(display);
	return 0;
}
