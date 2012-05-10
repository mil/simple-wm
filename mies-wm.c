// mies-wm
#include <X11/Xlib.h>
#include <X11/Xutil.h> // For Resizing
#include <assert.h> 

#define NIL (0)

/*
void mouseClick(XEvent * e, XButtonEvent * m, Display * d) {
	XEvent event = * e;
	XButtonEvent mouse = * m;
	Display display = * d;
}
*/

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

	/* main event loop */
	for (;;) {

		XNextEvent(dpy, &event); 
		
		if (event.type == MotionNotify) {
			//mouseClick( & event, & mouse, & dpy);

			switch(mouse.button) {
				case 1:
					XMoveResizeWindow(
							dpy, mouse.subwindow, 
							event.xbutton.x_root - mouse.x_root,
							event.xbutton.y_root - mouse.y_root,
							500,500
					);
					break;
				case 2:
					break;
				case 3:
					break;
			}


		} else if (event.type == ButtonPress) {
		} else if (event.type == KeyPress) {
		}
	}

	//Cleanup	
	XMapWindow(dpy, DefaultRootWindow(dpy));
	XFlush(dpy);
}

