#include "prog2.h"

_Bool f2(char *str, char c, int *position){
	int i;
	_Bool found;

	found = false;

	for(i = 0; str[i] != '\0' && !found; i++){
		if(str[i] == c){
			found = true;
			*position = i;
		}
	}

	return found;
}

