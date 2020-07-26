/*
- This code was written by Matan Leizerovich and Doron Abramov.
- This file is the implementation of the server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc , char *argv[])
{
	// check the size of argc
	if (argc != SERVER_MIN_ARGS){
		printf("The server did not meat the minimum requierment of %d argmunets!\n", SERVER_MIN_ARGS);
		exit(1);
	}

	// creating backup folder
	create_backup_folder();

	// agrc >= 2 - decide which type of network to work with
	string_to_lowercase(argv[1]);

	if(strcmp("ipv4", argv[1]) == 0){
		//printf("IPV4 is in the house bitach!\n");
		ipv4_setup_server();
	}
	else if (strcmp("ipv6", argv[1]) == 0){
		//printf("IPV6 is in the house bitach!\n");
		ipv6_setup_server();

	}
	else if (strcmp("domain", argv[1]) == 0){
		//printf("Domain Unix socket is in the house bitach!\n");
		unix_domain_setup_server();
	}
	else{
		printf("Wrong network was entered as an argument!\n");
		exit(1);
	}

	return 0;
}
