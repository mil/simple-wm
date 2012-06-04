#include <X11/Xlib.h>

typedef struct {
	XButtonEvent       buttonEvent;
	XWindowAttributes  attributes;
} PointerMotion;

typedef struct {
	Window *active;
	Window windows[20];
	int lastElement;
} Workspace;
