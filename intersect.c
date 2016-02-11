#define _BSD_SOURCE
#include "intersect.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

//taken from class code for hash program
static const size_t DEFAULT_SIZE = 10;
static struct h_llist *h_llist_create(const char *key, double value);
static void h_llist_destroy(struct h_llist *list);
static size_t hash_func(const char *key, size_t capacity);
static void hash_recalculate(hash *h);
uint64_t wang_hash(uint64_t key);

int main(int argc, char *argv[])
{
	if(argc == 1) {
		fprintf(stderr, "There were no files to read\n");
		return 1;
	}

	int file_count = 1;

	hash *hashy = hash_create();

	char *buffer = malloc(256 * sizeof(buffer)); //change to malloc later and realloc
	char *buffer1 = malloc(256 * sizeof(buffer1));

	while(file_count < argc) {
		FILE *fp = fopen(argv[file_count], "r");

		if(!fp) {
			perror("The file could not be opened\n");
			return 2;
		}

		if(file_count == 1) {
			while(!feof(fp)) {
				if(fgets(buffer, 256, fp) != NULL) {
					char *token = strtok(buffer, " ");
					do {
						if(hash_fetch(hashy, token) < .001) 
							hash_insert(hashy, token, file_count);
						}
						//token = strtok(NULL, "\0");
					} while((token = strtok(NULL, " ")) != NULL);
				}
			}
		}	
		else {
			while(!feof(fp)) {
				if(fgets(buffer, 256, fp) != NULL)
				{
					char *token = strtok(buffer, " ");
					while(token != NULL) {
						if(hash_fetch(hashy, token) > .001) {
							printf("here : %d\n", file_count);
							hash_insert(hashy, token, file_count);
						}
						token = strtok(NULL, " ");
					}
				}
			}	
		}	

		fclose(fp);
		++file_count;
	}

	free(buffer);

	free(buffer1);

	hash_destroy(hashy);
}

//the following functions were pulled from out hash program doen in class
uint64_t wang_hash(uint64_t key)
{
	key = (~key) + (key << 21);
	key = key ^ (key >> 24);
	key = (key + (key << 3)) + (key << 8);
	key = key ^ (key >> 14);
	key = (key + (key << 2)) + (key << 4);
	key = key ^ (key >> 28);
	key = key + (key << 31);
	return key;
}

hash *__hash_create(size_t capacity)
{
	hash *h = malloc(sizeof(*h));
	if(!h) {
		return NULL;
	}

	h->data = calloc(capacity, sizeof(*h->data));

	if(!h->data) {
		free(h);
		return NULL;
	}

	h->capacity = capacity;

	h->item_count = 0;

	return h;
}

hash *hash_create(void)
{
	return __hash_create(DEFAULT_SIZE);
}


static struct h_llist *h_llist_create(const char *key, double value)
{
	struct h_llist *node = malloc(sizeof(*node));

	if(!node) {
		return NULL;
	}

	node->key = strdup(key);
	if(!node->key) {
		free(node);
		return NULL;
	}
	node->value = value;
	node->next = NULL;

	return node;
}


static void h_llist_destroy(struct h_llist *list)
{
	while(list){
		struct h_llist *tmp = list->next;
		free(list->key);
		free(list);
		list = tmp;
	}
}

void hash_destroy(hash *h)
{
	if(h) {
		for(size_t n = 0; n < h->capacity; ++n) {
			h_llist_destroy(h->data[n]);
		}
	}

	free(h->data);
	free(h);	
}

static size_t hash_func(const char *key, size_t capacity)
{
	uint64_t buf = 0;
	strncpy((char *)(&buf), key, sizeof(buf));

	return wang_hash(buf) % capacity;
}	

void hash_insert(hash *h, const char *key, double value)
{
	if(!h || !key) {
		return;
	}

	hash_recalculate(h);

	size_t idx = hash_func(key, h->capacity);

	struct h_llist *tmp = h->data[idx];

	while(tmp) {
		if(strcmp(tmp->key, key) == 0) {
			tmp->value = value;
			return;
		}
		tmp = tmp->next;
	}

	struct h_llist *new = h_llist_create(key, value);
	if(!new) {
		return;
	}
	
	new->next = h->data[idx];

	h->data[idx] = new;

	h->item_count += 1;
}


double hash_fetch(hash *h, const char *key)
{
	if(!h || !key) {
		return 0;
	}

	size_t idx = hash_func(key, h->capacity);

	struct h_llist *tmp = h->data[idx];

	while(tmp) {
		if(strcmp(tmp->key, key) == 0) {
			return tmp->value;
		}

		tmp = tmp->next;
	}

	return 0;
}

static void hash_recalculate(hash *h)
{
	if(!h) {
		return;
	}

	if(h->item_count < 0.70 * h->capacity) {
		
		return;
	}

	hash *cpy = __hash_create(h->capacity * 2);
	if(!cpy) {
		return;
	}

	for(size_t n = 0; n < h->capacity;  ++n) {
		struct h_llist *tmp = h->data[n];
		while(tmp) {
			hash_insert(cpy, tmp->key, tmp->value);
			tmp = tmp->next;
		}
	}
	for(size_t n = 0; n < h->capacity; ++n) {
		h_llist_destroy(h->data[n]);
	}
	free(h->data);

	h->capacity = cpy->capacity;
	h->item_count = cpy->item_count;
	h->data = cpy->data;


	free(cpy);
}

void hash_traverse(hash *h, void (*func)(const char *, double))
{
	for(size_t n = 0; n < h->capacity; ++n) {
		struct h_llist *tmp = h->data[n];
		while(tmp) {
			func(tmp->key, tmp->value);
			tmp = tmp->next;
		}
	}
}
