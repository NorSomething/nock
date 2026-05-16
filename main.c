#include <security/_pam_types.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h> //for clean password entry
#include <stdio.h>

static struct pam_conv conv = {
	misc_conv,
	NULL
};

int main(int argc, char *argv[]) {

	pam_handle_t *pamh = NULL; //stores state of AUTH sesh
	int retval;
	const char *user = "nirmal";

	if (argc == 2) {
		user = argv[1];
	}

	if (argc > 2) {
		printf("Usage : ./check_user [username]\n");
		exit(1);
	}

	retval = pam_start("nock", user, &conv, &pamh);

	if (retval == PAM_SUCCESS)
		retval = pam_authenticate(pamh, 0);

	if (retval == PAM_SUCCESS) {
		printf("Password Correct.\n");
	}
	else {
		printf("Incorrect Password.\n");
	}

	//closing PAM 
	if (pam_end(pamh, retval) != PAM_SUCCESS) {
		pamh = NULL;
		printf("Failed to close pam..\n");
		exit(1);
	}

	if (retval == PAM_SUCCESS) {
		return 0;
	}
	else {
		return 1;
	}
}
