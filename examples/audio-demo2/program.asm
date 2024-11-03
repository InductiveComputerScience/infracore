%include "Audio.inc"                      ; Include Audio functions

section .bss                              ; Global variables initialized to 0

  samples: resq 48000*2                   ; The image to be displayed, color bytes: R, G, B, A
  size: resq 1                            ; The size of the audio buffer
  mindelay: resq 1                        ; The minimum delay of the audio
  audio: resq 1                           ; The pointer to the screen device
  counter: resq 1                         ; Loop counter variable
  samplesps: resq 1                       ; Samples per second for the audio device
  cur: resq 1                             ; Current position of audio samples sent to audio device
  delay: resq 1                           ; The current delay of the audio
  tmp: resq 1                             ; Temporary variable used for x87 loads
  samplesWritten: resq 1

section .rodata                           ; Read-only data

  pi: dq __?float64?__(3.14159265358979)  ; π

section .text                             ; Program data

global Program                            ; Make "Program" visible outside
Program:                                  ; Start of Program
  enter 0, 0                              ; Start new stack frame
  mov [audio], rdi                        ; Store audio pointer in variable

  ; Get the specs of the audio device
  mov rdi, [audio]                        ; (audio, &samplesps, &mindelay)
  mov rsi, samplesps                      ; ---
  mov rdx, mindelay                       ; ---
  call AudioSamplesMinDelay               ; Get samples per second and minimum delay

  ; Compute the size of the audio sample
  cvtsd2si rax, [samplesps]
  imul rax, 2                             ; 2 seconds of audio
  mov [size], rax

  finit                                   ; Initialize the x87 floating point unit

  ; Create pure C tone sine wave
  mov qword [counter], 0                  ; Start at counter = 0
  loop:                                   ; Start of loop
    cvtsi2sd xmm0, [counter]              ; expression: sin(counter/samplesps*(2*pi) * hz)
    divsd xmm0, [samplesps]               ; counter/samplesps
    mov rax, 2                            ; 2*pi
    cvtsi2sd xmm1, rax                    ; ---
    mulsd xmm1, [pi]                      ; ---
    mulsd xmm0, xmm1                      ; counter/samplesps*(2*pi)
    mov rax, __?float64?__(261.626)       ; middle C tone hertz
    movq xmm2, rax                        ; 
    mulsd xmm0, xmm2                      ; counter/samplesps*(2*pi) * hz
    movq [tmp], xmm0                      ; sin(counter/samplesps*(2*pi) * hz)
    fld qword [tmp]                       ; ---
    fsin                                  ; ---
    mov rax, samples                      ; Write sample
    mov rcx, [counter]                    ; ---
    fstp qword [rax + rcx*8]              ; ---

    inc qword [counter]                   ; Increment loop variable
    mov rax, [size]                       ; while counter < size
    cmp [counter], rax                    ; ---
  jl loop                                 ; ---

  ; Play sound
  mov qword [cur], 0                      ; Play audio from the start
  playloop:
    ; Get the current audio delay
    mov rdi, [audio]                      ; parameters: (audio)
    call CurrentDelay                     ; Get the current delay
    movq [delay], xmm0                    ; Get the return value

    movq xmm0, [delay]                    ; if delay < mindelay * 1.5 
    movq xmm1, [mindelay]                 ; ---
    mov rax, __?float64?__(1.5)           ; t1 = 1.5
    cvtsi2sd xmm2, rax                    ; ---
    mulsd xmm1, xmm2                      ; t2 = mindelay * t1
    comisd xmm0, xmm1                     ; delay < t2 -- set EFLAGS accordingly

    ja skip
      ; Write more audio samples
      mov rdi, [audio]                    ; Set up parameters: (audio, samples + cur, fmin(mindelay, size - cur), &samplesWritten)
      mov rcx, [cur]                      ; samples + cur
      lea rsi, [samples + rcx*8]          ; ---
      cvtsi2sd xmm1, [size]               ; fmin(mindelay, size - cur)
      cvtsi2sd xmm2, [cur]                ; ---
      subsd xmm1, xmm2                    ; ---
      movq xmm0, [mindelay]               ; ---
      minsd xmm0, xmm1                    ; ---
      mov rdx, samplesWritten             ; &samplesWritten
      call WriteSamples                   ; Write more audio samples
      cvtsd2si rax, [samplesWritten]      ; cur += samplesWritten
      add [cur], rax                      ; ---
    skip:

    pause                                 ; Tell CPU this is a spin-wait

    mov rax, [cur]                        ; while cur < size
    cmp rax, [size]                       ; ---
  jl playloop                             ; ---

  leave                                   ; Done with function
  ret                                     ; Return

