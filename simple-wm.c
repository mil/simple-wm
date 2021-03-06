/* Simple WM */
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
			16, 15,
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
			root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | OwnerGrabButtonMask, 
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
	Client *client;
	for (client=workspaces[currentWorkspace].last; client; client = client->previous) {
			applyBorder(&(client -> window), unfocusedColor);
	}


	XRaiseWindow(display, *window);
	applyBorder(window, focusedColor);

	//Focuses window
	XSelectInput(
			display, *window, 
			FocusChangeMask | KeyPressMask | ButtonPressMask | LeaveWindowMask | OwnerGrabButtonMask
			);
	XGrabButton(
			display, 
			AnyButton,
			AnyModifier,
			*window,
			False,
			OwnerGrabButtonMask | ButtonPressMask,
			GrabModeSync,
			GrabModeSync,
			None,
			None);
}

void dumpWorkspace(int wn) {
	Client *client;

	for (client=workspaces[wn].last; client; client = client->previous) {
			fprintf(stderr, "Client pointer %d\n", client);
	}
}

int changeWorkspace(int workspace) {

	//Trying to change to the current workspace
	if (workspace == currentWorkspace) { return False; }
	dumpWorkspace(workspace);

	Client *client;
	for (client=workspaces[workspace].last; client; client = client -> previous) {
			XMapWindow(display, client -> window);
	}

	for (client=workspaces[currentWorkspace].last; client; client = client -> previous) {
			XUnmapWindow(display, client -> window);
	}

	currentWorkspace = workspace;
	return True;
}

/* ---------------------------
 * Event Handlers
 * --------------------------- */
void hMapRequest(XEvent *event) {
	logMessage("Map Request");
	fprintf(stderr, "Mapping request %d\n", event -> xmaprequest.serial);

	//Create Pointer to new Client struct and put window inside
	//Set the Client's previous pointer to the last client added to workspace
	//Now set the workspace's last client pointer to the new client!
	Client *newClient;
	newClient = malloc(sizeof(Client));
	newClient -> window = event -> xmaprequest.window;
	newClient -> previous = workspaces[currentWorkspace].last;
	workspaces[currentWorkspace].last = newClient;

	//Map
	XMapWindow(display, newClient -> window);

	//Window Fns: Raise, Border, Center Pointer, Setup Events
	raiseWindow  (&(newClient -> window) );
	applyBorder  (&(newClient -> window), unfocusedColor); 
	centerPointer(&(newClient -> window) );
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
	//Need an active Window and the Win Mod
	if (event -> xkey.state != Mod4Mask) { return; }

	int moveX = 0, moveY = 0;
	switch (event -> xkey.keycode) {
		case 114: moveX = MOVESTEP;       break; //Right
		case 116: moveY = MOVESTEP;       break; //Down
		case 113: moveX = -1 * MOVESTEP;  break; //Left
		case 111: moveY = -1 * MOVESTEP;  break; //Up

		case 10: changeWorkspace(0); break;
		case 11: changeWorkspace(1); break;
		case 12: changeWorkspace(2); break;
		case 13: changeWorkspace(3); break;
		case 14: changeWorkspace(4); break;
		case 15: changeWorkspace(5); break;
		case 16: changeWorkspace(6); break;
		case 17: changeWorkspace(7); break;
		case 18: changeWorkspace(8); break;
		case 19: changeWorkspace(9); break;
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

	if (event -> xkey.state == Mod4Mask) {
		//Warp pointer to corner if resizing
		XGetWindowAttributes(display, 
				event -> xbutton.subwindow, 
				&(origin.attributes)
				);
		origin.buttonEvent = event -> xbutton;


		if (event -> xbutton.button == 3) {
			//Store the origin X Cordinates so they may be subtracted
			//MotionNotify won't see the resulting cords from XWarpPointer
			origin.x = origin.attributes.width;
			origin.y = origin.attributes.height;
			XWarpPointer(
					display, event -> xbutton.subwindow, event -> xbutton.subwindow,
					0, 0, 0, 0, origin.attributes.width, origin.attributes.height
					);
		}

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
			/* Calculate Difference between current position original click and offset from XWarpPointer */
			int xDifference = event -> xbutton.x_root - origin.x - origin.attributes.x;
			int yDifference = event -> xbutton.y_root - origin.y - origin.attributes.y;

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

	memset(&workspaces, 0x00, sizeof(Workspace)*10);

	//Setup the Root Window, Active Screen, and Events
	root = RootWindow(display, activeScreen);
	activeScreen = DefaultScreen(display);
	setupEvents(&root);
	setCursor(&root, 56);

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
