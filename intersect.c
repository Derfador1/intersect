#define _BSD_SOURCE
#include "intersect.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


int main(int argc, char *argv[])
{
	if(argc == 1) {
		fprintf(stderr, "There were no files to read\n");
		return 1;
	}

	int file_count = 1;

	hash *hashy = hash_create();

	while(file_count < argc) {
		FILE *fp = fopen(argv[file_count], "r");

		if(!fp) {
			perror("The file could not be opened");
			return 2;
		}

		while(!feof(fp)) {
			char *buffer = malloc(256 * sizeof(*buffer)); //change to malloc later and realloc
			fgets(buffer, 1, fp);
			printf("Word: %s\n", buffer);
		}		

		fclose(fp);
		++file_count;
	}

	hash_destroy(hashy);
}
