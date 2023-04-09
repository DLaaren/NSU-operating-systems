#include <stdio.h>
#include <unistd.h>

int globalVarUninit;
int globalVarInit = 12345;
const int globalConstVar = 1313;

void foo(int i) {
    int localVarUninit;
    int localVarInit = i;
    static int localStaticVar = 1984;
    const int localConstVar = i;

    printf("localVarUninit: %d; its address: %p\n", localVarUninit, &localVarUninit);
    printf("localVarInit: %d; its address: %p\n", localVarInit, &localVarInit);
    printf("localStaticVar: %d; its address: %p\n", localStaticVar, &localStaticVar);
    printf("localConstVar: %d; its address: %p\n", localConstVar, &localConstVar);
    printf("globalVarUninit: %d; its address: %p\n", globalVarUninit, &globalVarUninit);
    printf("globalVarInit: %d; its address: %p\n", globalVarInit, &globalVarInit);
    printf("globalConstVar: %d; its address: %p\n", globalConstVar, &globalConstVar);
    printf("\n");
}

int main() {
    printf("PID: %d\n\n", getpid());
    for (int i = 0; i < 3; i++) {
        foo(i);
    }
    while(1) {
        sleep(10);
    }
    return 0;
}