#include <stdio.h>
#include <stdlib.h>

static char rcsid[] = "$Id: yynull.c,v 1.1.1.1 2002/07/08 04:28:54 daa1 Exp $";

void _YYnull(char *file, int line) {
	fprintf(stderr, "null pointer dereferenced:");
	if (file)
		fprintf(stderr, " file %s,", file);
	fprintf(stderr, " line %d\n", line);
	fflush(stderr);
	abort();
}
