#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//file contains 64-bits value for each virtual page
#define ENTRY_SIZE 8

#define GET_BIT(X,Y) (X & ((__uint64_t)1<<Y)) >> Y
#define GET_PFN(X) X & 0x7FFFFFFFFFFFFF

const int __endian_bit = 1;
#define IS_BIG_ENDIAN() ( (*(char*)&__endian_bit) == 0)


void readPagemap(char* fileName, unsigned long int virtualAddress) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        perror("fopen() ");
        return;
    }

    __uint64_t offset = (virtualAddress / getpagesize()) * ENTRY_SIZE;
   if(fseek(file, offset, SEEK_SET) != 0){
      perror("fseek() ");
      return;
   }

    int c;
    unsigned char valueBuf[ENTRY_SIZE];
    if (IS_BIG_ENDIAN()) {
        printf("Big endian\n");
        for (int i = 0; i < ENTRY_SIZE; i++) {
            c = getc(file);
            valueBuf[i] = c;
        }
    } else {
        printf("Little endian\n");
        for (int i = 0; i < ENTRY_SIZE; i++) {
            c = getc(file);
            valueBuf[ENTRY_SIZE - 1 - i] = c;
        }
    }

    printf("Virtual address: 0x%lx, Page size: %d, Entry size: %d\n", virtualAddress, getpagesize(), ENTRY_SIZE);

    __uint64_t readValue = 0;
    for (int i = 0; i < ENTRY_SIZE; i++) {
        readValue = (readValue << 8) + valueBuf[i];
    }
    printf("Result 0x%llx\n", (unsigned long long int) readValue);
    
    if (GET_BIT(readValue, 63)) {
        printf("Page frame number (PFN) is 0x%llx\n", (unsigned long long int) GET_PFN(readValue)); 
        if (GET_BIT(readValue, 62)) {
            printf("Page is swapped\n");
        }
    } else {
        printf("Page is not in RAM\n");
    }
    if (fclose(file) != 0) {
        perror("fclose() ");
        return;
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Incorrect arguments\nEnter pagemap PID and virtual address\n");
        exit(EXIT_FAILURE);
    }

    int pid;
    unsigned long int virtualAddress;
    char* fileName = malloc(256);
    char* end;

    //memcmp -- compare memory areas
    if (memcmp(argv[1], "self", sizeof("self")) == 0) {
        sprintf(fileName, "/proc/self/pagemap");
    } else {
        //strtol(const char *start, char **end, int radix) -- converts string representation of number to int
        pid = strtol(argv[1], &end, 10);
        if (pid <= 0 | *end != '\0' | end == argv[1]) { //check these
            printf("PID msut be a positiv number\n");
            free(fileName);
            exit(EXIT_FAILURE);
        }
        sprintf(fileName, "/proc/%lu/pagemap", pid);
    }

    virtualAddress = strtol(argv[2], NULL, 16);

    readPagemap(fileName, virtualAddress);

    free(fileName);
    return 0;
}