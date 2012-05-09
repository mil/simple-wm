// mies-wm
#include <X11/Xlib.h>
#include <X11/Xutil.h> // For Resizing
#include <assert.h> 
#include <unistd.h> 

#define NIL (0)

main() {
	//Create and Assert Display
	Display *dpy = XOpenDisplay(NIL); 
	assert(dpy); 

	//Create Root Window
	Window rootWindow = XCreateWindow(
		dpy, DefaultRootWindow(dpy), // X and Parent
		0, 0, // X and Ypointer
		200, 100, 0, //Width, Height, Border
		CopyFromParent, CopyFromParent, CopyFromParent, // Depth, Class, Visual
		NIL, 0 //Mask, Attributes
	);
	

	//Hints
	XSizeHints *hints;
	XSetNormalHints(dpy, rootWindow, hints);

	XMapWindow(dpy, rootWindow);
	XFlush(dpy);
	sleep(10);
}
