#ifndef AudioH
#define AudioH

#include <stdint.h>

struct AudioStructure{
	void *p;
};

typedef struct AudioStructure AudioStructure;

// Audio Controller Instructions
void AudioSamplesMinDelay(AudioStructure *audio, double *samplespersecond, double *mindelay);
double CurrentDelay(AudioStructure *audio);
void WriteSamples(AudioStructure *audio, double *samples, double length, double *samplesWritten);

#endif
