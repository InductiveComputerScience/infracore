%include "Audio.inc"                      ; Include Audio functions

section .bss                              ; Global variables initialized to 0

samples: resq 48000*2                     ; The image to be displayed, color bytes: R, G, B, A
size: resq 1
audio: resq 1                             ; The pointer to the screen device
counter: resq 1                           ; Loop counter variable
samplesps: resq 1
hz: resq 1
cur: resq 1
delay: resq 1
mindelay: resq 1
x: resq 1
samplesWritten: resq 1

section .text                             ; Program data

global Program                            ; Make "Program" visible outside
Program:                                  ; Start of Program
  enter 0, 0                              ; Start new stack frame
  mov [audio], rdi                        ; Store audio pointer in variable
  mov qword [samplesps], 48000
	mov qword [size], 48000*2
  mov rax, __?float64?__(261.626)         ; C tone hertz
	mov qword [hz], rax
	mov qword [mindelay], 9600

	finit

	; Create pure C tone sine wave
  mov qword [counter], 0                  ; Loop this many times
  loop:                                   ; Start of loop
		fild qword [counter]                  ; Expression: sin(i/samplesps*(2*M_PI) * hz)
		fild qword [samplesps]
		fdiv
    fldpi
    fldpi
		fadd
		fmul
		fld qword [hz]
		fmul
		fsin
		mov rax, samples
		mov rcx, [counter]
		fstp qword [rax + rcx*8]

    inc qword [counter]                   ; Decrement loop variable
    cmp qword [counter], 48000*2
  jl loop                                 ; If zero, quit loop

	; Play sound
	mov qword [cur], 0
	playloop:
		mov rdi, [audio]                      ; parameters: (audio)
		call CurrentDelay                     ; Get the current delay
		cvtsd2si rax, xmm0                    ; Get the return value
		mov qword [delay], rax

		; if(delay < mindelay * 1.5){         ; delay < mindelay * 1.5
		finit
		fild qword [mindelay]
		mov rax, __?float64?__(1.5)           ;
		mov [x], rax
		fld qword [x]
		fmul
		fild qword [delay]                    ; 
		fcomi

		ja skip
			mov rdi, [audio]                    ; (audio, samples + cur, fmin(mindelay, size - cur), &samplesWritten)
			mov rcx, qword [cur]
			lea rsi, qword [samples + rcx*8]
			cvtsi2sd xmm1, qword [size]
			cvtsi2sd xmm2, qword [cur]
			subsd xmm1, xmm2
			cvtsi2sd xmm0, qword [mindelay]
			minsd xmm0, xmm1
			mov rdx, samplesWritten
			call WriteSamples
			cvtsd2si rax, [samplesWritten]
			add [cur], rax
		skip:

		pause                                  ; Tell CPU it is a spin-wait

		mov rax, qword [cur]
		cmp rax, qword [size]
	jl playloop

  leave                                    ; Done with function
  ret                                      ; Return



















