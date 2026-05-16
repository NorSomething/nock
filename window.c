//gcc window.c auth.c -o nock -lxcb -lxcb-keysyms -lpam -lpam_misc

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>
#include <unistd.h>
#include "auth.h"

xcb_connection_t *connection;
xcb_screen_t *screen;

void create_window() {
	
	uint32_t mask;
	uint32_t values[2];

	xcb_window_t window;
	xcb_void_cookie_t cookie;

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

	window = xcb_generate_id(connection);
	cookie = xcb_create_window(connection,
			     XCB_COPY_FROM_PARENT, window, screen->root,
			     0, 0, 640, 480,
			     0,
			     XCB_WINDOW_CLASS_INPUT_OUTPUT,
			     screen->root_visual,
			     mask, values);
  
	xcb_map_window(connection, window);
}

int main() {

	connection = xcb_connect(NULL, NULL);
	screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	xcb_key_symbols_t *syms = xcb_key_symbols_alloc(connection);


	create_window();
	xcb_flush(connection); //send all reqs after window is create_window

	xcb_generic_event_t *event; //universal event type container
	int running = 1;

	char password[1024] = {0}; 
	int password_len = 0;

	while (running && (event = xcb_wait_for_event(connection))) {

		if (event->response_type & 0x80) {
			//if its a fake event like vir keyb or somethign
			free(event);
			continue;
		}

		switch (event->response_type) { 
			//response_type is the actual raw ID of any event
			
			case XCB_KEY_PRESS:

				xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;

				xcb_keysym_t keysym = xcb_key_symbols_get_keysym(syms, kp->detail, 0); //converting to syms

				if (keysym == XK_Return) {

					password[password_len] = '\0';

					if (auth_user("nirmal")) {
						printf("Access Granted!\n");
						running = 0;
					}
					else {
						printf("Acess Denied!\n");
						memset(password, 0, sizeof(password)); //resetting buffer
						password_len = 0;															   
					}

				}
				else if (keysym == XK_BackSpace) {
					// User pressed Backspace, delete last character
			    }
				else if (keysym >= XK_a && keysym <= XK_z) {
					// It's a lowercase letter! typecast it to char to use it
					char letter = (char)keysym; 
					//printf("You entered %c\n", letter);
					password[password_len++] = letter;
				}

				//running = 0;
				break;

			default:
				break;
		}

		free(event);

		if (strcmp("nirmal", password) == 0) {
			printf("Correct password entered.\n");
			break;
		}


	}


	xcb_disconnect(connection); //cleanup before exit
	xcb_key_symbols_free(syms);
	return 0;

}

