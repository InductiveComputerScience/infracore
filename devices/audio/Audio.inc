extern AudioSamplesMinDelay  ; rdi = audio, rdi = &samplesps, rdx = &mindelay
extern CurrentDelay          ; rdi = audio, xmm0 holds return value
extern WriteSamples          ; rdi = audio, rdi = buffer, xmm0 = samples, rdx = &samplesWritten
