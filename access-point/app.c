#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "pcap-thread.h"
#include "http.h"


int main(void) {
	pthread_t thread_pcap;

	pthread_create(&thread_pcap, NULL, (void * (*)(void *))pcap_function, NULL);

	pthread_join(thread_pcap, NULL);

	return 0;
}



