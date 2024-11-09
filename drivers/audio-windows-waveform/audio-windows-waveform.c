#define _USE_MATH_DEFINES 1
#include <math.h>
#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <mmreg.h>

MMRESULT play(float nSeconds, float signal(float timeInSeconds, unsigned short channel, void *context), void *context, unsigned long samplesPerSecond){

  UINT timePeriod = 1;

  MMRESULT mmresult = MMSYSERR_NOERROR;
  WAVEFORMATEX waveFormat = {0};
  waveFormat.cbSize = 0;
  waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
  waveFormat.nChannels = 2;
  waveFormat.nSamplesPerSec = samplesPerSecond;
  const size_t nBuffer = (size_t)(nSeconds * waveFormat.nChannels * waveFormat.nSamplesPerSec);
  float *buffer;
  waveFormat.wBitsPerSample = CHAR_BIT * sizeof(buffer[0]);
  waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / CHAR_BIT;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

  buffer = (float *)calloc(nBuffer, sizeof(*buffer));

	for (size_t i = 0; i < nBuffer; i += waveFormat.nChannels){
		for (unsigned short j = 0; j < waveFormat.nChannels; j++){
			buffer[i+j] = signal((i+j) * nSeconds / nBuffer, j, context);
		}
	}

  HWAVEOUT hWavOut = NULL;
  mmresult = waveOutOpen(&hWavOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL);

	MMTIME xtime;
	xtime.wType = TIME_SAMPLES;

  if (mmresult == MMSYSERR_NOERROR){
    timeBeginPeriod(timePeriod);
    WAVEHDR hdr = {0};
    hdr.dwBufferLength = (ULONG)(nBuffer * sizeof(buffer[0]));
    hdr.lpData = (LPSTR)&buffer[0];
    mmresult = waveOutPrepareHeader(hWavOut, &hdr, sizeof(hdr));
    if (mmresult == MMSYSERR_NOERROR){
      ULONG start = GetTickCount();
      mmresult = waveOutWrite(hWavOut, &hdr, sizeof(hdr));
      Sleep(1000);
			mmresult = waveOutGetPosition(hWavOut, &xtime, sizeof(MMTIME));
			printf("Current play position: %d\n", xtime.u.sample);
      Sleep(1000);
      waveOutUnprepareHeader(hWavOut, &hdr, sizeof(hdr));
    }
    timeEndPeriod(timePeriod);
		waveOutClose(hWavOut);
	}

  free(buffer);

  return mmresult;
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

int _tmain(int argc, _TCHAR* argv[]){
	float frequency = 2 * 261.626F;
	play(10, pure, &frequency, 48000);
	return 0;
}
