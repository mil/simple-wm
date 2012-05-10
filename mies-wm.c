// mies-wm
#include <xcb/xcb.h>

#define NIL (0)

main() {
	//Connection, Screen, and Event
	xcb_connection_t *xcbConnection;
	xcb_generic_event_t *event;

	//Connect and Setup Screen and Grab Btn
	xcbConnection = xcb_connect(NIL, NIL);

	for (;;) {
		event = xcb_wait_for_event(xcbConnection);
	}
}
