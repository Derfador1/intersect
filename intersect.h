#ifndef INTERSECT_H
	#define INTERSECT_H

#include <stdlib.h>
#include <string.h>

struct h_llist {
	char *key;
	double value;
	struct h_llist *next;
};

typedef struct {
	size_t item_count;
	size_t capacity;
	struct h_llist **data;
} hash;

hash *__hash_create(size_t capacity);
hash *hash_create(void);
void hash_destroy(hash *h);
//void hash_delete
void hash_insert(hash *h, const char *key, double value);
double hash_fetch(hash *h, const char *key);
void hash_traverse(hash *h, void (*func)(const char *, double));

#endif
