#include "pcap-thread.h"

// from https://www.google.fr/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=0ahUKEwjwyMTz-6fNAhWGAxoKHUrvCFwQFgggMAA&url=ftp%3A%2F%2Fftp.fau.de%2Fnetbsd%2FNetBSD-current%2Fsrc%2Fexternal%2Fbsd%2Fwpa%2Fdist%2Fsrc%2Futils%2Fradiotap.c&usg=AFQjCNGG9M2y3NzJy335dUf4cpfmBWgvAQ&cad=rja
const struct radiotap_align_size radiotap_field_sizes[] = { { 8, 8 }, { 1, 1 }, { 1, 1 }, { 2, 4 }, { 2, 2 }, { 1, 1 }, { 1, 1 }, { 2, 2 }, { 2, 2 }, { 2, 2 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 }, { 2, 2 }, { 2, 2 }, { 1, 1 }, { 1, 1 }, { 0, 0 }, { 1, 3 }, { 4, 8 }, { 2, 12 }                    
	
	/*
	* add more here as they are defined in		
	* include/net/ieee80211_radiotap.h		
	*/		
};

void pcap_function(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet) {

	/* Headers of the packet */
	struct ieee80211_radiotap_header * rtap_head;
	struct ieee80211_header * eh;

	/* The MAC address related to the packet */
	u_char * mac;
	/* Flags to decode to calculate the offset */
	unsigned long flags;
	int offset = 0;
	/* RSSI value extracted from the informations of the packet */
	char rssi;
	int field, id_flag = 0;

	rtap_head = (struct ieee80211_radiotap_header *) packet;
	int len  = (int) rtap_head->it_len[0] + 256 * (int) rtap_head->it_len[1];
	eh = (struct ieee80211_header *) (packet + len);

	if ((eh->frame_control & 0x03) == 0x01) {
		/* Get the MAC address and transform it into a human readable mac address */
		mac = eh->source_addr;
		char mac_string[MAC_LENGTH];
		mac_to_string(mac, mac_string);
		
		/* We check if the server want the RSSI value of this MAC address */
		if(check_macs(mac_string) == 0) {
			/* Initialize with the size of rtap_head */
			offset = 4;
		
			/* First iteration to know the flags we have to deal with */
			do {
				flags = *(((u_char*)rtap_head->it_present) + (id_flag*4)) | 
					*(((u_char*)rtap_head->it_present) + (id_flag*4+1)) << 8 |
					*(((u_char*)rtap_head->it_present) + (id_flag*4+2)) << 16 | 
					*(((u_char*)rtap_head->it_present) + (id_flag*4+3)) << 24;
				id_flag++;
			}while(flags & (1 << IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE));
			
			/* Update the offset with the size of the flags */
			offset += 4*id_flag;

			/* Reset id_flag to start again the iteration */
			id_flag = 0;
			do {
				flags = *(((u_char*)rtap_head->it_present) + (id_flag*4)) | 
					*(((u_char*)rtap_head->it_present) + (id_flag*4+1)) << 8 |
					*(((u_char*)rtap_head->it_present) + (id_flag*4+2)) << 16 | 
					*(((u_char*)rtap_head->it_present) + (id_flag*4+3)) << 24;
				
				/* Process for each field */
				for(field = IEEE80211_RADIOTAP_TSFT; field <= IEEE80211_RADIOTAP_VHT; field++) {
					if(flags & (1 << field)) {
						/* Update the offset */
						if((offset % radiotap_field_sizes[field].align) != 0) {
							offset += radiotap_field_sizes[field].align - (offset % radiotap_field_sizes[field].align);
						}
						/* Get the RSSI value if the field corresponds */
						if(field == IEEE80211_RADIOTAP_DBM_ANTSIGNAL) {
							rssi = *((unsigned char *) rtap_head + offset) - 0x100;
							printf("\nmac: %s | rssi: %d", mac_string, (int) rssi);
							send_rssi_to_server((int) rssi, mac_string);
						}
						
						offset += radiotap_field_sizes[field].size;
					}
				}
				id_flag++;
			}while(flags & (1 << IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE));
		} else {
			// printf("\nMAC address not found in the server: %s", mac_string);
		}
	}
}




