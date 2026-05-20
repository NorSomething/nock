#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_aux.h>
#include <X11/keysym.h>
#include <unistd.h>
#include "auth.h"

// TODO - add error handling for the drawing text stuff lol

#define WIDTH 300
#define HEIGHT 100

xcb_connection_t *connection;
xcb_screen_t *screen;

static xcb_gc_t getFontGC(xcb_connection_t *connection, xcb_screen_t *screen, xcb_window_t window, const char *font_name) {

	//get font 
	xcb_font_t font = xcb_generate_id(connection);
	xcb_void_cookie_t fontCookie = xcb_open_font_checked(connection, font, strlen(font_name), font_name);



	//creating graphics context
	xcb_gcontext_t  gc            = xcb_generate_id (connection);
	uint32_t        mask          = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
	uint32_t        value_list[3] = { screen->black_pixel, screen->white_pixel,	font };

	xcb_void_cookie_t gcCookie = xcb_create_gc_checked (connection, gc, window, mask, value_list );

	//close the font
	fontCookie = xcb_close_font_checked (connection, font);
	return gc;


}

static void draw_text(xcb_connection_t *connection, xcb_screen_t *screen, xcb_window_t window, int16_t x1, int16_t y1, const char *label) {

	//label is the text we will draw

	//getting graphics context
	xcb_gcontext_t gc = getFontGC(connection, screen, window, "fixed");

	//drawing text
	xcb_void_cookie_t text_cookie = xcb_image_text_8_checked(connection, strlen(label), window, gc, x1, y1, label);

	//free the gc 
	xcb_void_cookie_t gcCookie = xcb_free_gc(connection, gc);

}

xcb_window_t create_window() {
	
	uint32_t mask;
	uint32_t values[2];

	xcb_window_t window; //the main lockscreen window
	xcb_void_cookie_t cookie;

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

	window = xcb_generate_id(connection);
	cookie = xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, 640, 480, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, values);
  
	xcb_map_window(connection, window);

	return window; //giving the window to main to use
}

int main() {

	connection = xcb_connect(NULL, NULL);
	screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	xcb_key_symbols_t *syms = xcb_key_symbols_alloc(connection);


	xcb_window_t window =  create_window();
	xcb_flush(connection); //send all reqs after window is create_window

	xcb_generic_event_t *event; //universal event type container
	int running = 1;

	char password[1024] = {0}; 
	int password_len = 0;

	xcb_flush(connection);
	xcb_aux_sync(connection); //wait for window to actually be on screen

	//routing input now
	xcb_grab_keyboard_cookie_t kb_cookie = xcb_grab_keyboard(
		connection,
		0,                    // owner_events
		screen->root,		  // The window to get the keyboard focus
		XCB_CURRENT_TIME,     // timestamp
		XCB_GRAB_MODE_ASYNC,  // pointer_mode
		XCB_GRAB_MODE_ASYNC   // keyboard_mode
	);

	xcb_grab_keyboard_reply_t *kb_reply = xcb_grab_keyboard_reply(connection, kb_cookie, NULL);
	free(kb_reply);
	printf("Keyboard locked to nock now...\n");

	//TODO - add error handling to this lol

	//for user input graphically buffer thingy
	char temp_buffer[100];
	char user_input[100];
	int x = 0;

	char *username = getlogin();

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

					if (auth_user(username, password)) {
						printf("Access Granted!\n");
						running = 0;
					}
					else {
						printf("Acess Denied!\n");
						memset(password, 0, sizeof(password)); //resetting buffer
						password_len = 0;							
						memset(user_input, 0, sizeof(user_input));
						x = 0;
						xcb_clear_area(connection, 1, window, 0, 0, 0, 0); //all zeroes is a special case in xcb for entire window
						xcb_flush(connection);
					}

				}
				else if (keysym == XK_BackSpace) {

					password_len--;
					password[password_len] = '\0';
					user_input[x--] = '\0';					xcb_clear_area(connection, 1, window, 0, 0, 0, 0); //all zeroes is a special case in xcb for entire window
					snprintf(temp_buffer, 17+x+1, "Enter Password : %s", user_input);
					draw_text(connection, screen, window, 10, 100-10, temp_buffer);
					xcb_flush(connection);


			    }
				else if (keysym == 0x0020) {
					//spacebar
					user_input[x++] = ' ';
					snprintf(temp_buffer, 17+x+1, "Enter Password : %s", user_input);
					draw_text(connection, screen, window, 10, 100-10, temp_buffer);
					xcb_flush(connection);
					password[password_len++] = ' ';


				}
				else if (keysym >= XK_a && keysym <= XK_z) {
					char letter = (char)keysym; 
					user_input[x++] = letter;
					snprintf(temp_buffer, 17+x+1, "Enter Password : %s", user_input);
					draw_text(connection, screen, window, 10, 100-10, temp_buffer);
					xcb_flush(connection);
					password[password_len++] = letter;
				}

				//running = 0;
				break;

			case XCB_EXPOSE:
				char temp[100];
				snprintf(temp, 26+strlen(username), "Enter password of user %s", username);
				draw_text(connection, screen, window, 10, 100-30, temp);
				draw_text(connection, screen, window, 10, 100-10, "Enter Password : ");
				xcb_flush(connection);
				break;

			case XCB_KEY_RELEASE:
				xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;

				switch (kr->detail) {
				
					//esc
					case 9:
						free(event);
						xcb_disconnect(connection);
						return 0;

				}

			default:
				break;
		}

		free(event);

	}


	xcb_disconnect(connection); //cleanup before exit
	xcb_key_symbols_free(syms);
	xcb_ungrab_pointer(connection, XCB_CURRENT_TIME);
	xcb_ungrab_keyboard(connection, XCB_CURRENT_TIME);
	return 0;

}

