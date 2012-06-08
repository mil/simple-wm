#include <X11/Xlib.h>

typedef struct {
	XButtonEvent       buttonEvent;
	XWindowAttributes  attributes;
	int                x;
	int                y;
} PointerMotion;

//Inspired by DWM
typedef struct Client Client;
struct Client {
	Window window;
	Bool   isFocused;
	Client *previous;
};

typedef struct {
	Window active;
	Window windows[20];
	Client *last;
	int lastElement;
} Workspace;
