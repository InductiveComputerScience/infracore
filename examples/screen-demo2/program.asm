%include "Screen.inc"                     ; Include Screen functions

section .bss                              ; Global variables initialized to 0

pixels: resd 640*480                      ; The image to be displayed, color bytes: R, G, B, A
screen: resq 1                            ; The pointer to the screen device
counter: resq 1                           ; Loop counter variable

section .text                             ; Program data

global Program                            ; Make "Program" visible outside
Program:                                  ; Start of Program
  enter 0, 0                              ; Start new stack frame
  mov [screen], rdi                       ; Store screen pointer in variable

  mov qword [counter], 60*4               ; Loop this many times
  loop:                                   ; Start of loop
    ; Draw pixel
    mov rdx, qword [counter]              ; Pixel Row
    mov rax, qword [counter]              ; Pixel Column
    imul rax, 640                         ; Compute pixel position in array
    add rax, rdx
    mov byte [pixels + rax*4 + 0], 255    ; Red
    mov byte [pixels + rax*4 + 1], 255    ; Green
    mov byte [pixels + rax*4 + 2], 255    ; Blue

    ; Display image on screen
    mov rdi, [screen]                     ; Set up parameters: (screen, pixels)
    mov rsi, pixels
    call Display                          ; Ask image to be displayed on screen

    mov rdi, [screen]                     ; Set up parameters: (screen)
    call Synchronize                      ; Wait for the draw to complete

    dec qword [counter]                   ; Decrement loop variable
  jnz loop                                ; If zero, quit loop

  leave                                   ; Done with function
  ret                                     ; Return


