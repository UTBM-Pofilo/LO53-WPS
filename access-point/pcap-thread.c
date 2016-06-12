#include "pcap-thread.h"

volatile sig_atomic_t got_sigint;
sem_t synchro;

void pcap_function(void) {
	char *iface = IFACE;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t * handle = NULL;
	struct ieee80211_radiotap_header * rtap_head;
	struct ieee80211_header * eh;
	struct pcap_pkthdr header;
	const u_char * packet;
	u_char * mac;
	u_char first_flags;
	int offset = 0;
	char rssi;

	// Open pcap handle to sniff traffic
	handle = pcap_open_live(iface, BUFSIZ, 1, 1000, errbuf);
	
	if (handle == NULL) {
		printf("Could not open pcap on %s\n", iface);
		pthread_exit((void *) -1);
	}

	sem_post(&synchro);
	while (got_sigint == 0) {
		packet = pcap_next(handle, &header);

		if (!packet) {
			continue;
		}

		rtap_head = (struct ieee80211_radiotap_header *) packet;
		int len = (int) rtap_head->it_len[0] + 256 * (int) rtap_head->it_len[1];
		eh = (struct ieee80211_header *) (packet + len);

		if ((eh->frame_control & 0x03) == 0x01) {
			mac = eh->source_addr;
			first_flags = rtap_head->it_present[0];
			offset = 8;
			offset += ((first_flags & 0x01) == 0x01) ? 8 : 0 ;
			offset += ((first_flags & 0x02) == 0x02) ? 1 : 0 ;
			offset += ((first_flags & 0x04) == 0x04) ? 1 : 0 ;
			offset += ((first_flags & 0x08) == 0x08) ? 4 : 0 ;
			offset += ((first_flags & 0x10) == 0x10) ? 2 : 0 ;
			rssi = *((char *) rtap_head + offset) - 0x100;
			//printf("%d bytes -- %02X:%02X:%02X:%02X:%02X:%02X -- RSSI: %d dBm\n",
			//       len, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], (int) rssi);
			// We got some message issued by a terminal (FromDS=0,ToDS=1)

			char mac_string[MAC_LENGTH];
			mac_to_string(mac, mac_string);

			if(check_macs(mac_string) == 0) {
				printf("\n-----> mac: %s / rssi: %d", mac_string, rssi);
				send_rssi_to_server((int) rssi, mac_string);
			} else {
				printf("\n> mac: %s / rssi: %d", mac_string, rssi);
			}

		}
	}
	pcap_close(handle);
	pthread_exit((void *) 0);
}


