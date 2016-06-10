#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include "rssi_list.h"


#define PORT 80
#define MESSAGE_SIZE 1024
#define RESPONSE_SIZE 4096
#define MAC_LENGTH 17

char* http_request(char *host, char *message_fmt);
void error(const char *msg);
void send_rssi_to_server(Element ** list, u_char * mac_value);


