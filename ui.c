
#include "ui.h"
#include "huffman.h"
#include "file_utils.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP(ms) usleep(ms * 1000)
#endif

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pressAnyKey() {
    printf("\nPress any key to continue...");
    getch();
    printf("\n");
}

void printHeader() {
    clearScreen();
    printf("=========================================\n");
    printf("        SIMPLE FILE COMPRESSOR\n");
    printf("=========================================\n");
}

int getMenuChoice() {
    int choice;
    printf("\nMain Menu:\n");
    printf("1. Compress File\n");
    printf("2. Decompress File\n");
    printf("3. Show File Information\n");
    printf("4. Compare File Sizes\n");
    printf("5. Exit\n");
    printf("\nEnter your choice (1-5): ");

    while(scanf("%d", &choice) != 1 || choice < 1 || choice > 5) {
        printf("Invalid input! Please enter a number between 1-5: ");
        while(getchar() != '\n'); // Clear input buffer
    }
    return choice;
}

void getFilenames(char* inputFile, char* outputFile, int mode) {
    printf("\nEnter input filename: ");
    scanf("%255s", inputFile);

    if(mode == 1) { // Compression
        printf("Enter output filename (or press enter for automatic): ");
        getchar(); // Clear newline
        if(fgets(outputFile, 256, stdin) != NULL) {
            outputFile[strcspn(outputFile, "\n")] = 0; // Remove newline
            if(strlen(outputFile) == 0) {
                sprintf(outputFile, "%s.huff", inputFile);
            }
        }
    } else { // Decompression
        printf("Enter output filename (or press enter for automatic): ");
        getchar(); // Clear newline
        if(fgets(outputFile, 256, stdin) != NULL) {
            outputFile[strcspn(outputFile, "\n")] = 0; // Remove newline
            if(strlen(outputFile) == 0) {
                // Remove .huff extension if present
                if(strstr(inputFile, ".huff")) {
                    strcpy(outputFile, inputFile);
                    char* ext = strstr(outputFile, ".huff");
                    *ext = '\0';
                } else {
                    sprintf(outputFile, "decompressed_%s", inputFile);
                }
            }
        }
    }
}

void showProgress(int current, int total, const char* operation) {
    int barWidth = 50;
    float progress = (float)current / total;
    int pos = barWidth * progress;

    printf("\r%s [", operation);
    for(int i = 0; i < barWidth; ++i) {
        if(i < pos) printf("=");
        else if(i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%", (int)(progress * 100));
    fflush(stdout);
}

void compressWithProgress(const char* inputFile, const char* outputFile) {
    printf("\nCompressing '%s' to '%s'...\n", inputFile, outputFile);

    // Get file size for progress
    long inputSize = getFileSize(inputFile);
    if(inputSize <= 0) {
        printf("Error: Could not determine file size\n");
        return;
    }

    // Simulate progress - in real implementation, you'd update this during compression
    for(int i = 0; i <= 100; i += 10) {
        showProgress(i, 100, "Compressing");
        SLEEP(100);
    }
    printf("\n");

    // Actual compression
    compressFile(inputFile, outputFile);

    // Show compression results
    long outputSize = getFileSize(outputFile);
    if(outputSize > 0) {
        float ratio = ((float)outputSize / inputSize) * 100;
        printf("\nCompression Results:\n");
        printf("Original size: %ld bytes\n", inputSize);
        printf("Compressed size: %ld bytes\n", outputSize);
        printf("Compression ratio: %.2f%%\n", ratio);
        printf("Space saved: %ld bytes\n", inputSize - outputSize);
    }
}

void showMainMenu() {
    int choice;
    char inputFile[256], outputFile[256];

    while(1) {
        printHeader();
        choice = getMenuChoice();

        switch(choice) {
            case 1: // Compress
                printHeader();
                printf("COMPRESS FILE\n");
                getFilenames(inputFile, outputFile, 1);
                compressWithProgress(inputFile, outputFile);
                pressAnyKey();
                break;

            case 2: // Decompress
                printHeader();
                printf("DECOMPRESS FILE\n");
                getFilenames(inputFile, outputFile, 2);
                decompressFile(inputFile, outputFile);
                pressAnyKey();
                break;

            case 3: // File Info
                printHeader();
                printf("Enter filename: ");
                scanf("%255s", inputFile);
                getFileInfo(inputFile);
                pressAnyKey();
                break;

            case 4: // Compare Files
                printHeader();
                printf("Enter first filename: ");
                scanf("%255s", inputFile);
                printf("Enter second filename: ");
                scanf("%255s", outputFile);
                compareFiles(inputFile, outputFile);
                pressAnyKey();
                break;

            case 5: // Exit
                printHeader();
                printf("Thank you for using Simple File Compressor!\n");
                printf("Goodbye!\n");
                return;
        }
    }
}
