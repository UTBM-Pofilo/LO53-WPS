/*!
 * \file mac.h
 * \brief Usefull MAC Functions
 * \author vmerat
 * \version 0.1
 * \date 16/06/16
 */
#ifndef _MAC_H_
#define _MAC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <arpa/inet.h>
#include "http.h"

#ifndef IFACE
	#define IFACE ((char *)"wlan0")
#endif

#ifndef AP_ID
	#define AP_ID 1
#endif
#ifndef HOST
	#define HOST ((char *)"192.168.1.42")
#endif
#define PORT 80
#define MAC_LENGTH 17


/*!
 * \fn u_char *string_to_mac(char * buf, u_char * byte_mac)
 * \brief Function string_to_mac converts a human-readable MAC to its binary counterpart.
 * \return the 6-bytes binary MAC
 * \param buf the buffer containing the MAC string
 * \param byte_mac a 6-byte buffer to store the result. byte_mac is returned by the function.
 */
u_char *string_to_mac(char * buf, u_char * byte_mac);

/*!
 * \fn char * mac_to_string(u_char * byte_mac, char * buf)
 * \brief mac_to_string opposite function to string_to_mac.
 * Takes a binary MAC address (such as extracted from IEEE802.11 header by libpcap)
 * and converts it to a human-readable string.
 * \return the string MAC
 * \param byte_mac the binary MAC
 * \param buf an already allocated char buffer, returned by the function.
 */
char * mac_to_string(u_char * byte_mac, char * buf);

/*!
 * \fn int check_macs(char* mac_string)
 * \brief call an http request to the server to get the list of mac address to deal with
 * Check if the mac address in parameter is in the list.
 * \return 0 if the mac is in the list, -1 else
 * \param mac_string an already allocated char buffer
 */
int check_macs(char* mac_string);


#endif /* _MAC_H_ */

