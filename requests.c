#include "requests.h"
#include "helpers.h"
#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

char *compute_get_request(char *host, char *url, char *cookie, char *token) {
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));

  // Step 1: write the method name, URL, request params (if any) and protocol
  // type
  sprintf(line, "GET %s HTTP/1.1", url);
  compute_message(message, line);

  // Step 2: add the host
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  // check the cookie
  if (cookie != NULL) {
    memset(line, 0, LINELEN);
    sprintf(line, "Cookie: %s", cookie);
    compute_message(message, line); 
  }

  // check the token
  if (token != NULL) {
     memset(line, 0, LINELEN);
      sprintf(line, "Authorization: Bearer %s", token);
      compute_message(message, line);
  }

  // Step 4: add final new line
  compute_message(message, "");
  free(line);
  return message;
}

char *compute_post_request(char *host, char *url, char *token, char *content_type,
                           char *body_data, int body_data_fields_count) {
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));
  
  // Step 1: write the method name, URL and protocol type
  sprintf(line, "POST %s HTTP/1.1", url);
  compute_message(message, line);

  // Step 2: add the host
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  sprintf(line, "Content-Type: %s", content_type);
  compute_message(message, line);
  
  sprintf(line, "Content-Length: %lu", strlen(body_data));
  compute_message(message, line);

 // add authorization JWT
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

  // Step 5: add new line at end of header
  compute_message(message, "");

  // Step 6: add the actual payload data
  memset(line, 0, LINELEN);
  strcat(message, body_data);

  free(line);

  return message;
}

char *compute_delete_request(char *host, char *url, char *cookie, char *token) {
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));

  // Step 1: write the method name, URL, request params (if any) and protocol
  // type
  sprintf(line, "DELETE %s HTTP/1.1", url);
  compute_message(message, line);

  // Step 2: add the host
  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  if (cookie != NULL) {
    memset(line, 0, LINELEN);
    sprintf(line, "Cookie: %s", cookie);
    compute_message(message, line); 
  }

  if (token != NULL) {
     memset(line, 0, LINELEN);
      sprintf(line, "Authorization: Bearer %s", token);
      compute_message(message, line);
  }

  // Step 4: add final new line
  compute_message(message, "");
  free(line);
  return message;
}