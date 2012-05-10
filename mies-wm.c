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
	XSetNormalHints(dpy, DefaultRootWindow(dpy), hints);


	//Mouse Event
	XButtonEvent mouse;
	XEvent event;


	// Loop X Events
	mouse.subwindow = None;
	for (;;) {
		XNextEvent(dpy, &event);
	}

	//Cleanup	
	XMapWindow(dpy, DefaultRootWindow(dpy));
	XFlush(dpy);
}
