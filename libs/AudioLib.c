double SineTone(double p, double samplesps, double hz){
	return sin(p/samplesps*(2*M_PI) * hz);
}
