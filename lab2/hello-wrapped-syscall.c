#include <sys/syscall.h>
#include <unistd.h>

// syscall() is a small library function that invokes the system
// call whose assembly language interface has the specified number
// with the specified arguments.  Employing syscall() is useful, for
// example, when invoking a system call that has no wrapper function
// in the C library.

// System constants are CPU-wide unique, global constants that
// are required and automatically created by the system.

size_t mySYSwrite(unsigned int fd, const char *buf, size_t size) {
    return syscall(SYS_write, fd, buf, size);
}

int main() {
    //use the same arguments but add SYS_write -- system constants
    mySYSwrite(1, "Hello from syscall/write!\n", 27);
    return 0;
}