//Анонимная память — это чисто оперативная память (RAM). (не привязана ни к какому файлу)
//#pragma pack(1)  //to disable pudding
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

struct metadata {
    size_t size;
    char freed;
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

    memoryTail->size = PAGE_SIZE - META_SIZE * 2;
    memoryTail->freed = 1;

    printf("Debuggin initMalloc()\n");
    printf("memory mmapped: %p\n", memory_mmapped);
    printf("memoryHead address %p   size %u\n", memoryHead, memoryHead->size);
    printf("memoryTail address %p   size %u\n", memoryTail, memoryTail->size);
    printf("check for size: %u\n", (memoryTail - memoryHead) * META_SIZE - META_SIZE);
    printf("\n\n");
}

void* my_malloc(size_t size) {
    if (size == 0) return NULL;
    if (memory_mmapped == NULL) {
        initMalloc();
    }

    struct metadata* currBlock = memoryHead;
    //if currBlock is not free or there is not enough space go to next metadata
    while (!currBlock->freed || currBlock->size + 2 * META_SIZE < size) {
        void* tmp = currBlock;
        currBlock = (struct metadata*)(tmp + META_SIZE + currBlock->size);
        if (currBlock == memoryTail) {
            printf("There is no free memory space\n");
            return NULL;
        }
        currBlock = (struct metadata*)(tmp + 2 * META_SIZE + currBlock->size);
    }

    printf("Debugging my_malloc()\n");

    void* currBlock_ptr = currBlock;
    struct metadata* tmpTailBlock = (struct metadata*)(currBlock_ptr + META_SIZE + currBlock->size);
    size_t tmpSize = currBlock->size;

    currBlock->freed = 0;
    currBlock->size = size;

    printf("CurrBlock address %p        CurrBlock size %u\n", currBlock, currBlock->size);

    struct metadata* endCurrBlock = (struct metadata*)(currBlock_ptr + META_SIZE + size);
    endCurrBlock->freed = 0;
    endCurrBlock->size = size;

    printf("nextCurrBlock address %p    nextCurrBlock size %u\n", endCurrBlock, endCurrBlock->size);
    printf("check for size: %u\n\n", (endCurrBlock - currBlock) * META_SIZE - META_SIZE);

    void* endCurrBlock_ptr = endCurrBlock;
    struct metadata* tmpHeadBlock = (struct metadata*)(endCurrBlock_ptr + META_SIZE);

    tmpHeadBlock->freed = 1;
    tmpHeadBlock->size = tmpSize - 2 * META_SIZE - currBlock->size;

    tmpTailBlock->freed = 1;
    tmpTailBlock->size = tmpSize - 2 * META_SIZE - currBlock->size;

    printf("tmpHead address %p     tmpHead size %u\n", tmpHeadBlock, tmpHeadBlock->size);
    printf("tmpTail address %p     tmpTail size %u\n", tmpTailBlock, tmpTailBlock->size);
    printf("check for size: %u\n", (tmpTailBlock - tmpHeadBlock) * META_SIZE - META_SIZE);
    
    printf("\nreturned value %p\n\n\n", currBlock_ptr + META_SIZE);

    return currBlock_ptr + META_SIZE;
}

void my_free(void* ptr) {
    if (ptr == NULL) {
        printf("Calling my_free() with NULL pointer\n");
        return;
    }

    printf("Debugging my_free()\n");
    printf("gotten address %p\n", ptr);

    struct metadata* currBlock = (struct metadata*)(ptr - META_SIZE);

    void* currBlock_ptr = currBlock;
    struct metadata* endCurrBlock = (struct metadata*)(currBlock_ptr + META_SIZE + currBlock->size);
    void* endCurrBlock_ptr = endCurrBlock;

    currBlock->freed = 1;
    endCurrBlock->freed = 1;

    printf("CurrBlock address %p    next mark address %p\n", currBlock, endCurrBlock);

    if (currBlock_ptr == memory_mmapped && endCurrBlock_ptr == memory_mmapped + PAGE_SIZE - META_SIZE) {
        printf("\n\n");
        return;
    }

    struct metadata* prevTail;
    struct metadata* nextHead;
    if (currBlock_ptr != memory_mmapped) {
        prevTail = (struct metadata*)(currBlock_ptr - META_SIZE);
        printf("is prevBlock freed? = %u\n", prevTail->freed);
    }
    if (endCurrBlock_ptr != memory_mmapped + PAGE_SIZE - META_SIZE) {
        nextHead = (struct metadata*)(endCurrBlock_ptr + META_SIZE);
        printf("is nextBlock freed? = %u\n", nextHead->freed);
    }

    size_t sizeCurrBlock = currBlock->size;
    struct metadata* prevHead;
    if (prevTail->freed == 1 && currBlock_ptr != memory_mmapped) {
        size_t sizePrev = prevTail->size;
        void* prevTail_ptr = prevTail;
        prevHead = (struct metadata*)(prevTail_ptr - sizePrev - META_SIZE);
        
        currBlock = prevHead;
        currBlock->freed = 1;
        endCurrBlock->freed = 1;
        currBlock->size = sizePrev + 2 * META_SIZE + sizeCurrBlock;
        endCurrBlock->size = sizePrev + 2 * META_SIZE + sizeCurrBlock;
    }

    struct metadata* nextTail;
    if (nextHead->freed == 1 && endCurrBlock_ptr != memory_mmapped + PAGE_SIZE - META_SIZE) {
        size_t sizeNext = nextHead->size;
        void* nextHead_ptr = nextHead;
        nextTail = (struct metadata*)(nextHead_ptr + META_SIZE + sizeNext);

        endCurrBlock = nextTail;
        currBlock->freed = 1;
        endCurrBlock->freed = 1;
        currBlock->size = sizeCurrBlock + 2 * META_SIZE + sizeNext;
        endCurrBlock->size = sizeCurrBlock + 2 * META_SIZE + sizeNext;
    }

    printf("currBlock address %p     currBlock size %u\n", currBlock, currBlock->size);
    printf("endCurrBlock address %p     endCurrBlock size %u\n", endCurrBlock, endCurrBlock->size);

    printf("\n\n");
} 

void freeAll() {
    if (munmap(memory_mmapped, PAGE_SIZE) != 0) {
        perror("mumap()");
    }
}

int main() {
    printf("%d %d %d\n\n\n", sizeof(size_t), sizeof(char), sizeof(struct metadata));
    printf("meta size %u\n", META_SIZE);
    printf("page size %u\n", PAGE_SIZE);
    char* ptr = (char*)my_malloc(16);
    printf("\n");
    char* ptr2 = (char*)my_malloc(16);
    printf("\n");
    char* ptr3 = (char*)my_malloc(16);
    my_free(ptr2);
    my_free(ptr3);
    freeAll();
    return 0;
}