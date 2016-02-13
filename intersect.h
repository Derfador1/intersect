#ifndef INTERSECT_H
	#define INTERSECT_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct h_llist {
	char *key;
	char *lower_key;
	size_t value;
	struct h_llist *next;
};

typedef struct {
	size_t item_count;
	size_t capacity;
	struct h_llist **data;
} hash;

//code given in class
struct tree {
	char * data;
	struct tree *left;
	struct tree *right;
};

//code given in class
typedef struct {
	struct tree *root;
} bst;

bst *bst_create(void);
void bst_destroy(bst *b);
struct tree *tree_create(char * data);
void tree_destroy(struct tree *t);
void tree_inorder(struct tree *t, void (*func)(char *));
void fill_bst(bst *sorted, hash *h, size_t file_count);
bool bst_insert(bst *b, char *data);

hash *__hash_create(size_t capacity);
hash *hash_create(void);
void hash_destroy(hash *h);
void hash_insert(hash *h, const char *key, size_t value);
size_t hash_fetch(hash *h, const char *key);
struct h_llist *hash_to_ll(hash *h);

#endif
