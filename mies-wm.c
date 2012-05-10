// mies-wm
#include <xcb/xcb.h>

#define NIL (0)

main() {
	xcb_connection_t *xcbConnection;
	xcb_generic_event_t *event;

	xcbConnection = xcb_connect(NIL, NIL);

	for (;;) {
		event = xcb_wait_for_event(xcbConnection);
	}
}
