#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include "buffer.h"

// function to read the username and password
// from input and return 1 in case a space is found 
int read_input_reg(char *type, char *data) {
	printf("%s=", type);
	memset(data, 0, BUFLEN - 1);
	fgets(data, BUFLEN - 1, stdin);

	for (int i = 0; i < strlen(data); i++) {
		if (data[i] == ' ') {
			return 1;
		}
	}
	return 0;
}

// function to read any (char *)data from input
void read_input(char *type, char *data) {
	printf("%s=", type);
	memset(data, 0, BUFLEN - 1);
	fgets(data, BUFLEN - 1, stdin);
}

// error messages for register
void check_register(char *response, char *username) {
	if (strstr(response, "error")) {
		printf("The username: %s is taken.\n", username);
	} else {
		printf("User: %s successfully registered!\n", username);
	}
}

// function that creates the json object that will be sent to server
char *create_obj(char *username, char *password, JSON_Value *root_value) {
	JSON_Object *root_object = json_value_get_object(root_value);
	char *converted_obj = NULL;

	json_object_set_string(root_object, "username", username);
	json_object_set_string(root_object, "password", password);
	converted_obj = json_serialize_to_string_pretty(root_value);

	return converted_obj;
}

// error messages for login
// in case of succes the cookie is extracted 
char *check_login(char *response, char *username) {
	char *err = strstr(response, "error");
	if (err) {
		if (strstr(err, "Credentials")) {
			printf("The credentials don't match.\n");
		} else {
			printf("The username: %s is not registered.\n", username);
		}
	} else {
		printf("User: %s successfully logged in!\n", username);
		char *cookie = strstr(response, "Set-Cookie: ");
		if (!err) {
			cookie = strtok(cookie, ";");
			cookie = strtok(cookie, " ");
			cookie = strtok(NULL, " ");
		}
		return cookie;
	}
	return NULL;
}

// checks if logout was done properly
void check_logout(char *response) {
	if (!strstr(response, "error")) {
		printf("success!");
	}
}

// error messages for entry_library
// in cas of succes the token is extracted
char *check_entry(char *response) {
	if (strstr(response, "error")) {
		printf("You are not logged in!\n");
	} else {
		printf("You have access to the library!\n");
		char *jwt = strstr(response, "token");
		if (jwt) {
			jwt = strtok(jwt, "\"");
			jwt = strtok(NULL, "\"");
			jwt = strtok(NULL, "\"");
		}
		return jwt;
	}
	return NULL;
}

// extracts the error message from the response
void only_response(char *response) {
	char *error = strstr(response, "{");
	if (error != NULL) {
		puts(error);
	} else {
		printf("success!");
	}
}

// extracts the error message from the response
// for the get_books command
void arr_response(char *response) {
	char *error = strstr(response, "{");
	char *resp = strstr(response, "[");

	if (error != NULL) {
		puts(error);
	} else {
		puts(resp);
	}
}

