#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

struct metadata {
    size_t size;
    int freed;
    struct metadata* nextMetadata;
};
#define META_SIZE sizeof(struct metadata)
#define PAGE_SIZE 4096

static void* memory_mmapped = NULL;
static struct metadata* memoryHead = NULL;
static struct metadata* memoryTail = NULL;

void initMalloc() {
    memory_mmapped = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (memory_mmapped == NULL) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /*  || HEAD |  ***     free memory     ***  | TAIL || */

    memoryHead = (struct metadata*)memory_mmapped;
    memoryTail = (struct metadata*)(memory_mmapped + PAGE_SIZE - META_SIZE);

    memoryHead->size = PAGE_SIZE - META_SIZE * 2;
    memoryHead->freed = 1;
    memoryHead->nextMetadata = (struct metadata*)(memory_mmapped + PAGE_SIZE - META_SIZE);

    memoryTail->size = PAGE_SIZE - META_SIZE * 2;
    memoryTail->freed = 1;
    memoryTail->nextMetadata = (struct metadata*)memory_mmapped;

    printf("meta size %u\n", META_SIZE);

    printf("Debuggin initMalloc()\n");
    printf("memory mmapped: %p\n", memory_mmapped);
    printf("memoryHead address %p   size %u     nextAddress %p\n", memoryHead, memoryHead->size, memoryHead->nextMetadata);
    printf("memoryTail address %p   size %u     nextAddress %p\n", memoryTail, memoryTail->size, memoryTail->nextMetadata);
    printf("check for size: %u\n", (memoryTail - memoryHead) * META_SIZE - META_SIZE);
    printf("\n");
}

void* my_malloc(size_t size) {
    if (size == 0) return NULL;
    if (memory_mmapped == NULL) {
        initMalloc();
    }

    struct metadata* currBlock = memoryHead;
    //if currBlock is not free or there is not enough space go to next metadata
    while (!currBlock->freed || currBlock->size - 2 * META_SIZE < size) {
        currBlock = currBlock->nextMetadata;
        if (currBlock == memoryTail) {
            printf("There is no free memory space\n");
            return NULL;
        }
        void* tmpPtr = currBlock;
        currBlock = (struct metadata*)(tmpPtr + META_SIZE);
    }

    printf("Debugging my_malloc()\n");

/* || HEAD |  *** busy space ***  | mark || mark(currBlock) |  *** free space ***  | TAIL ||  */
    
/* || currBlock |  *** allocated memory ***  | currBlock->next || tmpHead |   *** free space ***  | tmpTail ||   */

    struct metadata* tmpTailBlock = currBlock->nextMetadata;
    size_t tmpSize = currBlock->size;

    void* ptrCurrBlock = currBlock;
    currBlock->freed = 0;
    currBlock->size = size;
    currBlock->nextMetadata = (struct metadata*)(ptrCurrBlock + META_SIZE + size);

    printf("CurrBlock address %p        CurrBlock size %u       CurrBlock next mark address %p\n", currBlock, currBlock->size, currBlock->nextMetadata);

    currBlock->nextMetadata->freed = 0;
    currBlock->nextMetadata->size = size;
    currBlock->nextMetadata->nextMetadata = currBlock;

    printf("nextCurrBlock address %p    nextCurrBlock size %u   nextCurrBlock next mark address %p\n", currBlock->nextMetadata, currBlock->nextMetadata->size, currBlock->nextMetadata->nextMetadata);
    printf("check for size: %u\n\n", (currBlock->nextMetadata - currBlock) * META_SIZE - META_SIZE);

    void* ptrNextCurrBlock = currBlock->nextMetadata;
    struct metadata* tmpHeadBlock = (struct metadata*)(ptrNextCurrBlock + META_SIZE);

    tmpHeadBlock->freed = 1;
    tmpHeadBlock->size = tmpSize - 2 * META_SIZE - currBlock->size;
    tmpHeadBlock->nextMetadata = tmpTailBlock;

    tmpTailBlock->freed = 1;
    tmpTailBlock->size = tmpSize - 2 * META_SIZE - currBlock->size;
    tmpTailBlock->nextMetadata = tmpHeadBlock;

    printf("tmpHead address %p     tmpHead size %u     tmpHead next mark address %p\n", tmpHeadBlock, tmpHeadBlock->size, tmpHeadBlock->nextMetadata);
    printf("tmpTail address %p     tmpTail size %u     tmpTail next mark address %p\n", tmpTailBlock, tmpTailBlock->size, tmpTailBlock->nextMetadata);
    printf("check for size: %u\n", (tmpTailBlock - tmpHeadBlock) * META_SIZE - META_SIZE);
    
    printf("\nreturned value %p\n\n\n", ptrCurrBlock + META_SIZE);

    return ptrCurrBlock + META_SIZE;
}

void my_free(void* ptr) {
    if (ptr == NULL) {
        printf("Calling my_free() on a NULL pointer\n");
        return;
    }

    printf("Debugging my_free()\n");
    printf("gotten address %p\n", ptr);

    struct metadata* currBlock = (ptr - META_SIZE);

    struct metadata* tmpTailBlock = currBlock->nextMetadata;

    printf("CurrBlock address %p    next mark address %p\n", currBlock, tmpTailBlock);

    currBlock->freed = 1;
    tmpTailBlock->freed = 1;

    size_t tmpSizeCurr = currBlock->size;
    void* ptrCurrBlock = currBlock;
    if (ptrCurrBlock == memory_mmapped) {
        printf("\n\n");
        return;
    }

    struct metadata* tmpHeadBlock = (struct metadata*)(ptrCurrBlock - META_SIZE);

    printf("is prev freed? = %u\n", tmpHeadBlock->freed);

    if (tmpHeadBlock->freed == 0) {
        printf("\n\n");
        return;
    }

    tmpHeadBlock = tmpHeadBlock->nextMetadata;
    size_t tmpSize = tmpHeadBlock->size;

    tmpHeadBlock->size = tmpSize + tmpSizeCurr + 2 * META_SIZE;
    tmpHeadBlock->nextMetadata = tmpTailBlock;

    tmpTailBlock->size = tmpSize + tmpSizeCurr + 2 * META_SIZE;
    tmpTailBlock->nextMetadata = tmpHeadBlock;

    printf("tmpHead address %p     tmpHead size %u     tmpHead next mark address %p\n", tmpHeadBlock, tmpHeadBlock->size, tmpHeadBlock->nextMetadata);
    printf("tmpTail address %p     tmpTail size %u     tmpTail next mark address %p\n", tmpTailBlock, tmpTailBlock->size, tmpTailBlock->nextMetadata);

    printf("\n\n");
}   

void freeAll() {
    if (munmap(memory_mmapped, PAGE_SIZE) != 0) {
        perror("mumap()");
    }
}

int main() {
    char* ptr = (char*)my_malloc(16);
    printf("\n\n");
    char* ptr2 = (char*)my_malloc(16);
    printf("\n\n");
    char* ptr3 = (char*)my_malloc(16);
    my_free(ptr2);
    my_free(ptr3);
    freeAll();
    return 0;
}