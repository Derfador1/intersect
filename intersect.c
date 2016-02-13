#define _BSD_SOURCE
#include "intersect_funcs.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

const size_t BUF_SZ = 256;

int main(int argc, char *argv[])
{
	if(argc == 1) {
		fprintf(stderr, "There were no files to read\n");
		return 1;
	}

	size_t file_count = 1;

	hash *hashy = hash_create();

	char *buffer = malloc(BUF_SZ * sizeof(buffer));

	while(file_count < (size_t)argc) {
		FILE *fp = fopen(argv[file_count], "r");

		if(!fp) {
			perror("The file could not be opened\n");
			free(buffer);
			return 2;
		}

		while(!feof(fp)) {
			if(fgets(buffer, BUF_SZ, fp) != NULL)
			{
				char *token = strtok(	buffer, " \t\n\f\v\r"); //a token that stores he current place as it walks down the buffer
				while(token != NULL) { 
					if(hash_fetch(hashy, token) == file_count - 1) { //if i fetch and its the same as 1 less the my files count the
						hash_insert(hashy, token, file_count);		//then i know i need to insert otherwise it wont get changed
																	//with insert
					}
					token = strtok(NULL, " \t\n\f\v\r"); //moves token along as the while happens
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