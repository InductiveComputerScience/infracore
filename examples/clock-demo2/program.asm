%include "Clock.inc"                      ; Include Clock functions

section .bss                              ; Global variables initialized to 0

  samples: resq 48000*2                   ; The audio buffer, one sample is a double between 0 and 1
	clock: resq 1                           ; clock device pointer
	resolution: resq 1                      ; Resolution of clock
	range: resq 1                           ; Range of clock
	t1: resq 1                              ; Time 1
	t2: resq 1                              ; Time 2

section .text                             ; Program data

global Program                            ; Make "Program" visible outside
Program:                                  ; Start of Program
  enter 0, 0                              ; Start new stack frame
  mov [clock], rdi                        ; Store clock pointer in variable

	; Get clock specs
	mov rdi, [clock]                        ; (clock, &resolution, &range)
	mov rsi, resolution                     ; ---
	mov rdi, range                          ; ---
	call ClockSpecs                         ; Get clock specifications

	; Get curren time
	mov rdi, [clock]                        ; (clock, -2)
	mov rax, -2                             ; ---
	cvtsi2sd xmm0, rax                      ; ---
	call CurrentTime                        ; Get current time
	movq [t1], xmm0                         ; Store current time in t1

	; Wait 1 second
	loop:
	  mov rdi, [clock]                      ; (clock, -2)
	  mov rax, -2                           ; ---
  	cvtsi2sd xmm0, rax                    ; ---
  	call CurrentTime                      ; Get current time
  	movq [t2], xmm0                       ; Store current time in t1

		movq xmm0, [t1]                       ; while t1 + 1 < t2
		mov rax, 1                            ; ---
		cvtsi2sd xmm1, rax                    ; ---
		addsd xmm0, xmm1                      ; ---
		comisd xmm0, [t2]                     ; ---

		pause                                 ; Spinlock pause
	ja loop                                 ; ---

  leave                                   ; Done with function
  ret                                     ; Return