int main(int argc, char *argv[]) {
	char *message;
	char *response;
	char command[BUFLEN], username[100], password[100], title[100],
		author[100], genre[100], publisher[100], id[100];

	int sockfd;
	int page_count;
	char *cookie = NULL, *jwt = NULL;

	while (1) {
		memset(command, 0, BUFLEN);
		fgets(command, BUFLEN, stdin);

		if (strcmp(command, REGISTER) == 0) {

			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");
			int m, n;
			m = read_input_reg("username", username);
			n = read_input_reg("password", password);

			// checking for spaces
			if (m == 1 || n == 1) {
				printf("Spaces not allowed!\n");
				continue;
			}

			JSON_Value *root_value = json_value_init_object();
			char *converted_obj = create_obj(username, password, root_value);

			message = compute_post_request(HOST, URL_REGISTER, NULL,
				"application/json",	converted_obj, 0);

			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);

			check_register(response, username);

			free(message);
			free(response);
			json_free_serialized_string(converted_obj);
			json_value_free(root_value);
			close_connection(sockfd);

		} else if (strcmp(command, LOGIN) == 0) {
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");

			int m, n;
			m = read_input_reg("username", username);
			n = read_input_reg("password", password);

			// checking for spaces
			if (m == 1 || n == 1) {
				printf("Spaces not allowed!\n");
				continue;
			}

			JSON_Value *root_value = json_value_init_object();
			char *converted_obj = create_obj(username, password, root_value);

			message = compute_post_request(HOST, URL_LOGIN, NULL,
				"application/json", converted_obj, 0);

			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);

			cookie = check_login(response, username);

			free(message);
			json_free_serialized_string(converted_obj);
			json_value_free(root_value);
			close_connection(sockfd);

		} else if (strcmp(command, ENTER_LIBRARY) == 0) {
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");

			message = compute_get_request(HOST, URL_ACCESS, cookie, jwt);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			jwt = check_entry(response);

			free(message);
			close_connection(sockfd);
		} else if (strcmp(command, GET_BOOKS) == 0) {
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");

			message = compute_get_request(HOST, URL_BOOKS, NULL, jwt);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			arr_response(response);
			free(response);
			free(message);
			close_connection(sockfd);

		} else if (strcmp(command, GET_BOOK) == 0) {
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");

			// creating the url with the given id
			read_input("id", id);
			char url[1000];
			strcpy(url, "/api/v1/tema/library/books/");
			strcat(url, id);
			url[strlen(url) - 1] = 0;

			message = compute_get_request(HOST, url, NULL, jwt);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);

			only_response(response);

			free(response);
			free(message);
			close_connection(sockfd);

		} else if (strcmp(command, ADD_BOOK) == 0) {
			// deleting the "/n" for every string input
			read_input("title", title);
			title[strlen(title) - 1] = 0;
			read_input("author", author);
			author[strlen(author) - 1] = 0;
			read_input("genre", genre);
			genre[strlen(genre) - 1] = 0;
			read_input("publisher", publisher);
			publisher[strlen(publisher) - 1] = 0;
			printf("page_count=");
			int n;
			n = scanf("%d", &page_count);

			// if page_count is anything but a string or any of the other
			// fields is empty then the book doesn't respect the right format
			if (n != 1 || strcmp(title, "") == 0 || strcmp(author, "") == 0  ||
				strcmp(genre, "") == 0 || strcmp(publisher, "") == 0 ) {
				printf("The books doesn't respect the specified format!\n");
				continue;
			}

			JSON_Value *root_value = json_value_init_object();
			JSON_Object *root_object = json_value_get_object(root_value);
			char *converted_obj = NULL;

			json_object_set_string(root_object, "title", title);
			json_object_set_string(root_object, "author", author);
			json_object_set_string(root_object, "genre", genre);
			json_object_set_number(root_object, "page_count", page_count);
			json_object_set_string(root_object, "publisher", publisher);
			converted_obj = json_serialize_to_string_pretty(root_value);

			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");

			message = compute_post_request(HOST, URL_BOOKS, jwt,
				"application/json",	converted_obj, 0);

			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			
			only_response(response);

			free(response);
			free(message);
			json_free_serialized_string(converted_obj);
			json_value_free(root_value);
			close_connection(sockfd);

		} else if (strcmp(command, DELETE_BOOK) == 0) {
			read_input("id", id);

			// creating the url with the given id
			char url[1000];
			strcpy(url, "/api/v1/tema/library/books/");
			strcat(url, id);
			url[strlen(url) - 1] = 0;
			
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");

			message = compute_delete_request(HOST, url, NULL, jwt);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);

			only_response(response);

			free(response);
			free(message);
			close_connection(sockfd);
		} else if (strcmp(command, LOGOUT) == 0) {
			printf("Logging out...\n");
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			DIE(sockfd == -1, "error\n");

			message = compute_get_request(HOST, URL_LOGOUT, cookie, NULL);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);

			only_response(response);
			cookie = NULL;
			jwt = NULL;
			free(response);
			free(message);
			close_connection(sockfd);

		} else if (strcmp(command, EXIT) == 0) {
			break;
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
