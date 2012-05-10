// mies-wm
#include <xcb/xcb.h>
#include <assert.h> 

#define NIL (0)

main() {
	int screen;
	xcb_connection_t *xcbConnection;
	xcb_generic_event_t *event;

	xcbConnection = xcb_connect(
		NIL,
		&screen
	);   

	for (;;) {
		event = xcb_wait_for_event(xcbConnection);
	}

	xcb_flush(xcbConnection);
	xcb_disconnect(xcbConnection);

	return 0;
}
