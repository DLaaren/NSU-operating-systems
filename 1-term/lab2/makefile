all: hello_simple hello_syscall hello_wrapped_syscall without_syscall_v1 without_syscall_v2 hello_ptrace

hello_simple:
	gcc simple-hello.c -o simple-hello
hello_syscall:
	gcc hello-syscall.c -o hello-syscall
hello_wrapped_syscall:
	gcc hello-wrapped-syscall.c -o hello-wrapped-syscall
without_syscall_v1:
	gcc -c without-syscall-v1.s -o v1.o
	ld v1.o -o v1
	rm v1.o
without_syscall_v2:
	gcc -c without-syscall-v2.s -o v2.o
	ld v2.o -o v2
	rm v2.o
hello_ptrace:
	gcc hello-ptrace.c -o hello-ptrace
