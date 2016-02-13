
#include "intersect_funcs.h"

#include <stdio.h>

void ll_print(struct h_llist *h, size_t file_count) 
{
	while(h) {
		if(h->value == file_count) {
			printf("%zd %s\n", h->value, h->key); //change to fprintf(stdout
		}
		h = h->next;
	}
}
