/*!
 * \file app.c
 * \brief File that contains the main of the application
 * \author vmerat
 * \version 0.1
 * \date 16/06/16
 */
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>

#include "mac.h"
#include "pcap-thread.h"
#include "http.h"

/*!
 * \fn int main(void)
 * \brief Entrance of the program 
 * \return an int corresponding to the success of the function or not
 */
int main(void) {
	
	pcap_t *handle;
	struct bpf_program fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	char *filter_exp = "";
	char *iface = IFACE;

	bpf_u_int32 net = PCAP_NETMASK_UNKNOWN;	

	/* Open the session */
	handle = pcap_open_live(iface, BUFSIZ, 1, 1000, errbuf);
	
	if (handle == NULL) {
		printf("Could not open pcap on %s\n", iface);
		return 2;
	}

	/* Comply and set the filter */
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return 2;
	}
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return 2;
	}

	printf("\nlaunched ...\n");

	/* Call pcap_function for each packet captured by pcaplib */
	pcap_loop(handle, 0, pcap_function, NULL);

	/* Close session */
	pcap_close(handle);

	return 0;
}



