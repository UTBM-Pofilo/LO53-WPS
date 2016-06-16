/*!
 * \file pcap-thread.h
 * \brief HTTP Stuff to deal with packets catched by libpcap
 * \author vmerat
 * \version 0.1
 * \date 16/06/16
 */
#ifndef _PCAP_THREAD_H_
#define _PCAP_THREAD_H_

#include "mac.h"
#include "http.h"
#include <pcap.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*!
 * \struct ieee80211_header
 * \brief IEEE 802.11 Header
 * Header of protocol IEEE 802.11
 */
struct ieee80211_header {
  u_short frame_control;
  u_short frame_duration;
  u_char recipient[6];
  u_char source_addr[6];
  u_char address3[6];
  u_short sequence_control;
  u_char address4[6];
};

/*!
 * \struct ieee80211_radiotap_header
 * \brief IEEE 802.11 Radiotap Header
 * Header of radiotap protocol IEEE 802.11
 */
struct ieee80211_radiotap_header {
  u_char it_version;
  u_char it_pad;
  u_char it_len[2];
  u_char it_present[4];
};

/*!
 * \enum ieee80211_radiotap_type
 * \brief IEEE 802.11 Radiotap Flags
 * Flags of the radiotap header
 * Taken from http://osxr.org:8080/linux/source/include/net/ieee80211_radiotap.h
 */
enum ieee80211_radiotap_type {
	IEEE80211_RADIOTAP_TSFT = 0,
	IEEE80211_RADIOTAP_FLAGS = 1,
	IEEE80211_RADIOTAP_RATE = 2,
	IEEE80211_RADIOTAP_CHANNEL = 3,
	IEEE80211_RADIOTAP_FHSS = 4,
	IEEE80211_RADIOTAP_DBM_ANTSIGNAL = 5,
	IEEE80211_RADIOTAP_DBM_ANTNOISE = 6,
	IEEE80211_RADIOTAP_LOCK_QUALITY = 7,
	IEEE80211_RADIOTAP_TX_ATTENUATION = 8,
	IEEE80211_RADIOTAP_DB_TX_ATTENUATION = 9,
	IEEE80211_RADIOTAP_DBM_TX_POWER = 10,
	IEEE80211_RADIOTAP_ANTENNA = 11,
	IEEE80211_RADIOTAP_DB_ANTSIGNAL = 12,
	IEEE80211_RADIOTAP_DB_ANTNOISE = 13,
	IEEE80211_RADIOTAP_RX_FLAGS = 14,
	IEEE80211_RADIOTAP_TX_FLAGS = 15,
	IEEE80211_RADIOTAP_RTS_RETRIES = 16,
	IEEE80211_RADIOTAP_DATA_RETRIES = 17,

	IEEE80211_RADIOTAP_MCS = 19,
	IEEE80211_RADIOTAP_AMPDU_STATUS = 20,
	IEEE80211_RADIOTAP_VHT = 21,

	/* valid in every it_present bitmap, even vendor namespaces */
	IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE = 29,	
	IEEE80211_RADIOTAP_VENDOR_NAMESPACE = 30,	
	IEEE80211_RADIOTAP_EXT = 31					
};
/*!
 * \struct radiotap_align_size
 * \brief Alignements and size of the radiotap header field
 */
struct radiotap_align_size {		
	uint8_t align;
	uint8_t size;
};

/*!
 * \fn void pcap_function(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet)
 * \brief Function called when a packet is captured by libpcap 
 * \return
 * \param args arguments sent by libpcap
 * \param header an already allocated pcap_pkthdr that contains the timestamp and the size of the packet
 * \param packet a pointer to the packet itself
 */
void pcap_function(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet);



#endif /* _PCAP_THREAD_H_ */

