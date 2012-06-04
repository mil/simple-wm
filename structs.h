#include <X11/Xlib.h>

typedef struct {
	XButtonEvent       buttonEvent;
	XWindowAttributes  attributes;
} PointerMotion;
