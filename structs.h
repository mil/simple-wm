#include <X11/Xlib.h>

typedef struct {
	XButtonEvent       buttonEvent;
	XWindowAttributes  attributes;
	int                x;
	int                y;
} PointerMotion;

typedef struct {
	Window active;
	Window windows[20];
	int lastElement;
} Workspace;
