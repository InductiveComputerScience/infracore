.intel_syntax noprefix                         # Let's use intel syntax

.bss                                           # Global variables initialized to 0

.lcomm pixels, 640*480*4                       # The image to be displayed
                                               # color bytes: R, G, B, A
screen: .quad 0                                # The pointer to the screen device
counter: .quad 0                               # Loop counter variable

.text                                          # Program data

.global Program                                # Make "Program" visible outside
Program:                                       # Start of Program
  enter 0, 0                                   # Start new stack frame
  mov [screen], rdi                            # Store screen pointer in variable

  mov qword ptr [counter], 60*4                # Loop this many times
  loop:                                        # Start of loop
    # Draw pixel
    mov rdx, qword ptr [counter]               # Pixel Row
    mov rax, qword ptr [counter]               # Pixel Column
    imul rax, 640                              # Computer pixel position in array
    add rax, rdx
    mov byte ptr [pixels + rax*4 + 0], 255     # Red
    mov byte ptr [pixels + rax*4 + 1], 255     # Green
    mov byte ptr [pixels + rax*4 + 2], 255     # Blue

    # Display image on screen
    mov rdi, [screen]                          # Set up parameters: (screen, pixels)
    lea rsi, [pixels]
    call Display                               # Ask image to be displayed on screen

    mov rdi, [screen]                          # Set up parameters: (screen)
    call Synchronize                           # Wait for the draw to complete

    dec qword ptr [counter]                    # Decrement loop variable
  jnz loop                                     # If zero, quit loop

  leave                                        # Done with function
  ret                                          # Return


