Include Screen-masm.inc                     ; Include Screen functions

extern calloc :PROC

.data?                                      ; Global variables initialized to 0

pixels dq ?                                 ; The image to be displayed, color bytes: R, G, B, A
screen dq ?                                 ; The pointer to the screen device
counter dq ?                                ; Loop counter variable

.code                                       ; Program data

                                            ; Make "Program" visible outside
Program PROC                                ; Start of Program
  enter 0, 0                                ; Start new stack frame
  mov [screen], rcx                         ; Store screen pointer in variable

  mov rcx, 640*480*4
  mov rdx, 1
  call calloc
  mov [pixels], rax

  mov qword ptr [counter], 60*4             ; Loop this many times
  renderloop:                               ; Start of loop
    ; Draw pixel
    mov rdx, qword ptr [counter]            ; Pixel Row
    mov rax, qword ptr [counter]            ; Pixel Column
    imul rax, 640                           ; Compute pixel position in array
    add rax, rdx
    mov rdi, [pixels]
    mov byte ptr [rdi + rax*4 + 0], 255     ; Red
    mov byte ptr [rdi + rax*4 + 1], 255     ; Green
    mov byte ptr [rdi + rax*4 + 2], 255     ; Blue

    ; Display image on screen
    mov rcx, [screen]                       ; Set up parameters: (screen, pixels)
    mov rdx, [pixels]
    call Display                            ; Ask image to be displayed on screen

    mov rcx, [screen]                       ; Set up parameters: (screen)
    call Synchronize                        ; Wait for the draw to complete

    dec qword ptr [counter]                 ; Decrement loop variable
  jnz renderloop                            ; If zero, quit loop

  leave                                     ; Done with function
  ret                                       ; Return
Program ENDP

END

