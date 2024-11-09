#define _USE_MATH_DEFINES 1
#include <math.h>
#include <stdio.h>
#include <tchar.h>
#include <stdint.h>
#include <stdbool.h>

#include <Audio.h>

#include <windows.h>
#include <mmreg.h>

struct AudioStructureWindowsWaveform{
  HWAVEOUT hWavOut;
  WAVEFORMATEX waveFormat;
  WAVEHDR hdr[10];
  double samplesPerSecond;
  double minimumDelay;
  double samplesFed;
};

typedef struct AudioStructureWindowsWaveform AudioStructureWindowsWaveform;

bool CreateAudioWindowsWaveform(AudioStructure **audio, int64_t samplesPerSecond){
  AudioStructureWindowsWaveform *audioS;
  int ret;

  *audio = malloc(sizeof(AudioStructure));
  audioS = calloc(sizeof(AudioStructureWindowsWaveform), 1);
  (*audio)->p = audioS;

  audioS->samplesPerSecond = samplesPerSecond;
  audioS->minimumDelay = (double)(int)(0.2 * samplesPerSecond);

  UINT timePeriod = 1;

  MMRESULT mmresult = MMSYSERR_NOERROR;
  audioS->waveFormat.cbSize = 0;
  audioS->waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
  audioS->waveFormat.nChannels = 1;
  audioS->waveFormat.nSamplesPerSec = samplesPerSecond;
  audioS->waveFormat.wBitsPerSample = CHAR_BIT * sizeof(float);
  audioS->waveFormat.nBlockAlign = audioS->waveFormat.nChannels * audioS->waveFormat.wBitsPerSample / CHAR_BIT;
  audioS->waveFormat.nAvgBytesPerSec = audioS->waveFormat.nSamplesPerSec * audioS->waveFormat.nBlockAlign;

  audioS->hWavOut = NULL;
  mmresult = waveOutOpen(&audioS->hWavOut, WAVE_MAPPER, &audioS->waveFormat, 0, 0, CALLBACK_NULL);

  return true;
}

void CloseAudioWindowsWaveform(AudioStructure *audio){
 /* AudioStructureWindowsWaveform *audioS;
  audioS = (AudioStructureWindowsWaveform*)audio->p;

  waveOutClose(audioS->hWavOut);*/
}

void AudioSamplesMinDelay(AudioStructure *audio, double *samplespersecond, double *mindelay){
  AudioStructureWindowsWaveform *audioS;
  audioS = (AudioStructureWindowsWaveform*)audio->p;

  *samplespersecond = audioS->samplesPerSecond;
  *mindelay = audioS->minimumDelay;
}

double CurrentDelay(AudioStructure *audio){
  AudioStructureWindowsWaveform *audioS;
  audioS = (AudioStructureWindowsWaveform*)audio->p;

  MMTIME xtime;
  xtime.wType = TIME_SAMPLES;

  MMRESULT mmresult = MMSYSERR_NOERROR;

  mmresult = waveOutGetPosition(audioS->hWavOut, &xtime, sizeof(MMTIME));

  //return audioS->minimumDelay - xtime.u.sample;

  return audioS->samplesFed - xtime.u.sample;
}

void WriteSamples(AudioStructure *audio, double *samples, double length, double *samplesWritten){
  AudioStructureWindowsWaveform *audioS;
  audioS = (AudioStructureWindowsWaveform*)audio->p;

  MMRESULT mmresult = MMSYSERR_NOERROR;

  static int c = 0;
  c++;
  c = c % 10;

  float *buffer;
  size_t nBuffer = length;
  buffer = (float *)calloc(nBuffer, sizeof(*buffer));
  for (size_t i = 0; i < length; i++){
    buffer[i] = samples[i];
  }

  if (mmresult == MMSYSERR_NOERROR){
    //timeBeginPeriod(timePeriod);
    audioS->hdr[c].dwBufferLength = (ULONG)(nBuffer * sizeof(buffer[0]));
    audioS->hdr[c].lpData = (LPSTR)&buffer[0];
    mmresult = waveOutPrepareHeader(audioS->hWavOut, &audioS->hdr[c], sizeof(audioS->hdr[c]));
    if (mmresult == MMSYSERR_NOERROR){
      ULONG start = GetTickCount();
      mmresult = waveOutWrite(audioS->hWavOut, &audioS->hdr[c], sizeof(audioS->hdr[c]));
      //waveOutUnprepareHeader(audioS->hWavOut, &audioS->hdr[c], sizeof(audioS->hdr[c]));
    }
    //timeEndPeriod(timePeriod);

  }

  *samplesWritten = length;

  audioS->samplesFed += *samplesWritten;
}

// Triangle wave generator
float triangle(float timeInSeconds, unsigned short channel, void *context){
  const float frequency = *(const float *)context;
  const float angle = (float)(frequency * 2 * M_PI * timeInSeconds);
  switch (channel){
    case 0:{
      return (float)asin(sin(angle + 0 * M_PI / 2));
    }
    default:{
      return (float)asin(sin(angle + 1 * M_PI / 2));
    }
  }
}

// Pure tone generator
float pure(float timeInSeconds, unsigned short channel, void *context){
  const float frequency = *(const float *)context;
  const float angle = (float)(frequency * 2 * M_PI * timeInSeconds);
  switch (channel){
    case  0:{
      return (float)sin(angle + 0 * M_PI / 2);
    }
    default:{
      return (float)sin(angle + 1 * M_PI / 2);
    }
  }
}

/*int _tmain(int argc, _TCHAR* argv[]){
  float frequency = 2 * 261.626F;
  play(10, pure, &frequency, 48000);
  return 0;
}*/
