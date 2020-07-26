/*
- This code was written by Matan Leizerovich and Doron Abramov
- This is the implementation of a client
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc , char *argv[]){
	// check the size of argc
    if (argc != CLIENT_MIN_ARGS){
		printf("The client did not meat the minimum requierment of %d argmunets!\n", CLIENT_MIN_ARGS);
		exit(1);
	}

	// agrc >= 3 - decide which type of network to work with
	string_to_lowercase(argv[1]);

	//printf("Text file to backup is: %s\n", argv[2]);

	if(strcmp("ipv4", argv[1]) == 0){
		//printf("IPV4 is in the house bitach!\n");
		ipv4_setup_client(argv[2]);
	}
	else if (strcmp("ipv6", argv[1]) == 0){
		//printf("IPV6 is in the house bitach!\n");
		ipv6_setup_client(argv[2]);
	}
	else if (strcmp("domain", argv[1]) == 0){
		//printf("Domain Unix socket is in the house bitach!\n");
		unix_domain_setup_client(argv[2]);
	}
	else{
		printf("Wrong network was entered as an argument!\n");
		exit(1);
	}

	return 0;
}
