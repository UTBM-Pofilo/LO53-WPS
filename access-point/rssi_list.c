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
	/** structure timeval:
	 *		time_t tv_sec: represents the number of whole seconds of elapsed time
	 *		long int yv_usec: the rest of the elapsed time (fraction of a second) represented as the number of microseconds
	 */
	struct timeval timer;
	unsigned long long ull_timer = 0;
	// get current time (function in sys/time.h)
	gettimeofday(&timer, NULL);
	ull_timer = timer.tv_sec;
	
	// if the list is not empty
	if(list != NULL && list->head != NULL) {
		// go through the RSSI list
		Rssi_sample * currentElem = list->head;
		Rssi_sample * prevElem = NULL;
		// while we have elements
		while(currentElem != NULL) {
			// check if the current element deadline is over and update the list accordingly
			if(currentElem->deadline < ull_timer) {
				// the list contains only one element
				if(currentElem == list->head && currentElem->next == NULL) {
					free(currentElem);
					list->head = NULL;
					list->tail = NULL;
					currentElem = NULL;
				}
				// we need to delete the first element
				else if(currentElem == list->head) {
					// reassign the head pointer
					list->head = currentElem->next;
					// free the element
					free(currentElem);
					// reassign the pointer
					currentElem = list->head;							
				}
				// we need to delete the last element
				else if(currentElem == list->tail) {
					// reassign the tail pointer
					list->tail = prevElem;
					list->tail->next = NULL;
					// free the element
					free(currentElem);
					// reassign the pointer
					currentElem = NULL;
				//otherwise
				} else {
					// link the previous element to the next element
					prevElem->next = currentElem->next;
					// free the element
					free(currentElem);
					// reassign the current element to the next element
					currentElem = prevElem->next;
				}
			} else {
				// deadline ok, we get the next element
				prevElem = currentElem;
				currentElem = currentElem->next;
			}
		}
	}
	return;
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
	// we consider the list can't be empty 
	// TODO? add a value even if the list is NULL (-> create the list)
	if(list == NULL) {
		return;
	}
	
	// init new rssi_sample
	Rssi_sample * new_rssi_sample = NULL;
	new_rssi_sample = (Rssi_sample*) malloc(sizeof(Rssi_sample));
	
	// add the rssi value
	new_rssi_sample->rssi_mW = value;
	
	// add the deadline
	struct timeval timer;
	gettimeofday(&timer, NULL);
	new_rssi_sample->deadline = timer.tv_sec;
	
	new_rssi_sample->next = NULL;
	
	// add the sample to the list
	// if the list is empty we just assign the head to the element
	if(list->head == NULL) {
		list->head = new_rssi_sample;
		list->tail = list->head;
	// reassign the pointer of the last element to the new element
	} else {
		list->tail->next = new_rssi_sample;
		list->tail = new_rssi_sample;
	}
	
	return;
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
	// TODO: check that the mac_value is not already in the list (in order to be sure that a mac address is in the list only once)
	int i = 0;
	// init new element
	Element * element = NULL;
	element = (Element*) malloc(sizeof(Element));
	for(i = 0; i < 6; i++) {
		element->mac_addr[i] = mac_value[i];
	}
	element->measurements.head = NULL;
	element->measurements.tail = NULL;
	element->next = NULL;
	
	// add the element to the list
	if(*list != NULL) {
		Element * temp = *list;
		// go to last element
		while(temp->next != NULL) {
			temp = temp->next;
		}
		// add the new element at the end
		temp->next = element;
	} else {
		*list = element;
	}
	
	// return the list head pointer
	return *list;
}

// we assume each that mac addresses are unique in the list
void delete_element(Element ** list, Element * e) {
	// check that list and element are not null
	if(*list == NULL || e == NULL) {
		return;
	}
	
	Element * temp = *list;
	
	// if the element to delete is the first of the list
	if((*list)->mac_addr == e->mac_addr) {
		*list = temp->next;
		// clear rssi list before deleting element
		Rssi_sample * rssi_temp;
		*rssi_temp = *temp->measurements.head;
		while(rssi_temp != temp->measurements.tail) {
			temp->measurements.head = rssi_temp->next;
			free(rssi_temp);
			rssi_temp = temp->measurements.head;
		}
		// deal with the last element
		free(temp->measurements.tail);
		temp->measurements.head = NULL;
		temp->measurements.tail = NULL;
		// delete the element
		free(temp);
		return;
	}
	
	while(temp->next != NULL && temp->next->mac_addr != e->mac_addr) {
		temp = temp->next;
	}
	
	// we check that the element we want to delete well exists
	if(temp->next != NULL) {
		// temp is now the element before the one we want to delete
		// we want to get the element after the one we want to delete in order to remake the links in the list
		Element * temp2 = temp->next->next;
		
		// clear rssi list before deleting element
		Rssi_sample * rssi_temp;
		*rssi_temp = *temp->measurements.head;
		while(rssi_temp != temp->next->measurements.tail) {
			temp->next->measurements.head = rssi_temp->next;
			free(rssi_temp);
			rssi_temp = temp->next->measurements.head;
		}
		// deal with the last element
		free(temp->next->measurements.tail);
		temp->next->measurements.head = NULL;
		temp->next->measurements.tail = NULL;
		// delete the element
		free(temp->next);
		
		// remake the links in the list
		temp->next = temp2;
	} 
	return;
}

void clear_empty_macs(Element ** list) {
	Element * temp = *list;
	Element * temp2 = NULL;
	// we go through the list until NULL
	while(temp != NULL) {
		// if the current element has an empty rssi sample list
		if(temp->measurements.head == NULL && temp->measurements.tail == NULL) {
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
	}
	return;
}

// Communications functions
char * build_element(Element * e, char * buf) {
	double sum = 0.0;
	double mean = 0.0;
	int nb_samples = 0;
	char mac[17];
	mac_to_string(e->mac_addr, mac);
	
	// compute the mean
	Rssi_sample * temp = e->measurements.head;
	while(temp != NULL) {
		// for each sample of the device
		nb_samples++;
		sum += temp->rssi_mW;
		temp = temp->next;
	}
	mean = sum / nb_samples;
	
	// print for debug
	if(nb_samples > 0) {
		sprintf(buf, "{\"%s\":\"%.2f\",\"samples\":\"%d\"}", mac, mean, nb_samples);
	}
	
	return buf;
	
}

char * build_buffer(Element * list, char * buffer, char * my_name, u_char * macs_requested, unsigned short nb_macs) {
	// TODO
}

char * build_buffer_full(Element * list, char * buffer, char * my_name) {
	// TODO
}
