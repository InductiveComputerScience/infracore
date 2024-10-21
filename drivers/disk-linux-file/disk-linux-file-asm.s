.intel_syntax noprefix

.global sys_read
.global sys_write
.global sys_open
.global sys_close
.global sys_lseek
.global sys_fsync

sys_read:
	mov rax, 0
	syscall
	ret

sys_write:
	mov rax, 1
	syscall
	ret

sys_open:
	mov rax, 2
	syscall
	ret

sys_close:
	mov rax, 3
	syscall
	ret

sys_lseek:
	mov rax, 8
	syscall
	ret

sys_fsync:
	mov rax, 74
	syscall
	ret

