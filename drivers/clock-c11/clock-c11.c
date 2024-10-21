#include <time.h>
#include <math.h>

#include <Clock.h>


void ClockSpecs(ClockStructure *clock, double *resolution, double *range){
	*resolution = -9;
	*range = 15;

	// C23: timespec_getres returns the resolution.
}

double CurrentTime(ClockStructure *clock, double resolution){
  struct timespec now;
	double r, t;
	double n;

	n = resolution;

	timespec_get(&now, TIME_UTC);
	r = (double)now.tv_sec;

	if(n >= 0 && n <= 15){
		r = floor(r / pow(10, n))*pow(10, n);
	}else if(n >= -9 && n < 0){
		t = 15 - -n;
		r = fmod(r, pow(10, t));
		r = r + (double)now.tv_nsec / pow(10.0, 9.0);
		r = floor(r * pow(10.0, -n))/pow(10.0, -n);
	}else{
		r = 0;
	}

	return r;
}

