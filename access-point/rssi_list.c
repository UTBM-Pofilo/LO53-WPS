#include "rssi_list.h"

// General functions
u_char * string_to_mac(char * buf, u_char * byte_mac) {
	sscanf(buf, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &byte_mac[0], &byte_mac[1],&byte_mac[2],&byte_mac[3],&byte_mac[4],&byte_mac[5]);
	return byte_mac;
}

char * mac_to_string(u_char * byte_mac, char * buf) {
	sprintf(buf, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", byte_mac[0], byte_mac[1], byte_mac[2], byte_mac[3], byte_mac[4], byte_mac[5]);
	return buf;
}

void clear_outdated_values(Deque * list) {
	// TODO
}

void clear_values(Deque * list) {
	// check if the list is not already empty
	if(list != NULL && list->head != NULL) {
		// while there is still elements at the head of the list	
		while(list->head != NULL) {
			// create pointer on the second element
			Rssi_sample * temp = list->head->next;
			// clear the head of the list
			free(list->head);
			// make that the list point on the saved elements
			list->head = temp;
		}
	}
	// the tail points to the head as there is not element in the list anymore 
	list->tail = list->head;
	return;
}

void add_value(Deque * list, int value) {
	// TODO
}

// Element functions
void clear_list(Element ** list) {
	// check if the list is not already empty
	if(*list != NULL) {
		// while there is still elements at the head of the list	
		while(*list != NULL) {
			// create pointer on the next element
			Element * temp = (*list)->next;
			// clear the measurements
			clear_values(&(*list)->measurements);
			free(*list);
			// make that the list point on the saved element
			*list = temp;
		}
	}
	return;
}

Element * find_mac(Element * list, u_char * mac_value) {
	// if the list is empty, we don't have the mac address
	if(list != NULL) {
		Element * temp = list;
		// we go through the list until finding the mac address or being at the end of the list
		while((temp->mac_addr != mac_value) && (temp->next != NULL)) {
			temp = temp ->next;
		}
		// we check the reason why we left the loop
		if(temp->mac_addr != mac_value) {
			return NULL;
		} else {
			return temp;
		}
	} else {
		return NULL;
	}
}

Element * add_element(Element ** list, u_char * mac_value) {
	// TODO
}

void delete_element(Element ** list, Element * e) {
	// TODO
}

void clear_empty_macs(Element ** list) {
	Element * temp = *list;
	Element * temp2 = NULL;
	// we go through the list until NULL
	while(temp != NULL) {
		// if the current element has an empty rssi sample list
		if(pTmp->measurements.head == NULL && pTmp->measurements.tail == NULL) {
			// store next element
			temp2 = temp->next;
			// delete element from the list
			delete_element(list, temp);
			// restore the element with the next one
			temp = temp2;
		} else {
			// go to next element
			temp = temp->next;
	}
	return;
}

// Communications functions
char * build_element(Element * e, char * buf) {
	// TODO
}

char * build_buffer(Element * list, char * buffer, char * my_name, u_char * macs_requested, unsigned short nb_macs) {
	// TODO
}

char * build_buffer_full(Element * list, char * buffer, char * my_name) {
	// TODO
}