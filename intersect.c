#define _BSD_SOURCE
#include "intersect.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

//taken from class code for hash program
static const size_t DEFAULT_SIZE = 10;
static struct h_llist *h_llist_create(const char *key, size_t value);
static void h_llist_destroy(struct h_llist *list);
static size_t hash_func(const char *key, size_t capacity);
static void hash_recalculate(hash *h);
uint64_t wang_hash(uint64_t key);
void ll_print(struct h_llist *h, size_t file_count);

void print_pair(const char *key, size_t value)
{
	printf("%zd %s\n", value, key);
}

int main(int argc, char *argv[])
{
	if(argc == 1) {
		fprintf(stderr, "There were no files to read\n");
		return 1;
	}

	size_t file_count = 1;

	hash *hashy = hash_create();

	char *buffer = malloc(20 * sizeof(buffer)); //change to malloc later and realloc

	//int case1 = 0;
 
	///char *carry_over = malloc(20 * sizeof(carry_over));

	while(file_count < (size_t)argc) {
		FILE *fp = fopen(argv[file_count], "r");

		if(!fp) {
			perror("The file could not be opened\n");
			free(buffer);
			exit(1);
		}

		while(!feof(fp)) {
			if(fgets(buffer, 256, fp) != NULL)
			{
				char *token = strtok(buffer, " \t\n\f\v\r");
				while(token != NULL) {
					if(hash_fetch(hashy, token) == file_count - 1) {
						hash_insert(hashy, token, file_count);
					}
					token = strtok(NULL, " \t\n\f\v\r");
				}
			}
		}	

		fclose(fp);
		++file_count;
	}

	struct h_llist *head = hash_to_ll(hashy);

	//merge_sort();

	ll_print(head, file_count - 1);

	free(buffer);

	hash_destroy(hashy);

	h_llist_destroy(head);
}

/*
static void merge(const void *arr[], size_t midpoint, size_t len,
		int (*cmp)(const void *, const void *), const void **tmp)
{
	size_t pos = 0;
	size_t p1 = 0;
	size_t p2 = midpoint;
	while(p1 < midpoint && p2 < len) {
		if(cmp(arr[p2], arr[p1]) < 0) {
			tmp[pos++] = arr[p2++];
		} else {
			tmp[pos++] = arr[p1++];
		}
	}

	while(p1 < midpoint) {
		tmp[pos++] = arr[p1++];
	}
	while(p2 < len) {
		tmp[pos++] = arr[p2++];
	}

	while(--pos) {
		arr[pos] = tmp[pos];
	}
}

static void mergesort_range(const void *arr[], size_t len,
		int (*cmp)(const void *, const void *), const void **tmp)
{
	if(len < 2) {
		return;
	}

	size_t middle = len / 2;
	mergesort_range(arr, middle, cmp, tmp);
	mergesort_range(arr + middle, len - middle, cmp, tmp);
	merge(arr, middle, len, cmp, tmp);
}

bool mergesort(const void *arr[], size_t len, int (*cmp)(const void *, const void *))
{
	if(!arr || !cmp || len < 2) {
		return false;
	}
	const void **tmp = malloc(len * sizeof(*tmp));
	if(!tmp) {
		return false;
	}
	mergesort_range(arr, len, cmp, tmp);

	free(tmp);
	return true;
}
*/

void ll_print(struct h_llist *h, size_t file_count) 
{
	while(h) {
		if(h->value == file_count) {
			printf("%zd %s\n", h->value, h->key);
		}

		h = h->next;
	}
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


static struct h_llist *h_llist_create(const char *key, size_t value)
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

void hash_insert(hash *h, const char *key, size_t value)
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


size_t hash_fetch(hash *h, const char *key)
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

struct h_llist *hash_to_ll(hash *h)
{
	struct h_llist *head = NULL;

	for (size_t n = 0; n < h->capacity; ++n) {
		if(!h->data[n]) {
			continue;
		}

		struct h_llist *tail = h->data[n];
		while(tail->next) {
			tail = tail->next;
		}

		tail->next = head;
		head = h->data[n];
	}

	memset(h->data, '\0', h->capacity * sizeof(*h->data));

	return head;
	/*
	for(size_t n = 0; n < h->capacity; ++n) {
		struct h_llist *tmp = h->data[n];
		while(tmp) {
			if(tmp->value == file_count) {
				func(tmp->key, tmp->value);
			}
			tmp = tmp->next;
		}
	}
	*/
}
