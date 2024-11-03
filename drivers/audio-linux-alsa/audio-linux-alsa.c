#include <alsa/asoundlib.h>
#include <math.h>
#include <stdbool.h>

#include <Audio.h>

struct AudioStructureLinuxAlsa{
	snd_pcm_t *pcm;
	double samplesPerSecond;
	double minimumDelay;
};

typedef struct AudioStructureLinuxAlsa AudioStructureLinuxAlsa;

_Bool CreateAudioLinuxAlsa(AudioStructure **audio, int64_t samplesPerSecond){
	AudioStructureLinuxAlsa *audioS;

	*audio = malloc(sizeof(AudioStructure));
	audioS = malloc(sizeof(AudioStructureLinuxAlsa));
	(*audio)->p = audioS;

	audioS->samplesPerSecond = samplesPerSecond;
	audioS->minimumDelay = (double)(int)(0.2 * samplesPerSecond);

	// Init
	snd_pcm_open(&audioS->pcm, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

	snd_pcm_hw_params_t *hw_params;
	snd_pcm_hw_params_alloca(&hw_params);

	snd_pcm_hw_params_any(audioS->pcm, hw_params);
	snd_pcm_hw_params_set_access(audioS->pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(audioS->pcm, hw_params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(audioS->pcm, hw_params, 1);
	snd_pcm_hw_params_set_rate(audioS->pcm, hw_params, samplesPerSecond, 0);
	snd_pcm_hw_params_set_periods(audioS->pcm, hw_params, 10, 0);
	snd_pcm_hw_params_set_period_time(audioS->pcm, hw_params, 100 * 1000, 0); // 0.1 seconds

	snd_pcm_hw_params(audioS->pcm, hw_params);

	return true;
}

int CloseAudioLinuxAlsa(AudioStructure *audio){
	AudioStructureLinuxAlsa *audioS;

	audioS = (AudioStructureLinuxAlsa*)audio->p;

	// Done
	snd_pcm_drain(audioS->pcm);
	snd_pcm_close(audioS->pcm);
}

void AudioSamplesMinDelay(AudioStructure *audio, double *rsamplespersecond, double *rmindelay){
	AudioStructureLinuxAlsa *audioS;

	audioS = (AudioStructureLinuxAlsa*)audio->p;

	*rsamplespersecond = audioS->samplesPerSecond;
	*rmindelay = audioS->minimumDelay;
}

double CurrentDelay(AudioStructure *audio){
	AudioStructureLinuxAlsa *audioS;

	audioS = (AudioStructureLinuxAlsa*)audio->p;

	snd_pcm_sframes_t delay;
	snd_pcm_delay(audioS->pcm, &delay);
	return delay;
}

void WriteSamples(AudioStructure *audio, double *samples, double length, double *samplesWritten){
	AudioStructureLinuxAlsa *audioS;
	int ret;

	audioS = (AudioStructureLinuxAlsa*)audio->p;

	short buffer[9600];
	int num = fmin(length, 9600);

	for(int i = 0; i < num; i++){
		buffer[i] = samples[i] * 32768;
	}
	
	ret = snd_pcm_writei(audioS->pcm, buffer, num);

	if(ret >= 0){
		*samplesWritten = ret;
	}else{
		*samplesWritten = 0;
	}
}

}















