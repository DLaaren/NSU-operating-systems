.data

msg:
    .ascii "Hello World from assembly!\n"

.text
    .global _start

_start:
    movq $4, %rax       #system call number (sys_write)
    movq $1, %rbx       #file descriptor
    movq $msg, %rcx     #message
    movq $28, %rdx      #length
    int $0x80           #interrupt number --- just syscall in our case

    movq $1, %rax       #system call number (sys_exit)
    xorq %rbx, %rbx     #just cast to zero
    xorq %rcx, %rcx
    xorq %rdx, %rdx
    int $0x80
