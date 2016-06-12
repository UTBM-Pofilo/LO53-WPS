#include "http.h"

void error(const char *msg, int *success, int sockfd) { 
	close(sockfd);
	perror(msg);
	*success = -1;
}

char* http_request(char *host, char *message, int *success) {
	struct hostent *server;
	struct sockaddr_in serv_addr;
	int sockfd, bytes, sent, received, total;
	char response[RESPONSE_SIZE];

	time_t t;
	time(&t);

	printf("\n%s | Request:%s", ctime(&t), message);

	/* create the socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket", success, sockfd);
		return strdup(response);
	}

	/* lookup the ip address */
	server = gethostbyname(host);
	if (server == NULL) {
		error("ERROR, no such host", success, sockfd);
		return strdup(response);
	}

	/* fill in the structure */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr, server->h_length);

	/* connect the socket */
	if (connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	        error("ERROR connecting", success, sockfd);
		return strdup(response);
	}

	/* send the request */
	total = strlen(message);
	sent = 0;
	do {
		bytes = write(sockfd, message+sent, total-sent);
		if (bytes < 0) {
			error("ERROR writing message to socket", success, sockfd);
			return strdup(response);
		}		
		if (bytes == 0) {
			break;
		}
		sent += bytes;
	} while (sent < total);

	/* receive the response */
	memset(response, 0, sizeof(response));
	total = sizeof(response) - 1;
	received = 0;
	do {
		bytes = read(sockfd, response + received, total - received);
		if (bytes < 0) {
			error("ERROR reading response from socket", success, sockfd);
			return strdup(response);
		}		
		if (bytes == 0) {
			break;
		}	
		received += bytes;
	} while (received < total);

	if (received == total) {
		error("ERROR storing complete response from socket", success, sockfd);
		return strdup(response);
	}

	/* close the socket */
	close(sockfd);

	return strdup(response);
}

void send_rssi_to_server(int rssi, char * mac_string) {
	char message[MESSAGE_SIZE];
	char *response = "";
	char *host = HOST;
	char *message_fmt = "GET /api/gotcha/%d/%s/%d HTTP/1.0\r\n\r\n";
	int value = 0;
	int *success = NULL;
	success = &value;
		
	sprintf(message, message_fmt, AP_ID, mac_string, rssi);

	response = http_request(host, message, success);
	if (*success == 0) {
		printf("\nResponse:\n%s",response);
	} else {
		printf("\nImpossible to send the rssi value to the server\n");
	}
	
}


 
