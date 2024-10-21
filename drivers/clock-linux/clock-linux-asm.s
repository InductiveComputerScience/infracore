.intel_syntax noprefix

.global sys_clock_getres
.global sys_clock_gettime

sys_clock_getres:
	mov rax, 229
	syscall
	ret

sys_clock_gettime:
	mov rax, 228
	syscall
	ret

