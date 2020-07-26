#include "functions.h"
#include <stdio.h>
#include <string.h>   // strlen
#include <stdlib.h>  // malloc
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>   //write
#include <ctype.h> // tolower
#include <sys/un.h> // struct sockaddr_un
#include <sys/stat.h> // mkdir
#include <unistd.h> // change directory
#include <math.h>
#include <sys/statvfs.h>
#include <fcntl.h> // Working with unix commands and files
#include <sys/mman.h> // mmap


/*-----------------------------------------------------------------------------------*/


   /*********************/
  /* General Functions */
 /*********************/

//////////////////////////////////////////////////////////////////////

/* This function checks whether the input is valid */

int is_input_valid(const char test, const char min, const char max){
	if (min <= test && test <= max)
		return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////

/* This function turns a string to lowercase string */

void string_to_lowercase(char *str){
	int i = 0;

	while (str[i] != '\0'){
		if(isalpha(str[i])){
			if (isupper(str[i])){
				str[i] = tolower(str[i]);
			}
		}
		i++;
	}
}

/////////////////////////////////////////////////////////////////////

/* This function checks if the file exists */

int isFileExists(char* filename){
        const char* find_str = ".txt";
	FILE* fp = NULL;

	// NEW
	char *ptr = strstr(filename, find_str);
	//printf("FILENAME is: %s, FIND STR is: %s\n", filename, find_str); // check

	if(ptr == NULL){
		//printf("It's NULL\n"); // CHECK 
		strcat(filename, ".txt"); // fix filename extension
		//printf("Fixed!\n"); // check
	}
	//else
		//printf("NORMAL\n");

	//printf("BEFORE open\n"); // check
	// open the file
	fp = fopen(filename, "r");
	//printf("AFTER open\n"); // check

        if(fp != NULL){
                fclose(fp);
                return TRUE;
        }
        return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////

/* This function check if there is enough space for backup operation  */

int is_backup_valid(float disk_size, float file_size){
	if(disk_size > file_size)
		return TRUE;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////

/* This function calculates the maximux size of the file */

float get_file_size(char* filename) {
	struct stat file_size; //avilable to check file size
	float size = 0.0;

	// fix file extension
	strcat(filename, ".txt");

	//printf("filename is: %s",filename); //check

	//get the file size
	stat(filename, &file_size);

	size = ceil(file_size.st_size * pow(10, -3)); // kilobyte unit

	//printf("the size of the file is : %.3f\n", size); //check

	return size;
}

/////////////////////////////////////////////////////////////////////////////////

/* This function calculates the maximux size of free space in the hardisk */

float get_size_of_available_space(void) {
	float free_space_size = 0.0;
	struct statvfs storage;
	char path[100];

	if (statvfs(getcwd(path, 100), &storage) != 0) {
		printf("The path is not exist\n"); // error happens, just quits here
		exit(1);
	}

	//get the free size storage
	free_space_size = storage.f_bsize * storage.f_bavail;
	free_space_size = ceil(free_space_size * pow(10, -3)); //kilobyte unit
	//printf("the free size is : %f\n", free_space_size); // check

	return free_space_size;
}

/////////////////////////////////////////////////////////////////////

/* This function prints an ASCII ART Image from text file */

void print_image(FILE* fptr)
{
	char read_string[MAX_LEN];

	while (fgets(read_string, sizeof(read_string), fptr) != NULL)
		printf("%s", read_string);
}


/////////////////////////////////////////////////////////////////////


/*---------------------------------------------------------------------------------------------------------------------------------------------*/

   /********************/
  /* Server Functions */
 /********************/

/* This function send greeting message to the client - need to change!! */

void send_greeting_message_to_client(int socket){
	int write_return_value = 0, n = 0;
	char *filename = "image.txt";
	char read_string[MAX_LEN], buffer[MAX_LEN];
        FILE* fp = NULL;

	/* check if the file exists - why it not working?!?!
        if( isFileExists(file) )
                printf("File exists\n");
        else{
                printf("File does not exists\n");
                return;
        }*/

	// open the image file
	fp = fopen(filename, "r");

	// read the data from the file and send it to the client
	while  (!(feof(fp))){
		n = fread(buffer, 1, 100, fp);
                buffer[n]='\0';
                //printf("Red %d bytes, content - %s\n)", n, buffer);

		send_message_to_client(socket, buffer, (int)strlen(buffer));
        }

	// close the image file
	fclose(fp);
}


///////////////////////////////////////////////////////////////////////////////////////////////


// IPv4 Setup

void ipv4_setup_server(void){
        int socket_desc, new_socket;
        int size_of_sockaddr_in;
        char client_choice;
        struct sockaddr_in server, client;

        // Creating a socket
        socket_desc = socket(AF_INET , SOCK_STREAM, 0); // IPv4, TCP Protocol, IP protocol

        if (socket_desc == ERROR){ // Checks if the socket was successfully created
                printf("Could not create socket, disconnecting...\n");
                exit(1);
        }
        else{
                //printf("Socket was created!\n");

                // Binding a socket to a particular "address and port" combination

                // Prepare the sockaddr_in structure
                server.sin_family = AF_INET; // IPv4
                server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Local host
                server.sin_port = htons( 8080 ); // Port of local host , 3490

                // Bind
                if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
                        puts("bind failed, disconnecting...\n");
                        exit(1);
                }
                //else
                //      puts("bind done");  // Remark this line later

                // Listen
                if (listen(socket_desc, MAX_NUMBER_OF_CLIENTS) == ERROR) {
                        printf("listen failed, disconnecting...\n");
                        exit(1);
                }

                puts("Waiting for incoming connections...");

		// main accept() loop
                while( TRUE ){
                    //Accept and incoming connection

                        size_of_sockaddr_in = sizeof(struct sockaddr_in);
                        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&size_of_sockaddr_in);

                        if (new_socket < 0)
                                continue;  // continue to try accepting another clien
                        else
                                puts("client was connected"); // connection accepted

                         if (!fork()) { // this is the child process
                                //printf("Im a child process\n");
                                close(socket_desc); // child doesn't need the listen

				// work flow
				send_greeting_message_to_client(new_socket);// greeting welcome
                                show_menu_for_client(new_socket);
                                client_choice = get_client_choice(new_socket);
                                response_after_selecting_on_the_menu(new_socket, client_choice);

                                // end of child process
                                close(new_socket);
                                exit(0);
                        }
                }
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////

// IPv6 Setup

void ipv6_setup_server(void){
        int socket_desc, new_socket;
        int size_of_sockaddr_in;
        char client_choice;
        struct sockaddr_in6 server, client;

		/* Creating a socket */
		socket_desc = socket(AF_INET6 , SOCK_STREAM , 0); // IPv6, TCP Protocol, IP protocol

		if (socket_desc == ERROR){ // Checks if the socket was successfully created
			printf("Could not create socket, disconnecting...\n");
			exit(1);
		}
		else{
			/* Binding a socket to a particular "address and port" combination */
			// Prepare the sockaddr_in6 structure
			server.sin6_family = AF_INET6;
			server.sin6_addr = in6addr_any;
			server.sin6_port = htons(8080);

			// Bind
			if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
				puts("bind failed, disconnecting...");
				exit(1);
			}
			//puts("bind success");

			// Listen
			if (listen(socket_desc, MAX_NUMBER_OF_CLIENTS) == ERROR) {
				printf("listen failed, disconnecting...\n");
				exit(1);
			}

			puts("Waiting for incoming connections...");

			/* main accept() loop  */
			while( TRUE ){
				//Accept and incoming connection

				size_of_sockaddr_in = sizeof(struct sockaddr_in6);
				new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&size_of_sockaddr_in);
				if (new_socket < 0){
					continue;  // continue to try accepting another client
				}
				else
					puts("client was connected");

				if (!fork()) { // this is the child process
					//printf("Im a child process\n");
					close(socket_desc); // child doesn't need the listen


					// work flow
					send_greeting_message_to_client(new_socket);// greeting welcome
	                                show_menu_for_client(new_socket);
	                                client_choice = get_client_choice(new_socket);
	                                response_after_selecting_on_the_menu(new_socket, client_choice);

					// end of child process
					close(new_socket);
					exit(0);
				}
			}
		}
}

/////////////////////////////////////////////////////////////////////////////////////////////

// Domain unix socket

void unix_domain_setup_server(void){
	int socket_desc = 0, new_socket = 0, size_of_sockaddr_un = 0, len = 0, size_of_client = 0;
	char client_choice;
	struct sockaddr_un server, client;

	/* Creating a Unix Socket */
	socket_desc = socket(AF_UNIX, SOCK_STREAM, 0); // Socket Descriptor

	if (socket_desc == ERROR){ // Checks if the socket was successfully created
		printf("Could not create socket, disconnecting...\n");
		exit(1);
	}
	else{
		//printf("Socket was created!\n");

		/* Binding a Unix socket to a particular address */

		// Prepare the sockaddr_un struct - Unix Socket
		server.sun_family = AF_UNIX; 
		strcpy(server.sun_path, NAME);
		unlink(server.sun_path); // remove the socket if it already exists
		len = strlen(server.sun_path) + sizeof(server.sun_family);

		// Bind
		if(bind(socket_desc, (struct sockaddr *)&server, len) < 0){
			puts("bind failed, disconnecting...\n");
			exit(1);
		}
		//else
			//puts("bind done");  // Remark this line later

		// Listen
		if (listen(socket_desc, MAX_NUMBER_OF_CLIENTS) == ERROR) {
			printf("listen failed, disconnecting...\n");
			exit(1);
		}

		puts("Waiting for incoming connections...");

		/* main accept() loop  */
		while( TRUE ){
			//Accept and incoming connection
		    size_of_sockaddr_un = sizeof(struct sockaddr_un);
			size_of_client = sizeof(client);

			// Accept
		    if ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&size_of_sockaddr_un)) == ERROR) {
				printf("accept failed, disconnecting...\n");
				exit(1);
			}

			if (new_socket < 0){
			// perror("accept failed");
				continue;  // continue to try accepting another client
	        }
	        else
				puts("client was connected");

           	if (!fork()) { // this is the child process
				close(socket_desc); // child doesn't need the listen


				// work flow
				send_greeting_message_to_client(new_socket);// greeting welcome
                                show_menu_for_client(new_socket);
                                client_choice = get_client_choice(new_socket);
                                response_after_selecting_on_the_menu(new_socket, client_choice);

				// end of child process
				close(new_socket);
				exit(0);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

/* This function creates the backup folder in the server */

void create_backup_folder(void){
        // Creating a directory
        if (mkdir("Backup Folder", 0777) == ERROR)
                printf("Backup folder is already exists!\n");
        else
                printf("Backup folder created!\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////

/* This function shows the menu for the client when he connects */

void show_menu_for_client(int socket){
        const char* message_to_send = "\n\n1.Check Backup Validity\n2.Backup a file with C style\n3.Backup a file with Unix style\n4.Backup a file with mmap\n5.Quit\nEnter your choice:";
        int write_return_value = 0;
        
	do{
			write_return_value = write(socket, message_to_send, strlen(message_to_send));
			//printf("message length: %d\n", (int)strlen(message_to_send)); // check
			
			if (write_return_value == ERROR)
				printf("Failed to write\n");
			else if (write_return_value == 0)
				printf("Nothing was written\n");
        }
        while(write_return_value < 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////

/* This function gets a choice from the client */

char get_client_choice(int socket){
        const int reply_size = 100; // change 100 to predefined processor constant #define
        int answer_flag = FALSE, recv_return_value = 0;
        char client_reply[reply_size];

        do{
			recv_return_value = recv(socket, client_reply, reply_size, 0);
			if (recv_return_value == ERROR)
				printf("Failed to read\n");
	      //else if (recv_return_value == 0)
				//printf("Connection is closed\n");
			else{
				answer_flag = TRUE;
				//puts(client_reply);
			}
        }
        while(!answer_flag);

        return client_reply[0];
}

///////////////////////////////////////////////////////////////////////////////////////////////

/* This function sends message to the client */

void send_message_to_client(int socket, char* message, int size_to_send){
	const int reply_size = 100;
	int write_flag = FALSE,  write_return_value = 0;
   
	do{
		write_return_value = write(socket,message, size_to_send);
		//printf("write value is: %d\n", write_return_value); // check

		if (write_return_value == ERROR)
			printf("Failed to write\n");
		else if (write_return_value == 0)
			printf("Nothing was written\n");
		else
			write_flag = TRUE;
	}
	while(!write_flag);

	//printf("server to client: message: %s\n", message); //check
}
///////////////////////////////////////////////////////////////////////////////////////////////

/* This function gets data from the client */

char* get_message_from_client(int socket){
        const int reply_size = 100; // 10??
        int recv_return_value = 0;
        char message_to_read[reply_size];

        do{
                recv_return_value = recv(socket, message_to_read, reply_size, 0);
                //printf("recive is: %d\n", recv_return_value); // check

		if (recv_return_value == ERROR)
	                printf("Failed to read\n");
                else if (recv_return_value == 0)
                        printf("Connection is closed\n");
               // else
                        //puts(message_to_read);
        }
        while(recv_return_value < 1);

        //message_to_read[strlen(message_to_read)] = '\0';
        message_to_read[recv_return_value] = '\0';
        char *message = message_to_read;

        return message;
}


///////////////////////////////////////////////////////////////////////////////////////////////
/* This function react to the client's answer */

void response_after_selecting_on_the_menu(int socket, const char user_choice){ // !! WORKING HERE !!
        char client_choice;
	const char* v = "valid";
	// switch statement with response for each case
        switch(user_choice){
                case '1': printf("Calculation of free space in the hard disk!\n");
			  send_disk_size_to_client(socket);
  			  char* valid =  get_message_from_client(socket);
			  //printf("valid is: %s\n", valid); // check
			  //printf("valid length: %d, v length: %d\n",(int)strlen(valid),(int)strlen(v)); // check
			  if( strcmp(valid, v) == 0 ){
				//printf("IT IS VALID!\n"); // check
			  	show_menu_for_client(socket);
				client_choice = get_client_choice(socket);
                                response_after_selecting_on_the_menu(socket, client_choice);
			  }
                          break;
                case '2': printf("Server backup file with C style!\n");
			  backup_file_c_style_server(socket);
                          break;
                case '3': printf("Server backup file with Unix style\n");
			  backup_file_unix_style_server(socket);
                          break;
                case '4': printf("Server backup file with MMAP\n");
			  backup_file_mmap_server(socket);
                          break;
                case '5': printf("client was disconnected!\n");
                          break;
                default: printf("Bad choice\n");
                         // send message to the client
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////

/* This function send the size of the disk to the client from the server*/

void send_disk_size_to_client(int socket){
	const int reply_size = 100;
    int write_flag = FALSE,  write_return_value = 0;
	float disk_size = 0.0;
	char str[20] = { 0 };

	// get the size of the file - float
	disk_size = get_size_of_available_space();

	//casting float to string
	sprintf(str, "%.2f", disk_size);
	//printf("After casting string is: %s\n", str); // check

	//send data over the socket to the client
	do{
		write_return_value = write(socket,str,strlen(str)); // 3rd argument was reply_size

		if (write_return_value == ERROR)
			printf("Failed to write\n");
		else if (write_return_value == 0)
			printf("Nothing was written\n");
		else
			write_flag = TRUE;
	}
	while(!write_flag);

	//printf("message: %s\n", str); // check
}

//////////////////////////////////////////////////////////////////////////////////////////////

/*SERVER
1. get the name of the file to backup
2. open new file with this name
3. get chunk of data from the client
4. write this chunk to the new file
5. repeat steps 3 & 4 until getting the string "finish"
6. close the new file
*/

void backup_file_c_style_server(int socket){
        FILE *file = NULL;
	char filename[30], buffer[101], path[100]; // buffer to 100??
        char* finish = "finish";
	int answer_flag = FALSE, recv_return_value = 0, n = 0;

	// Change directory to "Backup Folder"
        //printf("%s\n", getcwd(path, 100)); // before change
        chdir("Backup Folder");
        //printf("%s\n", getcwd(path, 100)); // after change


	// get filename from the client - #1
	do{
	        recv_return_value = recv(socket, filename, 30, 0);
	        if (recv_return_value == ERROR)
	                printf("Failed to read\n");
	        else{
        	        answer_flag = TRUE;
	                //puts(filename);
	        }
        }
        while(!answer_flag);

        printf("Back up this file: %s\n", filename);
        file = fopen(filename, "w+");  // step #2


        // steps #3 & #4
	recv_return_value = 1;
        while  ( recv_return_value ){
	        recv_return_value = recv(socket, buffer, 100, 0);
		buffer[recv_return_value] = '\0';

		//printf("recieve is: %d\n", recv_return_value); // check
		//printf("buffer length is: %d\n", (int)strlen(buffer)); // check

		n = fwrite(buffer , 1, strlen(buffer) , file);
		//printf("written %d bytes\n", n);
        }


        // close the file
        fclose(file);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void backup_file_unix_style_server(int socket){
	char filename[30], buffer[101], path[100];
	int answer_flag = FALSE, recv_return_value = 0,backup_file = 0, n = 0;

        //printf("%s\n", getcwd(path, 100)); // before change
	// Change directory to "Backup Folder"
        chdir("Backup Folder");
        //printf("%s\n", getcwd(path, 100)); // after change


	// get filename from the client - #1
	do{
	        recv_return_value = recv(socket, filename, 30, 0);
	        if (recv_return_value == ERROR)
	                printf("Failed to read\n");
		//else if (recv_return_value == 0)
	                //printf("Connection is closed\n");
	        else{
        	        answer_flag = TRUE;
	                puts(filename);
	        }
        }
        while(!answer_flag);


        printf("Back up this file: %s\n", filename);
	backup_file = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777); // step #2

        // steps #3 & #4
	recv_return_value = 1;
        while  ( recv_return_value ){
	        recv_return_value = recv(socket, buffer, 100, 0);
		buffer[recv_return_value] = '\0';

                //printf("buffer: %s\n", buffer); // check
		write(backup_file, buffer , (int)strlen(buffer));
        }

        //step #5
        close(backup_file);
}



//////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------------------------------------------------------------*/

/*********************/
/* Client Functions */
/*******************/

// IPv4 setup

void ipv4_setup_client(char* filename){
	int socket_desc = 0;
	char user_answer;
	struct sockaddr_in server;

	// Create a socket
	socket_desc = socket(AF_INET , SOCK_STREAM, 0); // IPv4, Tcp Protocol, IP Protocol

	// Checks if a socket was successefully created
	if (socket_desc == ERROR)
		printf("Could not create socket\n");
	else
	{
		//printf("Socket was created\n");
		// Prepare the sockaddr_in structure
		server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Local host
		server.sin_family = AF_INET; // IPv4
		server.sin_port = htons( 8080 ); // Port


		//Connect to remote server
		if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0){
			puts("connect error\n");
			exit(1);
		}

		puts("connected to the server");

		while( TRUE ){
			display_greeting_message(socket_desc);
			get_answer_from_server(socket_desc,&user_answer);
			send_choice_to_server(socket_desc, user_answer, filename);
		}
	}
}

// IPv6 Setup
void ipv6_setup_client(char* filename){
	int socket_desc = 0;
	char user_answer;
	struct sockaddr_in6 server;

   /* Create a socket */
	socket_desc = socket(AF_INET6 , SOCK_STREAM , 0); // IPv6, Tcp Protocol, IP Protocol

	// Checks if a socket was successefully created
	if (socket_desc == ERROR)
		printf("Could not create socket\n");
	else{
		//printf("Socket was created\n");
		// Prepare the sockaddr_in6 structure
		inet_pton(AF_INET6, "::1", &server.sin6_addr);// Local host
		server.sin6_family = AF_INET6; // IPv6
		server.sin6_port = htons( 8080 ); // Port

	    //Connect to remote server
		if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0){
			puts("connect error\n");
			exit(1);
		}

		puts("connected to the server");

		while( TRUE ){
			display_greeting_message(socket_desc);
			get_answer_from_server(socket_desc,&user_answer);
			send_choice_to_server(socket_desc, user_answer, filename);
		}
	}
}

// Unix Domain Socket Setup
void unix_domain_setup_client(char* filename){
	int socket_desc = 0, len = 0;
	char user_answer;
    struct sockaddr_un server;

	/* Create a Unix Socket */
	socket_desc = socket(AF_UNIX, SOCK_STREAM, 0);

	// Checks if a socket was successefully created
	if (socket_desc == ERROR){
		printf("Could not create socket\n");
		exit(1);
	}
	else{
		//printf("Socket was created\n");  // remark it later

		server.sun_family = AF_UNIX;
		strcpy(server.sun_path, SOCK_PATH ); //SOCK_PATH
		len = strlen(server.sun_path) + sizeof(server.sun_family);

		//Connect to remote server
		if (connect(socket_desc, (struct sockaddr *)&server, len) == ERROR) {
			printf("connect failed, disconnecting...\n");
			exit(1);
		}

		printf("connected to the server\n");


		while(TRUE){
			display_greeting_message(socket_desc);
			get_answer_from_server(socket_desc,&user_answer);
			send_choice_to_server(socket_desc, user_answer, filename);
		}
	}

	close(socket_desc);
}

/* This function gets message from the server */

char* get_message_from_server(int socket){
        const int reply_size = 2000;
        int recv_return_value = 0;
        char message_to_read[reply_size];


        do{
			recv_return_value = recv(socket, message_to_read, reply_size, 0);
			if (recv_return_value == ERROR)
				printf("Failed to read\n");
			else if (recv_return_value == 0)
				printf("Connection is closed\n");
			//else
				//puts(message_to_read);
        }
        while(recv_return_value < 1);
        
	message_to_read[strlen(message_to_read)] = '\0';
	
	char *message = message_to_read;

	return message;
        
}

/* This function gets answer from the server */

void get_answer_from_server(int socket, char *user_answer){
        const int reply_size = 139;
        int recv_return_value = 0;
        char *message_to_read = NULL;

        message_to_read = (char *) malloc (sizeof(char) * reply_size+1);
		
        do{
			recv_return_value = recv(socket, message_to_read, reply_size, 0);
			if (recv_return_value == ERROR)
				printf("Failed to read\n");
			else if (recv_return_value == 0)
				printf("Connection is closed\n");
			else
				puts(message_to_read);
        }
        while(recv_return_value < 1);

        free(message_to_read); // free memory allocation

        get_client_answer(user_answer);
}

/////////////////////////////////////////////////////////////////////////////

/* This function checks the validation of backup operation */

void check_validation_of_backup_operation(int socket, char* filename){
	float disk_size = 0.0, file_size = 0.0;
	char user_answer;
	char *valid = "valid";
	
	//printf("filename in validation: %s\n", filename); //check

	disk_size = get_disk_size_from_server(socket);
	file_size = get_file_size(filename);
	
	if(is_backup_valid(disk_size, file_size)){
		printf("backup operation is valid!\n");
		//send "valid" message to the server
		send_message_to_server(socket, valid, strlen(valid));
		//printf("valid length: %d\n", (int)strlen(valid)); // check
		//display menu - get it from server
		get_answer_from_server(socket,&user_answer);
        	//send choice
		//printf("filename is: %s\n", filename); // check
		send_choice_to_server(socket, user_answer, filename);
	}
	else{
		printf("backup operation is invalid!, bye bye...\n");
		exit(1);
	}
}

/* This function sends a message to the server */

void send_choice_to_server(int socket, char client_choice, char* filename){
        const int reply_size = 100;
        int write_flag = FALSE,  write_return_value = 0;
        char *client_reply = NULL;
	
        client_reply = (char *) malloc (sizeof(char) * (reply_size+1) );

        do{
			if (!is_input_valid(client_choice,'1','5')){
				printf("HEY! not valid\n");
				get_client_answer(&client_choice);  //get another choice!
			}
			else{
				client_reply[0] = client_choice;
				client_reply[1] = '\0';

				write_return_value = write(socket,client_reply,reply_size);

				if (write_return_value == -1)
					printf("Failed to write\n");
				else if (write_return_value == 0)
					printf("Nothing was written\n");
				else
					write_flag = TRUE;
			}
        }
        while(!write_flag);

	//switch or if statements for the states
	switch( client_choice ){
	case '1':
		 printf("\nChecking backup availability...\n");
		 check_validation_of_backup_operation(socket, filename);
		 break;
	case '2':
		 printf("\nBackup file with C style...\n");
		 backup_file_c_style_client(socket, filename);
		 break;
	case '3':
		 printf("\nBackup file with Unix style...\n");
		 backup_file_unix_style_client(socket, filename);
		 break;
	case '4':
		 printf("\nBackup file with MMAP...\n");
		 backup_file_mmap_client(socket, filename);
		 break;
	case '5':
		 printf("\ndisconnecting, bye bye...\n");
		 exit(0);
		 break;
	default:
		// do nothing
		break;
	}

        free(client_reply);
}

/* This function gets the disk size from the server */

float get_disk_size_from_server(int socket){
	float disk_size = 0.0;
	
	// gets the disk size from the server - string
	char *file_size_str = get_message_from_server(socket);
	
	// cast string to float
	disk_size = strtof(file_size_str, NULL);
	//printf("file size is: %.2f\n", disk_size); // check
	
	return disk_size;
}

/* This function get an input from the client */

void get_client_answer(char *user_answer){
	char temp;

	*user_answer = getchar();
	temp = getchar(); // clean buffer
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void send_message_to_server(int socket, char* message, int size_to_send){
        const int reply_size = 100;
        int write_flag = FALSE,  write_return_value = 0;
       // char *client_reply = NULL;

       // client_reply = (char *) malloc (sizeof(char) * (reply_size+1) );

        do{
	        write_return_value = write(socket,message, size_to_send); //3rd argument is reply_size
		//printf("write value is: %d\n", write_return_value); // check

	        if (write_return_value == ERROR)
	                printf("Failed to write\n");
	        else if (write_return_value == 0)
	                printf("Nothing was written\n");
	        else
	                write_flag = TRUE;
        }
        while(!write_flag);

	//printf("message: %s\n", message); //remark later
}

//////////////////////////////////////////////////////////////////////////////////////////////////

/* C Style - Files  */
/* CLIENT
1.open the file to copy
2.send the name of the file to the server
3.read chunk of data from the file to the buffer
4. send to the server
5. repeat steps 3 & 4 until the end of the file
6. when finished , send "finish"
7. close the file

*/
void backup_file_c_style_client(int socket, char* filename){
	FILE *file = NULL;
	char buffer[100];
        int n = 0;

        // check if the file exists
        if( !isFileExists(filename) )
                return;

        printf("Back up this file: %s\n", filename);

        file = fopen(filename, "r");  // step #1

        // send the name of the file - step #2
	send_message_to_server(socket, filename, 30); //500

	// steps #3 & #4
        while  (!(feof(file))){
                n = fread(buffer, 1, 100, file);
                buffer[n]='\0';
                //printf("Red %d bytes, content - %s\n)", n, buffer);
		//printf("n is: %d, buffer length is: %d\n", n, (int)strlen(buffer));
		send_message_to_server(socket, buffer, (int)strlen(buffer));
        }


	//step #7
	fclose(file);

	//return to menu
	exit(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void backup_file_unix_style_client(int socket, char* filename){
        //FILE *file = NULL;
        char buffer[101];
        int backup_file ,n = 0;

        // check if the file exists
        if( isFileExists(filename) )
                printf("File exists\n");
        else{
                printf("File does not exists\n");
                return;
        }

        printf("Back up this file: %s\n", filename);

        //file = fopen(filename, "r");  // step #1
	backup_file = open(filename, O_RDONLY); // step #1

        // send the name of the file - step #2
        send_message_to_server(socket, filename, 30);

        // steps #3 & #4
        while  ( n = read(backup_file, buffer, 100) ){
                buffer[n]='\0';
		//printf("Red %d bytes, content - %s\n)", n, buffer); // check
                send_message_to_server(socket, buffer, (int)strlen(buffer) );
        }

        //step #6
        close(backup_file);
	
        //return to menu
        exit(0);
}

///////////////////////////////////////////////////////////////////////////////

/* SERVER */

void backup_file_mmap_server(int socket){
        char *src, *dst;
        struct stat statbuf;
	char filename[30], filesize[10], path[100];
        int file_size = 0, fdout, answer_flag = FALSE, recv_return_value = 0,backup_file = 0, n = 0, i = 0;


        //printf("%s\n", getcwd(path, 100)); // before change

        // Change directory to "Backup Folder"
        chdir("Backup Folder");

        //printf("%s\n", getcwd(path, 100)); // after change

        // get filename from the client - #
        do{
                recv_return_value = recv(socket, filename, 30, 0);
		//printf("recieve value: %d\n", recv_return_value); // check
                if (recv_return_value == ERROR)
                        printf("Failed to read\n");
                //else if (recv_return_value == 0)
                        //printf("Connection is closed\n");
                else{
                        answer_flag = TRUE;
                        //printf("filename after receive is: %s\n", filename); // check
                }
        }
        while(!answer_flag);


	// get file size
	answer_flag = FALSE;
        do{
                recv_return_value = recv(socket, filesize, 10, 0);
       		//printf("recieve value: %d\n", recv_return_value); // check

		if (recv_return_value == ERROR)
                        printf("Failed to read\n");
                //else if (recv_return_value == 0)
                        //printf("Connection is closed\n");
                else{
                        answer_flag = TRUE;
                        //printf("file size is: %s\n", filesize); // check
                }
        }
        while(!answer_flag);


        // open/create the new backup file
        printf("Back up this file: %s\n", filename);
	if ((fdout = open (filename, O_RDWR | O_CREAT | O_TRUNC, 0777 )) < 0) {
		printf ("can't create %s for writing\n", filename);
		return;
	}

	// cast filesize string to int
	sscanf(filesize, "%d", &file_size);

	char buffer[file_size];
        recv_return_value = recv(socket, buffer, file_size, 0);

	write(fdout, buffer , (int)strlen(buffer));

        // close the file
        close(fdout);
}

/* CLIENT */

void backup_file_mmap_client(int socket, char* filename){
	int fdin, n = 0;
	char buffer[101], str[256];
	char *src;
	struct stat statbuf;


	// check if the file exists
        if( isFileExists(filename) )
                printf("File exists\n");
        else{
                printf("File does not exists\n");
                return;
        }


	// open the backup file
 	if ((fdin = open (filename, O_RDONLY)) < 0){
		printf("can't open %s for reading\n", filename);
    		return;
   	}
	//printf("fdin is: %d\n",fdin); // check

	// send the name of the file
        send_message_to_server(socket, filename, 30);

	/* find size of input file */
	if (fstat (fdin,&statbuf) < 0) {
		printf ("fstat error");
		return;
	}

	//printf("SIZE OF THE FILE IS: %ld\n", statbuf.st_size); // check

	/* mmap the input file */
	if ((src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == (caddr_t) -1){
		printf ("mmap error for input\n");
	  	return;
	}


	// send the size of the file to the server
	sprintf(str, "%ld", statbuf.st_size);
	//printf("str is: %s\n", str); // check
	send_message_to_server(socket, str, 10);


	//printf("\nsrc BEFORE is: %s\n", src); // check
	// send the backup file content to the server
	send_message_to_server(socket, src, (int)statbuf.st_size);
	//printf("\n\nsrc AFTER is: %s\n", src); // check
	//printf("length: %d\n", (int)strlen(src)); // check

        // close the backup file
        close(fdin);

        //return to menu
        exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////

/* This function displays the welcome greeting  */
void display_greeting_message(int socket){
	const int reply_size = 614;
        int recv_return_value = 0;
        char message_to_read[reply_size];

        do{
		recv_return_value = recv(socket, message_to_read, reply_size, 0);

		if (recv_return_value == ERROR)
			printf("Failed to read\n");
		else if (recv_return_value == 0)
			printf("Connection is closed\n");
	}
        while(recv_return_value < 1);

	message_to_read[strlen(message_to_read)] = '\0';

	// display the greeting message
	printf("%s\n", message_to_read);
}
