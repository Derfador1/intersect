#include "intersect.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc == 1) {
		fprintf(stderr, "There were no files to read\n");
		return 1;
	}

	FILE *fp = fopen(argv[1], "r");

	if(!fp) {
		perror("The file could not be opened");
		return 2;
	}

	fclose(fp);
}
