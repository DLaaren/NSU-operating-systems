#include <stdio.h>
#include <unistd.h>

int* foo(int c) {
    int var;
    var = c;
    int *p = &var;
    //printf("address %p\n", p);
    return p;
    //if return &var --> return nullptr
    //it is the way to pass through the warning
}
int main() {
    printf("pid %d\n", getpid());
    int* p = foo(123);

    printf("address %p\n", p);
    printf("data %d\n", *p);
    return 0;
}