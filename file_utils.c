#include "file_utils.h"
#include <stdio.h>
#include <string.h>

long getFileSize(const char* filename) {
    struct stat st;
    if(stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

int fileExists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

void getFileInfo(const char* filename) {
    long size = getFileSize(filename);
    if(size >= 0) {
        printf("\nFile Information:\n");
        printf("Filename: %s\n", filename);
        printf("Size: %ld bytes\n", size);
        printf("Size: %.2f KB\n", size / 1024.0);
        printf("Size: %.2f MB\n", size / (1024.0 * 1024.0));
    } else {
        printf("Error: Could not get file information for '%s'\n", filename);
    }
}

void compareFiles(const char* file1, const char* file2) {
    long size1 = getFileSize(file1);
    long size2 = getFileSize(file2);

    if(size1 >= 0 && size2 >= 0) {
        printf("\nFile Size Comparison:\n");
        printf("File 1: %s - %ld bytes\n", file1, size1);
        printf("File 2: %s - %ld bytes\n", file2, size2);
        printf("Difference: %ld bytes\n", (size1 > size2) ? (size1 - size2) : (size2 - size1));

        if(size1 > 0) {
            float ratio = ((float)size2 / size1) * 100;
            printf("Size ratio: %.2f%%\n", ratio);
        }
    } else {
        printf("Error: Could not get file information\n");
    }
}
