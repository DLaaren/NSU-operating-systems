#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <glob.h>

void swap(char* a, char* b) {
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

int sizeOfString(char* string) {
    int length = 0;
    while (string[length] != '\0') {
        length++;
    }
    return length;
}

void reverseFileName(char* pathname, char* result) {
    char* tmp = malloc(255);
    int pathLen = sizeOfString(pathname);
    for (int i = 0; i < sizeOfString(pathname); i++) {
        result[i] = pathname[i];
        tmp[i] = pathname[i];
    }
    char* revDirName;
    char* revFileName;
    char* newToken = strtok(tmp, "/");
    revDirName = newToken;
    newToken = strtok(NULL, "/");
    revFileName = newToken;
    while (1) {
        newToken = strtok(NULL, "/");
        if (newToken == NULL) {
            break;
        }
        revDirName = revFileName;
        revFileName = newToken;
    }
    free(tmp);

    int fileNameLen = sizeOfString(revFileName);
    int dirNameLen = sizeOfString(revDirName);
    for (int i = 0; i < dirNameLen / 2; i++) {
        swap(&revDirName[i], &revDirName[dirNameLen - i - 1]);
    }

    for (int i = 0; i < fileNameLen / 2; i++) {
        swap(&revFileName[i], &revFileName[fileNameLen - i - 1]);
    }

    for (int i = pathLen - dirNameLen - fileNameLen - 1, j = 0; j < dirNameLen && i < pathLen; i++, j++) {
        result[i] = revDirName[j];
    }
    for (int i = pathLen - fileNameLen, j = 0; j < fileNameLen && i < pathLen; i++, j++) {
        result[i] = revFileName[j];
    }
    printf("path\n%s\n\n", pathname);
    printf("result\n%s\n\n", result);
}

void reverseDirName(char* pathname, char* result) {
    for (int i = 0; i < sizeOfString(pathname); i++) {
        result[i] = pathname[i];
    }
    char* revDirName;
    char* newToken = strtok(pathname, "/");
    while (newToken != NULL) {
        revDirName = newToken;
        newToken = strtok(NULL, "/");
    }

    int dirNameLen = sizeOfString(revDirName);
    for (int i = 0; i < dirNameLen / 2; i++) {
        swap(&revDirName[i], &revDirName[dirNameLen - i - 1]);
    }

    int pathLen = sizeOfString(result);
    for (int i = pathLen - dirNameLen, j = 0; j < dirNameLen && i < pathLen; i++, j++) {
        result[i] = revDirName[j];
    }
}

int copyFile(char* fileName) {
    FILE* file = fopen(fileName,"r");
    if (file == NULL) {
        perror("fopen(file) ");
        return 1;
    }

    char* reversedFileName = malloc(255);
    reverseFileName(fileName, reversedFileName);
    FILE* reversedFile = fopen(reversedFileName, "w");
    if (reversedFile == NULL) {
        perror("fopen(reversedFile) ");
        free(reversedFileName);
        return 2;
    }

    //find the end of "file"
    if (fseek(file, 0L, SEEK_END) != 0) {
        perror("fseek()");
        fclose(file);
        fclose(reversedFile);
        free(reversedFileName);
        return 3;
    }

    //read from the end and write in the beginning
    long int position = ftell(file) - 1; 
    while (position >= 0L) {
        if (fseek(file, position, SEEK_SET) != 0) {
            fclose(file);
            fclose(reversedFile);
            perror("fseek() ");
            free(reversedFileName);
            return 4;
        }
        char byte = fgetc(file);
        if (byte == EOF) {
            fclose(file);
            fclose(reversedFile);
            perror("fseek() ");
            free(reversedFileName);
            return 5;
        }
        if (fputc(byte, reversedFile) == EOF) {
            fclose(file);
            fclose(reversedFile);
            perror("fseek() ");
            free(reversedFileName);
            return 6;
        }
        position--;
    }

    if (fclose(file) != 0 || fclose(reversedFile) != 0) {
        perror("fclose() ");
        free(reversedFileName);
        return 7;
    }

    free(reversedFileName);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Enter an absolute path to your directory\n");
        exit(EXIT_FAILURE);
    }

    //copy pathname and copy pathname to create pattern
    int pathLen = sizeOfString(argv[1]);
    int flag = 0;
    if (*(argv[1] + pathLen - 1) == '/') {
        flag++;
    }
    char* pathname = malloc(255);
    char* pathnamePattern = malloc(255);
    for (int i = 0; i < pathLen - flag; i++) {
        pathname[i] = *(argv[1] + i);
        pathnamePattern[i] = *(argv[1] + i);
    }
    
    //open directory
    DIR* dir = opendir(pathname);
    if (dir == NULL) {
        perror("opendir() ");
        free(pathname);
        free(pathnamePattern);
        exit(EXIT_FAILURE);
    }

    //reverse last argument in pathname
    char* reversedDirName = malloc(255);
    reverseDirName(pathname, reversedDirName);
    free(pathname);

    //make directory with reversed name
    if (mkdir(reversedDirName, /*S_IROTH | S_IWOTH | S_IXOTH |*/ S_IREAD | S_IWRITE | 
                                S_IEXEC | S_IRGRP | S_IWGRP | S_IXGRP) == -1) {
        perror("opendir() ");
        free(reversedDirName);
        exit(EXIT_FAILURE);
    }

    //create pathname pattern
    pathnamePattern[pathLen - flag] = '/';
    pathnamePattern[pathLen - flag + 1] = '*';

    //look for files in the directory
    glob_t globBuf;
    if (glob(pathnamePattern, 0, NULL, &globBuf) != 0) {
        perror("glob() ");
        free(pathnamePattern);
        globfree(&globBuf);
        exit(EXIT_FAILURE);
    }
    
    //copy all regular files
    for (int i = 0; i < globBuf.gl_pathc; i++) {
        struct stat fileStat;
        if (stat(globBuf.gl_pathv[i], &fileStat) != 0) {
            perror("stat() ");
            free(pathnamePattern);
            globfree(&globBuf);
            exit(EXIT_FAILURE);
        }
        if (S_ISREG(fileStat.st_mode) != 1) {
            continue;
        }
        if (copyFile(globBuf.gl_pathv[i]) != 0) {
            free(pathnamePattern);
            globfree(&globBuf);
            exit(EXIT_FAILURE);
        }
    }

    //close directory
    free(reversedDirName);
    if (closedir(dir) == -1) {
        perror("closedir() ");
        free(pathnamePattern);
        globfree(&globBuf);
        exit(EXIT_FAILURE);
    }

    globfree(&globBuf);

    return 0;
}