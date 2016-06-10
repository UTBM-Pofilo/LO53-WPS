#include "http.h"

void error(const char *msg) { 
	perror(msg); 
	exit(0); 
}

char* http_request(char *host, char *message) {

	struct hostent *server;
	struct sockaddr_in serv_addr;
	int sockfd, bytes, sent, received, total;
	char response[RESPONSE_SIZE];

	// sprintf(message, message_fmt, "a", "b");
	printf("Request:\n%s\n",message);

	/* create the socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	/* lookup the ip address */
	server = gethostbyname(host);
	if (server == NULL) {
		error("ERROR, no such host");
	}

	/* fill in the structure */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr, server->h_length);

	/* connect the socket */
	if (connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	        error("ERROR connecting");
	}

	/* send the request */
	total = strlen(message);
	sent = 0;
	do {
		bytes = write(sockfd, message+sent, total-sent);
		if (bytes < 0) {
			error("ERROR writing message to socket");
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
			error("ERROR reading response from socket");
		}		
		if (bytes == 0) {
			break;
		}	
		received += bytes;
	} while (received < total);

	if (received == total) {
		error("ERROR storing complete response from socket");
	}

	/* close the socket */
	close(sockfd);

	/* process response */
	// printf("Response:\n%s\n",response);

	return strdup(response);
}


void send_rssi_to_server(Element ** list, u_char * mac_value) {
	char message[MESSAGE_SIZE];
	char *response = "";
	char *host = "www.google.fr";
	char *message_fmt = "GET /api/gotcha/%s/%f HTTP/1.0\r\n\r\n";

	// deal with the mac address
	char buf[MAC_LENGTH];
	mac_to_string(mac_value, buf);

	// deal with the rssi value

	Rssi_sample * temp = (*list)->measurements.head;
	while(temp != NULL) {
		double rssi_value= temp->rssi_mW;
		temp = temp->next;		
		sprintf(message, message_fmt, *buf, rssi_value);

		response = http_request(host, message);
		printf("Response:\n%s\n",response);
	}

	free(response);
}




