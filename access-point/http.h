#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <time.h>
#include "util.h"

#define MESSAGE_SIZE 1024
#define RESPONSE_SIZE 4096

/*!
 * \Send an http request to the host and the message in parameter 
 * \return response an already allocated char buffer
 * \param host an already allocated char buffer
 * \param message an already allocated char buffer
 * \param success which will be set at -1 if an error occured
 */
char* http_request(char *host, char *message, int *success);

/*!
 * \print the message on the screen, close the socket and update the success value
 * \return
 * \param msg an already allocated char buffer
 * \param success an already allocated int
 * \param sockfd corresponding to the socket we need to close
 */
void error(const char *msg, int *success, int sockfd);

/*!
 * \Send to the server the rssi value associated with the mac address 
 * \return
 * \param rssi an int corresponding to the rssi value
 * \param mac_string an already allocated char buffer
 */
void send_rssi_to_server(int rssi, char * mac_string);




