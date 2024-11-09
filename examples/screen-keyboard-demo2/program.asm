%include "Screen.inc"                     ; Include Screen functions
%include "Keyboard.inc"                   ; Include Keyboard functions

section .bss                              ; Global variables initialized to 0

  pixels: resd 640*480                    ; The image to be displayed, color bytes: R, G, B, A
  screen: resq 1                          ; The pointer to the screen device
  keyboard: resq 1                        ; The pointer to the keyboard device
  counter: resq 1                         ; Loop counter variable
  xpos: resq 1                            ; Pixel draw position
  ypos: resq 1                            ; ---
  dxp: resq 1                             ; Change of pixel position
  dyp: resq 1                             ; ---
  history: resq 200                       ; History of key presses since last check
  state: resq 200                         ; History of key states since last check
  length: resq 1                          ; Number of keyboard history events

section .text                             ; Program data

global Program                            ; Make "Program" visible outside
Program:                                  ; Start of Program
  enter 0, 0                              ; Start new stack frame
  mov [screen], rdi                       ; Store screen pointer in variable
  mov [keyboard], rsi                     ; Store keyboard pointer in variable

  mov qword [xpos], 60*4                  ; Pixel start position
  mov qword [ypos], 60*4                  ; ---
  mov qword [dxp], 1                      ; Initial change of position
  mov qword [dyp], 0                      ; ---

  mov qword [counter], 60*4               ; Loop this many times
  loop:                                   ; Start of loop
    ; Set pixel
    mov rax, [dxp]                        ; Change x position
    add [xpos], rax                       ; ---
    mov rax, [dyp]                        ; Change y position
    add [ypos], rax                       ; ---

    ; Draw pixel
    mov rdx, qword [xpos]                 ; Pixel Row
    mov rax, qword [ypos]                 ; Pixel Column
    imul rax, 640                         ; Compute pixel position in array
    add rax, rdx                          ; ---
    mov byte [pixels + rax*4 + 0], 255    ; Red
    mov byte [pixels + rax*4 + 1], 255    ; Green
    mov byte [pixels + rax*4 + 2], 255    ; Blue

    ; Check keyboard
    mov rdi, [keyboard]                   ; (keyboard, history, state, &length);
    mov rsi, history                      ; ---
    mov rdx, state                        ; ---
    mov rcx, length                       ; ---
    call GetKeyboardState                 ; Get keyboard events since last check

    ; Check which keys were pressed
    mov rcx, 0                            ; Loop over events
    keyloop:
      cvtsd2si rdi, [history + rcx*8]     ; Get event
      mov esi, [state + rcx*4]            ; Get state
      call KeyEvent                       ; Perform actions based on event

      cvtsi2sd xmm0, rcx                  ; while count < length
      comisd xmm0, [length]               ; ---
    jl keyloop                            ; ---

    ; Display image on screen
    mov rdi, [screen]                     ; Set up parameters: (screen, pixels)
    mov rsi, pixels                       ; ---
    call Display                          ; Ask image to be displayed on screen

    mov rdi, [screen]                     ; Set up parameters: (screen)
    call Synchronize                      ; Wait for the draw to complete

    dec qword [counter]                   ; Decrement loop variable
  jnz loop                                ; If zero, quit loop

  leave                                   ; Done with function
  ret                                     ; Return

KeyEvent:                                 ; Function for doing something based on keyboard events
  enter 0, 0                              ; Start stack frame

  cmp edi, 0                              ; Skip if key up
  je done                                 ; ---

  cmp rdi, -1                             ; Up
  je up                                   ; ---
  cmp rdi, -2                             ; Down
  je down                                 ; ---
  cmp rdi, -3                             ; Left
  je left                                 ; ---
  cmp rdi, -4                             ; Right
  je right                                ; ---

  jmp done                                ; No relevant key pressed

  up:                                     ; Move -1, 0
    mov qword [dyp], -1                   ; ---
    mov qword [dxp], 0                    ; ---
    jmp done
  down:                                   ; Move 1, 0
    mov qword [dyp], 1                    ; ---
    mov qword [dxp], 0                    ; ---
    jmp done
  left:                                   ; Move 0, -1
    mov qword [dyp], 0                    ; ---
    mov qword [dxp], -1                   ; ---
    jmp done
  right:                                  ; Move 0, 1
    mov qword [dyp], 0                    ; ---
    mov qword [dxp], 1                    ; ---
    jmp done

  done:

  leave                                   ; Done with function
  ret                                     ; Return

