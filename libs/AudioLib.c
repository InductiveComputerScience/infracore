double SineTone(double p, double samplesps, double hz){
	return sin(p/samplesps*(2*M_PI) * hz);
}

double SquareTone(double p, double samplesps, double hz){
	double n = sin(p/samplesps*(2*M_PI) * hz);
	double t;

	if(n > 0.5){
		t = 1;
	}
	if(n < 0.5){
		t = 0;
	}

	return t;
}

double SawToothTone(double p, double samplesps, double hz){
	double t;

	t = fmod(p*hz, samplesps);

	return t;
}
