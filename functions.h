

/******************************************************************************
*  			 			==============================            			  *
*   		 			Matan&Doron File Backup System						  *
*  			 			==============================             			  *
*                                                                             *
*   			  			Implemention include:                  			  *
*  			      			---------------------                 			  *
* 																			  *       
*                        													  *
*			API List:  														  *
*			---------   													  *
*			1. Work with a number of arguments - agrc , argv  				  *
*			2. Work with a null terminated strings: 						  *
*			   strcpy , strcat ,strlen , strstr, strcmp  					  *
*			3. FILE API - C style 											  *
*			4. FILE API - Unix stlye  										  *
*			5. Working with files using mmap								  *
*			6. IPv4															  *
*			7. IPv6 														  *
*			8. Unix domain sockets  										  *
*			9. Stream sockets 												  *
*			10. Fork 														  *
*			11. Math library 												  *
*                                        									  *
*          	Server:					    									  *
*		    -------					    									  *
*			1. send_greeting_message_to_client 								  *
*			2. ipv4_setup_server                      		            	  *
*          	3. ipv6_setup_server                      						  *
*          	4. unix_domain_setup_server               				          *
*          	5. create_backup_folder                   				      	  *
*          	6. show_menu_for_client                            				  *
*          	7. get_client_choice 											  *
*			8. send_message_to_client 										  *
*			9. get_message_from_client                                		  *
*          	10. response_after_selecting_on_the_menu 						  *
*			11. send_disk_size_to_client                                      *
*          	12. backup_file_c_style_server                    				  *
*          	13. backup_file_unix_style_server 								  *
*			14. backup_file_mmap_server 									  *
*									   										  *
*			Client:					   										  *
*			-------					   										  *
*			1. ipv4_setup_client               							      *
*       	2. ipv6_setup_client               							      *
*       	3. unix_domain_setup_client         					          *
*       	4. get_message_from_server            						      *
*       	5. send_choice_to_server 										  *
*			6. get_answer_from_server              						      *
*			7. check_validation_of_backup_operation 						  *
*			8. send_choice_to_server 										  *
*			9. get_disk_size_from_server 									  *
*			10. send_message_to_server 										  *
*			11. backup_file_c_style_client                  				  *
*           12. backup_file_unix_style_client             					  *
*           13. backup_file_mmap_client                    					  *
*           14. display_greeting_message                   					  *
* 																			  *
*			General: 														  *
*			-------- 														  *
*			1. is_input_valid 												  *
*			2. string_to_lowercase											  *
*			3. isFileExists													  *
*			4. is_backup_valid 												  *
*			5. get_file_size 												  *
*			6. get_size_of_available_space 									  *
*			7. print_image 													  *
*									      									  *
******************************************************************************/




#include <stdio.h>
#ifndef _FUNCTIONS_H 
#define _FUNCTIONS_H

/* Constant Variables*/

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define SERVER_MIN_ARGS 2
#define CLIENT_MIN_ARGS 3
#define MAX_NUMBER_OF_CLIENTS 3
#define NAME "socket"
#define SOCK_PATH  "socket"
#define MAX_LEN 128
#define BSIZE 1048576


/*~*~*~*~*~*~*~*~*~*~*~*~*/
/* Function declarations */
/*~*~*~*~*~*~*~*~*~*~*~*~*/


   /*********************/
  /* General Functions */
 /*********************/

int is_input_valid(const char test, const char min, const char max);
void string_to_lowercase(char *str);
int isFileExists(char* filename);
int is_backup_valid(float disk_size, float file_size);
float get_file_size(char* filename);
float get_size_of_available_space(void);
void print_image(FILE* fptr);


   /********************/
  /* Server Functions */
 /********************/

void send_greeting_message_to_client(int socket);
void ipv4_setup_server(void);
void ipv6_setup_server(void);
void unix_domain_setup_server(void);
void create_backup_folder(void);
void show_menu_for_client(int socket);
char get_client_choice(int socket);
void send_message_to_client(int socket, char* message, int size_to_send);
void response_after_selecting_on_the_menu(int socket, const char user_choice);
void send_disk_size_to_client(int socket);
void backup_file_c_style_server(int socket);
void backup_file_unix_style_server(int socket);
void backup_file_mmap_server(int socket);


/*********************/
/* Client Functions */
/*******************/

void ipv4_setup_client(char* filename);
void ipv6_setup_client(char* filename);
void unix_domain_setup_client(char* filename);
char* get_message_from_server(int socket);
void get_answer_from_server(int socket, char *user_answer);
void check_validation_of_backup_operation(int socket, char* filename);
void send_choice_to_server(int socket, char client_choice, char* filename);
float get_disk_size_from_server(int socket);
void get_client_answer(char *user_answer);
void send_message_to_server(int socket, char* message, int size_to_send);
void backup_file_c_style_client(int socket, char* filename);
void backup_file_unix_style_client(int socket, char* filename);
void backup_file_mmap_client(int socket, char* filename);
void display_greeting_message(int socket);

#endif
