%include "Disk.inc"                       ; Include Clock functions

extern putchar                            ; Use putchar from the C library for demo purposes

section .bss                              ; Global variables initialized to 0

  disk: resq 1                            ; disk device pointer
  entries: resq 1                         ; entries on the disk
  entrysize: resq 1                       ; entry size
  blockWrapper: resq 2                    ; block structure wrapper
  block: resb 512                         ; block data
  count: resq 1                           ; block size

section .text                             ; Program data

global Program                            ; Make "Program" visible outside
Program:                                  ; Start of Program
  enter 0, 0                              ; Start new stack frame
  mov [disk], rdi                         ; Store disk pointer in variable

  ; Get the size of the disk
  mov rdi, [disk]                         ; (disk, &entries, &entrysize)
  mov rsi, entries                        ; ---
  mov rdx, entrysize                      ; ---
  call Entries                            ; Get disk specifications

  ; Set up block structure
  mov rax, block                          ; blockWrapper.byteArray = block
  mov [blockWrapper], rax                 ; ---
  mov rax, [entrysize]                    ; blockWrapper.byteArrayLength = entrysize
  mov [blockWrapper + 8], rax             ; ---

  ; Read from disk
  mov rdi, [disk]                         ; (disk, 0, blockWrapper)
  mov rax, 0                              ; ---
  movq xmm0, rax                          ; ---
  mov rsi, blockWrapper                   ; ---
  call Read                               ; Read block from disk

  ; Print contents to screen
  mov qword [count], 0                    ; counter = 0
  loop:
    mov rax, [count]                      ; (block[count])
    mov al, [block + rax]                 ; ---
    movzx rdi, al                         ; ---
    call putchar                          ; print character

    inc qword [count]                     ; while count < 13
    cmp qword [count], 13                 ; ---
  jne loop                                ; ---

  mov rdi, `\n`                           ; print newline, \n
  call putchar                            ; ---

  leave                                   ; Done with function
  ret                                     ; Return

