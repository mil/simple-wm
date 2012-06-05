/* Simple WM */
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <X11/Xlib.h>

#include "structs.h"
#include "config.h"

#define NIL (0)
int screen;
int	activeScreen;
Display	*display;
Window statusWindow, root; 
XEvent event;
Cursor cursor;
PointerMotion origin;

Workspace workspaces[10];
int currentWorkspace = 0;
long focusedColor, unfocusedColor;


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

	origin.buttonEvent.subwindow = None;

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
void applyBorder(Window *window, long pixel) {
	//Set Width and Color
	XSetWindowBorderWidth(display, *window, 5);
	XSetWindowBorder(display, *window, pixel);
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

	int centerX = windowAttributes.width  / 2,
			centerY = windowAttributes.height / 2;

	//Warp to Center
	XWarpPointer(display, None, *window, 0, 0, 0, 0, centerX,centerY);
}
//Raises Window, Focuses, Makes Window the Active Window
void raiseWindow(Window *window){

	int i;
	for (i = 0; i <= workspaces[0].lastElement; i++) {
		applyBorder(&workspaces[0].windows[i], unfocusedColor);
	}

	XRaiseWindow(display, *window);
	applyBorder(window, focusedColor);
}

void dumpWorkspace(int wn) {
	int i;
	for (i = 0; i <= workspaces[wn].lastElement; i++) {
		fprintf(stderr, "Workspaces %d has Window %d\n", wn, &workspaces[wn].windows[i]);
	}
}

int changeWorkspace(int wn) {
	//Trying to change to the current workspace
	if (currentWorkspace == wn) { return False; }


	int a;
	for (a = 0; a <= workspaces[currentWorkspace].lastElement; a++) {
		XUnmapWindow(display, workspaces[currentWorkspace].windows[a]);
	}

	currentWorkspace = wn;

	int b;
	for (b = 0; b <= workspaces[wn].lastElement; b++) {
		XMapWindow(display, workspaces[currentWorkspace].windows[b]);
	}


	return True;
}

/* ---------------------------
 * Event Handlers
 * --------------------------- */
void hMapRequest(XEvent *event) {
	logMessage("Map Request");

	//Map the Window
	Window mapRequestWindow = event -> xmaprequest.window;
	XMapWindow(display, mapRequestWindow);

	workspaces[currentWorkspace].windows[workspaces[currentWorkspace].lastElement] = mapRequestWindow;
	workspaces[currentWorkspace].lastElement++;

	//Window Fns: Raise, Border, Center Pointer, Setup Events
	raiseWindow(&mapRequestWindow);
	applyBorder(&mapRequestWindow, unfocusedColor); 
	centerPointer(&mapRequestWindow);

	//centerPointer(window)
	XSelectInput(
			display, 
			event -> xmaprequest.window, 
			FocusChangeMask | KeyPressMask | ButtonPressMask | LeaveWindowMask
			);


}

void hConfigureRequest(XEvent *event) {
	fprintf(stderr, "Configure Request");
}
void hResizeRequest(XEvent *event) {
}

void hClientMessage(XEvent *event) {
	fprintf(stderr, "Client Message Recieved");
}


//Handles Keypress, takes in modifier and keycode
void hKeyPress(XEvent *event) {

	fprintf(stderr, "Got mod %d\n", event -> xkey.state);
	if (event -> xkey.state == Mod2Mask) { }

	//Need an active Window and the Shift Mod
	if (event -> xkey.state != ShiftMask) { return; }

	int moveX = 0, moveY = 0;
	switch (event -> xkey.keycode) {
		case 114: moveX =  MOVESTEP;      break; //Right
		case 116: moveY =  MOVESTEP;      break; //Down
		case 113: moveX = -1 * MOVESTEP;  break; //Left
		case 111: moveY = -1 * MOVESTEP;  break; //Up

		case 38: changeWorkspace(0); break; //a
		case 39: changeWorkspace(1); break; //s
	}

	XWindowAttributes attributes;
	XGetWindowAttributes(display, workspaces[currentWorkspace].active, &attributes);

	XMoveWindow(
			display,
			workspaces[currentWorkspace].active,
			attributes.x + moveX,
			attributes.y + moveY
			);
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
		origin.buttonEvent = event -> xbutton;
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
	char err[500];

	XGetErrorText(display, e -> request_code, err, 500);
	fprintf(stderr, "XErrorEvent of Request Code: %d and Error Code of %d\n", e -> request_code, e -> error_code);
	fprintf(stderr, "%s\n", err);
	return 0;
}

void hButtonRelease(XEvent *event) {
	origin.buttonEvent.subwindow = None;
}

void hMotionNotify(XEvent *event) {	

	if (origin.buttonEvent.subwindow == None) { return; }
	switch (origin.buttonEvent.button) {
		case 1:
			XMoveWindow(
					display,
					origin.buttonEvent.subwindow,
					origin.attributes.x + (event -> xbutton.x_root - origin.buttonEvent.x_root),
					origin.attributes.y + (event -> xbutton.y_root - origin.buttonEvent.y_root)
					);
			break;
		case 3:
			logMessage("Drag 3");
			/* Calculate Difference between current position original click */
			int xDifference = event -> xbutton.x_root - origin.buttonEvent.x_root;
			int yDifference = event -> xbutton.y_root - origin.buttonEvent.y_root;

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
			XMoveResizeWindow(display, origin.buttonEvent.subwindow,newX, newY, newWidth, newHeight);
			break;
	}

}

void hCreateNotify(XEvent *event) {
}

void hFocusIn(XEvent *event) {
	fprintf(stderr, "FOCUS IN\n");
	if (event -> xfocus.window != NIL) {
		workspaces[currentWorkspace].active = event -> xfocus.window;
	}
	//logMessage("Focus In");
}

void hFocusOut(XEvent *event) {
	logMessage("Focus Out\n");
	//applyBorder(&event -> xfocus.window, BlackPixel(display, activeScreen));
}

void hLeaveNotify(XEvent *event) {	
	//applyBorder(&event -> xcrossing.window, BlackPixel(display, activeScreen));
}

void hPropertyNotify(XEvent *event) {

}

//Thank you DWM ;)
unsigned long getColor(const char *colstr) {
	Colormap cmap = DefaultColormap(display, activeScreen);
	XColor color;

	if(!XAllocNamedColor(display, cmap, colstr, &color, &color)) { return 0; }
	return color.pixel;
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
		case ClientMessage:     hClientMessage(&event);    break;
		case FocusIn:           hFocusIn(&event);          break;
		case FocusOut:          hFocusOut(&event);         break;
		case LeaveNotify:       hLeaveNotify(&event);      break;
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
	setCursor(&root, 56);

	workspaces[0].lastElement = 0;

	unfocusedColor = getColor(UNFOCUSEDCOLOR);
	focusedColor   = getColor(FOCUSEDCOLOR);

	//Create a Window and Setup Events for the Window
	createStatusWindow();

	//Setup Error Handling
	XSetErrorHandler((XErrorHandler)(xError));

	//Main Event Loop
	while (True) { handleEvent(); }

	//If Event Loop Were to Break
	XCloseDisplay(display);
	return 0;
}
