#include <security/_pam_types.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include "auth.h"

static struct pam_conv conv = {
	misc_conv,
	NULL
};

int auth_user(const char *username) {

	pam_handle_t *pamh = NULL; //stores state of AUTH sesh
	int retval;

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
