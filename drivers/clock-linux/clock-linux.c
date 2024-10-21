#include <time.h>
#include <math.h>

#include <Clock.h>

int sys_clock_getres(clockid_t clk_id, struct timespec *res);
int sys_clock_gettime(clockid_t clk_id, struct timespec *tp);

void ClockSpecs(ClockStructure *clock, double *resolution, double *range){
	int r;
	struct timespec res;
	double n;

	*resolution = -9;
	*range = 15;

	r = sys_clock_getres(CLOCK_REALTIME, &res);
	if(r == 0){
		if(res.tv_sec == 0 && res.tv_nsec > 0){
			n = res.tv_nsec;
			*resolution = -9 + ceil(log10(n));

			//printf("%f\n", n);
		}
	}

	//printf("%ld, %ld\n", res.tv_sec, res.tv_nsec);
	//printf("%ld, %ld\n", (long)*resolution, (long)*range);
}

double CurrentTime(ClockStructure *clock, double resolution){
	struct timespec now;
	double r, t, n;

	n = resolution;

	sys_clock_gettime(CLOCK_REALTIME, &now);
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

