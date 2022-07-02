#include "prog.h"

void f(struct F *fs){
	fs->found = false;

	for(fs->i = 0; fs->str[fs->i] != '\0' && !fs->found; fs->i++){
		if(fs->str[fs->i] == fs->c){
			fs->found = true;
			fs->position = fs->i;
		}
	}
}
