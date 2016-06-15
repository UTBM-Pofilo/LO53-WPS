#include "util.h"


u_char * string_to_mac(char * buf, u_char * byte_mac) {
	sscanf(buf, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &byte_mac[0], &byte_mac[1],&byte_mac[2],&byte_mac[3],&byte_mac[4],&byte_mac[5]);
	return byte_mac;
}

char * mac_to_string(u_char * byte_mac, char * buf) {
	sprintf(buf, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", byte_mac[0], byte_mac[1], byte_mac[2], byte_mac[3], byte_mac[4], byte_mac[5]);
	return buf;
}

int check_macs(char* mac_string) {
	char *response = NULL;
	char *host = HOST;
	/* The GET parameter we want to send */
	char *message = "GET /api/check 	HTTP/1.0\r\n\r\n";
	int value = 0;
	int *success;
	success = &value;

	response = http_request(host, message, success);
	// printf("\nResponse:%s", response);

	/* If the HTTP request is a success and mac_string is part of the response, then return 0 which means that we want the MAC */ 
	if (*success == 0 && strstr(response, mac_string) != NULL) {
		return 0;
	}
	return -1;
}


