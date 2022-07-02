#include "prog.h"
#include "prog2.h"

int main(){
	struct F fs;

	fs.str = "Hello world!";
	fs.c = 'w';

	f(&fs);

	printf("Found: %s\n", fs.found ? "true" : "false");
	if(fs.found){
		printf("Position: %d\n", fs.position);
	}

	int position;
	_Bool found = f2("Hello world!", 'w', &position);
	printf("Found: %s\n", found ? "true" : "false");
	if(found){
		printf("Position: %d\n", position);
	}

	return 0;
}

