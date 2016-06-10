#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */

#include "rssi_list.h"
#include "pcap-thread.h"
#include "http.h"


void communication(void);

int main(void) {
	pthread_t thread_pcap/*, thread_com*/;

	pthread_create(&thread_pcap, NULL, (void * (*)(void *))pcap_function, NULL);
	// pthread_create(&thread_com, NULL, (void * (*)(void *))communication, NULL);

	pthread_join(thread_pcap, NULL);
	// pthread_join(thread_com, NULL);

	return 0;
}

void communication(void) {

}



/*int main(int argc,char *argv[]) {
	char *response;
	
	// char *host = "http://trips.loc/api";
	char *host = "www.google.fr";
	char *message_fmt = "GET /arg1=%s&arg2=%s HTTP/1.0\r\n\r\n";

	response = http_request(host, message_fmt);

	printf("Response:\n%s\n",response);
	
	free(response);
	return 0;
}*/


