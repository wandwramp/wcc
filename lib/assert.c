#include <stdio.h>
#include <stdlib.h>

static char rcsid[] = "$Id: assert.c,v 1.1.1.1 2002/07/08 04:28:54 daa1 Exp $";

int _assert(char *e, char *file, int line) {
	fprintf(stderr, "assertion failed:");
	if (e)
		fprintf(stderr, " %s", e);
	if (file)
		fprintf(stderr, " file %s", file);
	fprintf(stderr, " line %d\n", line);
	fflush(stderr);
	abort();
	return 0;
}
