#include <security/_pam_types.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"

//custom pam conv func to route inputs from window.c to here cleanly to check - custom password interaction
static int my_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *password) {

	struct pam_response *p = malloc(sizeof(struct pam_response));
	p->resp = (char *)malloc(strlen(password)+1);
	strcpy(p->resp, password);
	*resp = p;
		
	return PAM_SUCCESS;

}

static struct pam_conv conv = {
	*my_conv,
	NULL
};

int auth_user(const char *username, const char *pass) {

	pam_handle_t *pamh = NULL; //stores state of AUTH sesh
	int retval;

	conv.appdata_ptr = (char *)pass;

	retval = pam_start("nock", username, &conv, &pamh);

	if (retval == PAM_SUCCESS) {
		retval = pam_authenticate(pamh, 0);
	}

	//closing PAM 
	if (pam_end(pamh, retval) != PAM_SUCCESS) {
		pamh = NULL;
		printf("Failed to close pam..\n");
		exit(1);
	}

	if (retval == PAM_SUCCESS) {
		return 1;
	}
	else {
		return 0;
	}

}
