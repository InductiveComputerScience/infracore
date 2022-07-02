#include <stdio.h>
#include <stdbool.h>

struct F{
	char *str;
	char c;

	_Bool found;
	int position;

	int i;
};

void f(struct F *fs);
