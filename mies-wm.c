// mies-wm
#include <X11/Xlib.h>
#include <X11/Xutil.h> // For Resizing
#include <assert.h> 

#define NIL (0)

main() {
	//Create and Assert Display
	Display *dpy = XOpenDisplay(NIL); 
	assert(dpy); 

	//Hints
	XSizeHints *hints;
	XSetWMNormalHints(dpy, DefaultRootWindow(dpy), hints);

	//Mouse Event
	XButtonEvent mouse;
	XEvent event;

	//Grab Key
    XGrabKey(dpy, XKeysymToKeycode(
		dpy, XStringToKeysym("XK_ESCAPE")), Mod1Mask,
		DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync
	);


	// Loop X Events
	mouse.subwindow = None;
	for (;;) {
		XNextEvent(dpy, &event); 
		
		if (event.type == MotionNotify) {
		} else if (event.type == ButtonPress) {
		} else if (event.type == KeyPress) {
		}
	
		/*
		if (mouse.subwindow) {
			XCirculateSubwindowsDown(dpy, mouse.subwindow);
		}
		*/
		XSync(dpy, True);
		mouse.subwindow = None;
	}

	//Cleanup	
	XMapWindow(dpy, DefaultRootWindow(dpy));
	XFlush(dpy);
}

