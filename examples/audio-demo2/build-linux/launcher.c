#include <Audio.h>

#include "../../../drivers/audio-linux-alsa/audio-linux-alsa.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int main(){
	int success;
	int returnCode;
	AudioStructure *audio1;

	success = CreateAudioLinuxAlsa(&audio1, 48000);

	if(success){
		Program(audio1);

		CloseAudioLinuxAlsa(audio1);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return returnCode;
}



