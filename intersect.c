#define _BSD_SOURCE
#include "intersect.h"
#include "intersect_funcs.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

//taken from class code for hash program
static const size_t DEFAULT_SIZE = 10;
static struct h_llist *h_llist_create(const char *key, size_t value);
static void h_llist_destroy(struct h_llist *list);
static size_t hash_func(const char *key, size_t capacity);
static void hash_recalculate(hash *h);
uint64_t wang_hash(uint64_t key);

void print_func(char *word)
{
	printf("%s\n", word);
}

const size_t BUF_SZ = 256;

char *make_it_lower(const char *string)
{
	char *str = strdup(string);
	for(unsigned int i = 0; i < strlen(str); ++i)
	{
		str[i] = tolower(str[i]);
	}
	return str;
}

int main(int argc, char *argv[])
{
	if(argc == 1) {
		fprintf(stderr, "There were no files to read\n");
		return 1;
	}

	size_t file_count = 1;

	hash *hashy = hash_create();

	char *buffer = malloc(BUF_SZ * sizeof(buffer)); //change to malloc later and realloc

	//int case1 = 0;
 
	///char *carry_over = malloc(20 * sizeof(carry_over));

	while(file_count < (size_t)argc) {
		FILE *fp = fopen(argv[file_count], "r");

		if(!fp) {
			perror("The file could not be opened\n");
			free(buffer);
			//fclose(fp);
			return 2;
		}

		while(!feof(fp)) {
			if(fgets(buffer, BUF_SZ, fp) != NULL)
			{
				char *token = strtok(	buffer, " \t\n\f\v\r");
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
	
	//gave in and used trees instead of a merge sort

	bst *new_bst = bst_create();
	
	fill_bst(new_bst, hashy, file_count);

	tree_inorder(new_bst->root, print_func);

	free(buffer);

	hash_destroy(hashy);

	bst_destroy(new_bst);
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
		//free(list->lower_key);
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

	if(value == 1) {
		
		char *lowered = make_it_lower(key);
		
		hash_recalculate(h);
	
		size_t idx = hash_func(lowered, h->capacity);
	
		struct h_llist *tmp = h->data[idx];
	
		while(tmp) {
			if(strcmp(tmp->key, key) == 0) {
				tmp->value = value;
				free(lowered);
				return;
			}
			tmp = tmp->next;
		}
	
		struct h_llist *new = h_llist_create(key, value);
		if(!new) {
			return;
		}
		new->lower_key = lowered;
		
		new->next = h->data[idx];
	
		h->data[idx] = new;
	
		h->item_count += 1;

		//free(new->lower_key);
		free(lowered);
	}
	else {
		char *key_check = make_it_lower(key);
		size_t idx = hash_func(key_check, h->capacity);
		struct h_llist*tmp = h->data[idx];
		while(tmp) {
			if(strcmp(tmp->key, key_check) == 0) {
				tmp->value = value;
				free(key_check);
				return;
			}
			tmp = tmp->next;	
		}
		free(key_check);
	}
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

//Code derived from dsprimm
void fill_bst(bst *sorted, hash *h, size_t file_count){
	if(!h){
        	return;
    	}

    	for(size_t n = 0; n <= h->capacity; ++n){
        	struct h_llist *tmp = h->data[n];
        	while(tmp){
            	if(tmp->value == file_count - 1){
                	bst_insert(sorted, tmp->key);
            	}
            	tmp = tmp->next;
        	}
	}
}

//Taken from class tree exercise
bst *bst_create(void){
	bst *b = malloc(sizeof(*b));
	if(b){
		b->root = NULL;
	}

	return b;
}

//Taken from class tree exercise
void bst_destroy(bst *b){
	if(!b){
		return;
	}
	tree_destroy(b->root);
	free(b);
}

//Taken from class tree exercise
static bool tree_insert(struct tree **t, char *data){
	if(!*t){
		*t = tree_create(data);
		return *t;
	}
	struct tree *node = *t;

	if(strcmp(data, node->data) > 0){
		return tree_insert(&node->right, data);
	} 
	else{
		return tree_insert(&node->left, data);
	}
}

//Taken from class tree exercise
bool bst_insert(bst *b, char *data){
	if(!b){
		return false;
	}
	return tree_insert(&b->root, data);
}

//Taken from class tree exercise
struct tree *tree_create(char *data){
	struct tree *t = malloc(sizeof(*t));
	if(t){
		t->data = data;
		t->left = NULL;
		t->right = NULL;
	}

	return t;
}

//Taken from class tree exercise
void tree_destroy(struct tree *t){
	if(!t){
		return;
	}

	tree_destroy(t->left);
	tree_destroy(t->right);
	free(t);
}

//Taken from class tree exercise
void tree_inorder(struct tree *t, void (*func)(char *)){
	if(!t){
		return;
	}

	tree_inorder(t->left, func);
	func(t->data);
	tree_inorder(t->right, func);
}
